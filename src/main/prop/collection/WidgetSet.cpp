/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 авг. 2020 г.
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
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {

        GenericWidgetSet::GenericWidgetSet(const w_class_t *meta, prop::Listener *listener, prop::CollectionListener *clistener):
            Property(listener)
        {
            pMeta       = meta;
            pCListener  = clistener;
        }

        GenericWidgetSet::~GenericWidgetSet()
        {
            sSet.flush();

            pMeta       = NULL;
            pCListener  = NULL;
        }

        status_t GenericWidgetSet::add(Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (sSet.contains(w))
                return STATUS_ALREADY_EXISTS;
            if (!sSet.put(w))
                return STATUS_NO_MEM;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->add(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetSet::toggle(Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (!sSet.toggle(w))
                return STATUS_NO_MEM;

            // Notify listeners
            if (pCListener != NULL)
            {
                if (sSet.contains(w))
                    pCListener->add(this, w);
                else
                    pCListener->remove(this, w);
            }
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        status_t GenericWidgetSet::remove(Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (!w->instance_of(pMeta))
                return STATUS_BAD_TYPE;
            if (!sSet.remove(w))
                return STATUS_NOT_FOUND;

            // Notify listeners
            if (pCListener != NULL)
                pCListener->remove(this, w);
            if (pListener != NULL)
                pListener->notify(this);

            return STATUS_OK;
        }

        void GenericWidgetSet::clear()
        {
            // Remove items
            lltl::parray<Widget> removed;
            if (!sSet.values(&removed))
                return;
            sSet.flush();

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

        status_t GenericWidgetSet::swap(GenericWidgetSet *dst)
        {
            if (pMeta != dst->pMeta)
                return STATUS_BAD_TYPE;
            if (dst == this)
                return STATUS_OK;

            // Remove data from original lists
            lltl::parray<Widget> t1, t2;
            if (!sSet.values(&t1))
                return STATUS_NO_MEM;
            if (!dst->sSet.values(&t2))
                return STATUS_NO_MEM;

            lltl::phashset<Widget> s1, s2;
            s1.swap(&sSet);
            s2.swap(&dst->sSet);

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
            s2.swap(&sSet);
            s1.swap(&dst->sSet);

            // Notify listeners for adding
            if (pCListener != NULL)
            {
                for (size_t i=0, n=t2.size(); i<n; ++i)
                {
                    Widget *w   = t2.uget(i);
                    pCListener->add(this, w);
                }
            }
            if (dst->pCListener != NULL)
            {
                for (size_t i=0, n=t1.size(); i<n; ++i)
                {
                    Widget *w   = t1.uget(i);
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
    
    } /* namespace tk */
} /* namespace lsp */
