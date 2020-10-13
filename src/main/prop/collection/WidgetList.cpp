/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 июл. 2020 г.
 *
 * lsp-tk-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-tk-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-tk-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        GenericWidgetList::GenericWidgetList(const w_class_t *meta, prop::Listener *listener, prop::CollectionListener *clistener):
            Property(listener)
        {
            pMeta       = meta;
            pCListener  = clistener;
        }

        GenericWidgetList::~GenericWidgetList()
        {
            // Garbage-collect managed widgets
            for (size_t i=0, n=sList.size(); i<n; ++i)
            {
                item_t *xw  = sList.uget(i);
                if (xw->bManage)
                {
                    xw->pWidget->destroy();
                    delete xw->pWidget;
                }
            }

            sList.flush();

            pMeta       = NULL;
            pCListener  = NULL;
        }

        ssize_t GenericWidgetList::index_of(const Widget *w) const
        {
            for (size_t i=0, n=sList.size(); i<n; ++i)
            {
                item_t *xw  = sList.uget(i);
                if (xw->pWidget == w)
                    return i;
            }
            return -1;
        }

        Widget *GenericWidgetList::get(size_t index)
        {
            item_t *item = sList.get(index);
            return (item != NULL) ? item->pWidget : NULL;
        }

        status_t GenericWidgetList::add(Widget *w, bool manage)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (index_of(w) >= 0)
                return STATUS_ALREADY_EXISTS;

            item_t *item = sList.add();
            if (item == NULL)
                return STATUS_NO_MEM;

            item->pWidget = w;
            item->bManage = manage;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->add(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::premove(Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            ssize_t index = index_of(w);
            if (index < 0)
                return STATUS_NOT_FOUND;

            item_t xw;
            if (!sList.remove(index, &xw))
                return STATUS_UNKNOWN_ERR;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->remove(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            // Remove widget
            if (xw.bManage)
            {
                xw.pWidget->destroy();
                delete xw.pWidget;
            }

            return STATUS_OK;
        }

        status_t GenericWidgetList::remove(size_t index)
        {
            item_t xw;
            if (sList.remove(index, &xw))
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->remove(this, xw.pWidget);
            if (pListener != NULL)
                pListener->notify(this);

            // Remove widget
            if (xw.bManage)
            {
                xw.pWidget->destroy();
                delete xw.pWidget;
            }

            return STATUS_OK;
        }

        status_t GenericWidgetList::remove(size_t index, size_t count)
        {
            lltl::darray<item_t> removed;
            if (!sList.remove_n(index, count, &removed))
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pCListener != NULL)
            {
                for (size_t i=0, n=removed.size(); i<n; ++i)
                {
                    item_t *xw = removed.uget(i);
                    pCListener->remove(this, xw->pWidget);
                    if (xw->bManage)
                    {
                        xw->pWidget->destroy();
                        delete xw->pWidget;
                    }
                }
            }
            if ((pListener != NULL) && (removed.size() > 0))
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::insert(Widget *w, size_t index, bool manage)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (index_of(w) >= 0)
                return STATUS_ALREADY_EXISTS;

            item_t *item = sList.insert(index);
            if (item == NULL)
                return STATUS_NO_MEM;

            item->pWidget = w;
            item->bManage = manage;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->add(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        void GenericWidgetList::clear()
        {
            lltl::darray<item_t> removed;
            removed.swap(&sList);

            // Notify listeners
            if (pCListener != NULL)
            {
                for (size_t i=0, n=removed.size(); i<n; ++i)
                {
                    item_t *xw = removed.uget(i);
                    pCListener->remove(this, xw->pWidget);
                    if (xw->bManage)
                    {
                        xw->pWidget->destroy();
                        delete xw->pWidget;
                    }
                }
            }
            if ((pListener != NULL) && (removed.size() > 0))
                pListener->notify(this);
        }

        status_t GenericWidgetList::set(Widget *w, size_t index, bool manage)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            ssize_t idx = index_of(w);
            if (idx >= 0)
                return (size_t(idx) == index) ? STATUS_OK : STATUS_ALREADY_EXISTS;

            item_t *xw      = sList.get(idx);
            item_t old      = *xw;
            xw->pWidget     = w;
            xw->bManage     = manage;

            // Notify listeners
            if (pCListener != NULL)
            {
                if (old.pWidget != NULL)
                    pCListener->remove(this, old.pWidget);
                pCListener->add(this, w);
                if (xw->bManage)
                {
                    xw->pWidget->destroy();
                    delete xw->pWidget;
                }
            }
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::swap(GenericWidgetList *dst)
        {
            if (pMeta != dst->pMeta)
                return STATUS_BAD_TYPE;
            if (dst == this)
                return STATUS_OK;

            // Remove data from original lists
            lltl::darray<item_t> t1, t2;
            t1.swap(&sList);
            t2.swap(&dst->sList);

            // Notify listeners for removal
            if (pCListener != NULL)
            {
                for (size_t i=0, n=t1.size(); i<n; ++i)
                {
                    item_t *xw  = t1.uget(i);
                    pCListener->remove(this, xw->pWidget);
                }
            }
            if (dst->pCListener != NULL)
            {
                for (size_t i=0, n=t2.size(); i<n; ++i)
                {
                    item_t *xw  = t2.uget(i);
                    dst->pCListener->remove(dst, xw->pWidget);
                }
            }

            // Swap internal contents
            t2.swap(&sList);
            t1.swap(&dst->sList);

            // Notify listeners for adding
            if (pCListener != NULL)
            {
                for (size_t i=0, n=sList.size(); i<n; ++i)
                {
                    item_t *xw  = sList.uget(i);
                    pCListener->add(this, xw->pWidget);
                }
            }
            if (dst->pCListener != NULL)
            {
                for (size_t i=0, n=dst->sList.size(); i<n; ++i)
                {
                    item_t *xw  = dst->sList.uget(i);
                    dst->pCListener->add(dst, xw->pWidget);
                }
            }

            // Notify listeners for change
            if (pListener != NULL)
                pListener->notify(this);
            if (dst->pListener != NULL)
                dst->pListener->notify(dst);

            return STATUS_OK;
        }

        status_t GenericWidgetList::xswap(size_t i1, size_t i2)
        {
            if (!sList.xswap(i1, i2))
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }
    }
}
