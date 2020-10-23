/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2020 г.
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
        void FileFilters::Listener::notify(Property *prop)
        {
            if (pFilters != NULL)
                pFilters->commit(prop);
        }

        FileFilters::FileFilters(prop::Listener *listener, prop::CollectionListener *clistener):
            Property(listener),
            sListener(this)
        {
            pCListener  = clistener;
            nProperty   = -1;
            pDict       = NULL;
        }

        FileFilters::~FileFilters()
        {
            unbind();
            destroy_items(&vItems);
        }

        void FileFilters::destroy_items(lltl::parray<FileMask> *list)
        {
            for (size_t i=0, n=list->size(); i<n; ++i)
                destroy_item(list->uget(i));
            list->flush();
        }

        void FileFilters::sync()
        {
            if (pListener != NULL)
                pListener->notify(this);
        }

        void FileFilters::commit(Property *prop)
        {
            if (pListener != NULL)
                pListener->notify(this);
        }

        status_t FileFilters::bind(const char *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t FileFilters::bind(const LSPString *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t FileFilters::bind(atom_t property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property < 0) || (dict == NULL))
                return STATUS_BAD_ARGUMENTS;

            status_t res = (pStyle != NULL) ? unbind() : STATUS_OK;
            if (res != STATUS_OK)
                return res;

            // Bind all items
            sListener.set_lock(true);
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(vItems.uget(i));
                if (m != NULL)
                    res = m->bind(property, style, dict);
                if (res != STATUS_OK)
                    break;
            }
            sListener.set_lock(false);

            // Remember values and sync
            if (res == STATUS_OK)
            {
                nProperty       = property;
                pStyle          = style;
                pDict           = dict;

                sync();
            }

            return res;
        }

        status_t FileFilters::unbind()
        {
            status_t res = STATUS_OK;

            sListener.set_lock(true);
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(vItems.uget(i));
                if (m != NULL)
                    res = m->unbind();
                if (res != STATUS_OK)
                    break;
            }
            sListener.set_lock(false);

            nProperty       = -1;
            pStyle          = NULL;
            pDict           = NULL;

            return res;
        }

        FileMask *FileFilters::create_item()
        {
            prop::FileMask *m = new prop::FileMask();
            if (m == NULL)
                return NULL;

            if (pStyle != NULL)
            {
                if ((m->bind(nProperty, pStyle, pDict)) != STATUS_OK)
                {
                    delete m;
                    return NULL;
                }
            }

            m->listener(&sListener);
            return m;
        }

        void FileFilters::destroy_item(FileMask *mask)
        {
            if (mask == NULL)
                return;

            prop::FileMask *m = static_cast<prop::FileMask *>(mask);
            delete m;
        }

        void FileFilters::clear()
        {
            lltl::parray<FileMask> removed;
            removed.swap(&vItems);
            destroy_items(&removed);
            sync();
        }

        FileMask *FileFilters::prepend()
        {
            FileMask *m = create_item();
            if ((m != NULL) && (vItems.prepend(m)))
            {
                sync();
                return m;
            }

            destroy_item(m);
            return NULL;
        }

        FileMask *FileFilters::append()
        {
            FileMask *m = create_item();
            if ((m != NULL) && (vItems.append(m)))
            {
                if (pCListener != NULL)
                    pCListener->add(this, m);
                sync();
                return m;
            }

            destroy_item(m);
            return NULL;
        }

        FileMask *FileFilters::add()
        {
            FileMask *m = create_item();
            if ((m != NULL) && (vItems.add(m)))
            {
                if (pCListener != NULL)
                    pCListener->add(this, m);
                sync();
                return m;
            }

            destroy_item(m);
            return NULL;
        }

        FileMask *FileFilters::insert(size_t idx)
        {
            FileMask *m = create_item();
            if ((m != NULL) && (vItems.insert(idx, m)))
            {
                if (pCListener != NULL)
                    pCListener->add(this, m);
                sync();
                return m;
            }

            destroy_item(m);
            return NULL;
        }

        status_t FileFilters::remove(size_t idx)
        {
            FileMask *m = NULL;
            if (!vItems.remove(idx, &m))
                return STATUS_NOT_FOUND;

            if (pCListener != NULL)
                pCListener->remove(this, m);
            sync();

            destroy_item(m);
            return STATUS_OK;
        }

        status_t FileFilters::remove(size_t idx, size_t count)
        {
            lltl::parray<FileMask> removed;
            if (!vItems.remove_n(idx, count, &removed))
                return STATUS_NO_MEM;

            if (pCListener != NULL)
            {
                for (size_t i=0, n=removed.size(); i<n; ++i)
                {
                    FileMask *m     = removed.uget(i);
                    pCListener->remove(this, m);
                }
            }
            sync();

            destroy_items(&removed);
            return STATUS_OK;
        }

        status_t FileFilters::premove(FileMask *item)
        {
            FileMask *m = NULL;
            if (!vItems.premove(item, &m))
                return STATUS_NOT_FOUND;

            if (pCListener != NULL)
                pCListener->remove(this, m);
            sync();
            destroy_item(m);
            return STATUS_OK;
        }

        void FileFilters::swap(FileFilters *src)
        {
            lltl::parray<FileMask> s1, s2;

            s1.swap(&vItems);
            s2.swap(&src->vItems);

            // Re-bind properties
            sListener.set_lock(true);
            for (size_t i=0, n=s1.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(s1.uget(i));
                m->unbind();
                if (src->pStyle != NULL)
                    m->bind(src->nProperty, src->pStyle, src->pDict);
                if (pCListener != NULL)
                    pCListener->remove(this, m);
            }

            for (size_t i=0, n=s2.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(s2.uget(i));
                m->unbind();
                if (pStyle != NULL)
                    m->bind(nProperty, pStyle, pDict);
                if (src->pCListener != NULL)
                    src->pCListener->remove(src, m);
            }

            // Swap contents
            s1.swap(&src->vItems);
            s2.swap(&vItems);

            // Notify about addition
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(vItems.uget(i));
                if (pCListener != NULL)
                    pCListener->add(this, m);
            }

            for (size_t i=0, n=src->vItems.size(); i<n; ++i)
            {
                prop::FileMask *m = static_cast<prop::FileMask *>(src->vItems.uget(i));
                if (src->pCListener != NULL)
                    src->pCListener->add(src, m);
            }

            sListener.set_lock(false);

            sync();
        }
    }
}


