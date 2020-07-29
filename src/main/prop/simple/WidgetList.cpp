/*
 * WidgetList.cpp
 *
 *  Created on: 29 июл. 2020 г.
 *      Author: sadko
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
            sList.flush();

            pMeta       = NULL;
            pCListener  = NULL;
        }

        status_t GenericWidgetList::add(Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (sList.index_of(w) >= 0)
                return STATUS_ALREADY_EXISTS;
            if (!sList.add(w))
                return STATUS_NO_MEM;

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
            if (!sList.premove(w))
                return STATUS_NOT_FOUND;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->remove(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::remove(size_t index)
        {
            Widget *w = sList.remove(index);
            if (w == NULL)
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->remove(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::remove(size_t index, size_t count)
        {
            lltl::parray<Widget> removed;
            if (!sList.remove_n(index, count, &removed))
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pCListener != NULL)
            {
                for (size_t i=0, n=removed.size(); i<n; ++i)
                {
                    Widget *w = removed.uget(i);
                    pCListener->remove(this, w);
                }
            }
            if ((pListener != NULL) && (removed.size() > 0))
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetList::insert(Widget *w, size_t index)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (sList.index_of(w) >= 0)
                return STATUS_ALREADY_EXISTS;
            if (!sList.insert(index, w))
                return STATUS_NO_MEM;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->add(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        void GenericWidgetList::clear()
        {
            lltl::parray<Widget> removed;
            removed.swap(&sList);

            // Notify listeners
            if (pCListener != NULL)
            {
                for (size_t i=0, n=removed.size(); i<n; ++i)
                {
                    Widget *w = removed.uget(i);
                    pCListener->remove(this, w);
                }
            }
            if ((pListener != NULL) && (removed.size() > 0))
                pListener->notify(this);
        }

        status_t GenericWidgetList::set(Widget *w, size_t index)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            ssize_t idx = sList.index_of(w);
            if (idx >= 0)
                return (size_t(idx) == index) ? STATUS_OK : STATUS_ALREADY_EXISTS;
            Widget *old = sList.get(idx);
            if (!sList.set(index, w))
                return STATUS_INVALID_VALUE;

            // Notify listeners
            if (pCListener != NULL)
            {
                if (old != NULL)
                    pCListener->remove(this, old);
                pCListener->add(this, w);
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
            lltl::parray<Widget> t1, t2;
            t1.swap(&sList);
            t2.swap(&dst->sList);

            // Notify listeners for removal
            if (pCListener != NULL)
            {
                for (size_t i=0, n=t1.size(); i<n; ++i)
                {
                    Widget *w   = t1.uget(i);
                    pCListener->remove(this, w);
                }
            }
            if (dst->pCListener != NULL)
            {
                for (size_t i=0, n=t2.size(); i<n; ++i)
                {
                    Widget *w   = t2.uget(i);
                    dst->pCListener->remove(dst, w);
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
                    Widget *w   = sList.uget(i);
                    pCListener->add(this, w);
                }
            }
            if (dst->pCListener != NULL)
            {
                for (size_t i=0, n=dst->sList.size(); i<n; ++i)
                {
                    Widget *w   = dst->sList.uget(i);
                    dst->pCListener->add(dst, w);
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
