/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 7 сент. 2020 г.
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
        void StringList::Listener::notify(atom_t property)
        {
            pList->commit(property);
        }

        void StringList::Changes::notify(Property *prop)
        {
            if (bEnabled)
                pList->sync();
        }

        StringList::StringList(prop::Listener *listener):
            SimpleProperty(listener),
            sListener(this),
            sChanges(this)
        {
            pDict           = NULL;
        }

        StringList::~StringList()
        {
            SimpleProperty::unbind(&sListener);

            // Destroy all nested properties
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                StringItem *si = vItems.uget(i);
                if (si != NULL)
                    delete si;
            }
            vItems.flush();
        }

        status_t StringList::bind(atom_t property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property < 0) || (dict == NULL))
                return STATUS_BAD_ARGUMENTS;

            // Unbind first
            status_t res;
            if ((pStyle != NULL) && (nAtom >= 0))
            {
                res = pStyle->unbind(nAtom, &sListener);
                if (res != STATUS_OK)
                    return res;
                pStyle      = NULL;
                nAtom       = -1;
            }

            // Bind to new handler
            style->begin();
            {
                res = style->bind(property, PT_STRING, &sListener);
                if (res == STATUS_OK)
                {
                    pDict       = dict;
                    pStyle      = style;
                    nAtom       = property;
                }
            }
            style->end();

            if (pListener != NULL)
                pListener->notify(this);

            return res;
        }

        status_t StringList::bind(const char *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t StringList::bind(const LSPString *property, Style *style, i18n::IDictionary *dict)
        {
            if ((style == NULL) || (property == NULL))
                return STATUS_BAD_ARGUMENTS;
            atom_t atom = style->atom_id(property);
            return (atom >= 0) ? bind(atom, style, dict) : STATUS_UNKNOWN_ERR;
        }

        status_t StringList::unbind()
        {
            status_t res = SimpleProperty::unbind(&sListener);
            if (res == STATUS_OK)
                pDict       = NULL;

            return res;
        }

        void StringList::invalidate()
        {
            // Invalidate all nested strings
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                StringItem *si = vItems.uget(i);
                if (si != NULL)
                    si->invalidate(pDict);
            }
        }

        void StringList::sync()
        {
            if (pListener != NULL)
                pListener->notify(this);
        }

        void StringList::commit(atom_t property)
        {
            if (pStyle != NULL)
            {
                const char *lang;
                if ((property == nAtom) && (pStyle->get_string(property, &lang) == STATUS_OK))
                    invalidate();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        StringList::StringItem *StringList::create_item()
        {
            // Here may be some additional initialization stuff
            return new StringItem(&sChanges);
        }

        String *StringList::insert(size_t index)
        {
            StringItem *si  = create_item();
            if (si == NULL)
                return si;

            if (vItems.insert(index, si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        String *StringList::append()
        {
            StringItem *si  = create_item();

            if (vItems.append(si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        String *StringList::prepend()
        {
            StringItem *si = create_item();
            if (si == NULL)
                return si;

            if (vItems.prepend(si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        status_t StringList::remove(size_t index)
        {
            StringItem *si = vItems.remove(index);
            if (si == NULL)
                return STATUS_NOT_FOUND;

            delete si;
            sync();

            return STATUS_OK;
        }

        status_t StringList::remove(size_t index, size_t count)
        {
            lltl::parray<StringItem> removed;
            if (!vItems.remove_n(index, count, removed))
                return STATUS_NOT_FOUND;

            for (size_t i=0, n=removed.size(); i<n; ++i)
            {
                StringItem *si = removed.uget(i);
                if (si != NULL)
                    delete si;
            }

            sync();
            return STATUS_OK;
        }

        void StringList::clear()
        {
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                StringItem *si = vItems.uget(i);
                if (si != NULL)
                    delete si;
            }
            vItems.flush();

            sync();
        }

        status_t StringList::premove(const String *s)
        {
            // Lookup for the entry
            ssize_t index = -1;
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                StringItem *si = vItems.uget(i);
                if (si == s)
                {
                    index = i;
                    break;
                }
            }

            return (index >= 0) ? remove(index) : STATUS_NOT_FOUND;
        }

        status_t StringList::swap(size_t i1, size_t i2)
        {
            StringItem *si1 = vItems.get(i1);
            if (si1 == NULL)
                return STATUS_NOT_FOUND;

            StringItem *si2 = vItems.get(i2);
            if (si1 == NULL)
                return STATUS_NOT_FOUND;

            // Prevent from multiple sync()
            sChanges.enable(false);
                si1->swap(si2);
            sChanges.enable(true);

            // Perform sync()
            sync();

            return STATUS_OK;
        }
    }
}


