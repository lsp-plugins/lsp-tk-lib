/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins-referencer
 * Created on: 29 нояб. 2024 г.
 *
 * lsp-plugins-referencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins-referencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins-referencer. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        Shortcuts::Shortcuts(tk::prop::Listener *listener)
        {
            pListener       = listener;
        }

        Shortcuts::~Shortcuts()
        {
            clear();
        }

        void Shortcuts::clear()
        {
            for (size_t i=0; i<vLinks.size(); ++i)
            {
                Shortcut *link = vLinks.uget(i);
                if (link != NULL)
                    delete link;
            }
            vLinks.flush();
        }

        size_t Shortcuts::size() const
        {
            return vLinks.size();
        }

        Shortcut *Shortcuts::append()
        {
            return insert(vLinks.size());
        }

        Shortcut *Shortcuts::append(const Shortcut *scut)
        {
            return insert(scut, vLinks.size());
        }

        Shortcut *Shortcuts::append(const Shortcut &scut)
        {
            return insert(scut, vLinks.size());
        }

        Shortcut *Shortcuts::append(ws::code_t key, size_t mod)
        {
            return insert(key, mod, vLinks.size());
        }

        Shortcut *Shortcuts::prepend()
        {
            return insert(0);
        }

        Shortcut *Shortcuts::prepend(const Shortcut *scut)
        {
            return insert(scut, 0);
        }

        Shortcut *Shortcuts::prepend(const Shortcut &scut)
        {
            return insert(scut, 0);
        }

        Shortcut *Shortcuts::prepend(ws::code_t key, size_t mod)
        {
            return insert(key, mod, 0);
        }

        Shortcut *Shortcuts::insert(size_t index)
        {
            prop::Shortcut *lnk = new prop::Shortcut(pListener);
            if (lnk == NULL)
                return NULL;
            lsp_finally {
                if (lnk != NULL)
                    delete lnk;
            };

            if (!vLinks.insert(index, lnk))
                return NULL;

            return release_ptr(lnk);
        }

        Shortcut *Shortcuts::insert(const Shortcut *scut, size_t index)
        {
            Shortcut *link = insert(index);
            if (link != NULL)
                link->set(scut);
            return link;
        }

        Shortcut *Shortcuts::insert(const Shortcut &scut, size_t index)
        {
            Shortcut *link = insert(index);
            if (link != NULL)
                link->set(scut);
            return link;
        }

        Shortcut *Shortcuts::insert(ws::code_t key, size_t mod, size_t index)
        {
            Shortcut *link = insert(index);
            if (link != NULL)
                link->set(key, mod);
            return link;
        }

        Shortcut *Shortcuts::get(size_t index)
        {
            return vLinks.get(index);
        }

        bool Shortcuts::remove(size_t index)
        {
            Shortcut *res = vLinks.remove(index);
            if (res == NULL)
                return false;

            delete res;
            return true;
        }

        bool Shortcuts::remove(const Shortcut * scut)
        {
            ssize_t idx = index_of(scut);
            return (idx >= 0) ? remove(idx) : false;
        }

        bool Shortcuts::remove(const Shortcut & scut)
        {
            ssize_t idx = index_of(scut);
            return (idx >= 0) ? remove(idx) : false;
        }

        size_t Shortcuts::remove_all(const Shortcut * scut)
        {
            const size_t old_size = vLinks.size();
            for (size_t i=0, n=old_size; i< n; )
            {
                Shortcut *link = vLinks.uget(i);
                if ((link != NULL) && (link->equals(scut)))
                {
                    Shortcut *removed = vLinks.remove(i);
                    if (removed != NULL)
                        delete removed;
                    --n;
                }
                else
                    ++i;
            }

            return old_size - vLinks.size();
        }

        size_t Shortcuts::remove_all(const Shortcut & scut)
        {
            return remove_all(&scut);
        }

        bool Shortcuts::premove(Shortcut *item)
        {
            Shortcut *res = vLinks.premove(item);
            if (res == NULL)
                return false;

            delete res;
            return true;
        }

        ssize_t Shortcuts::index_of_instance(const Shortcut *item) const
        {
            return vLinks.index_of(item);
        }

        bool Shortcuts::contains_instance(const Shortcut *item) const
        {
            return vLinks.index_of(item) > 0;
        }

        bool Shortcuts::contains(const Shortcut *shortcut) const
        {
            return index_of(shortcut, 0) >= 0;
        }

        Slot *Shortcuts::handler_match(const Shortcut *shortcut, size_t first)
        {
            for (size_t i=first, n=vLinks.size(); i< n; ++i)
            {
                Shortcut *link = vLinks.uget(i);
                if ((link != NULL) && (link->match(shortcut)))
                    return link->slot();
            }

            return NULL;
        }

        Slot *Shortcuts::handler_of(const Shortcut *shortcut, size_t first)
        {
            for (size_t i=first, n=vLinks.size(); i< n; ++i)
            {
                Shortcut *link = vLinks.uget(i);
                if ((link != NULL) && (link->equals(shortcut)))
                    return link->slot();
            }

            return NULL;
        }

        ssize_t Shortcuts::index_of(const Shortcut & shortcut, size_t first) const
        {
            return index_of(&shortcut, first);
        }

        ssize_t Shortcuts::index_of(const Shortcut *shortcut, size_t first) const
        {
            for (size_t i=first, n=vLinks.size(); i< n; ++i)
            {
                const Shortcut *link = vLinks.uget(i);
                if ((link != NULL) && (link->equals(shortcut)))
                    return i;
            }

            return -1;
        }

        ssize_t Shortcuts::index_match(const Shortcut & shortcut, size_t first) const
        {
            return index_match(&shortcut, first);
        }

        ssize_t Shortcuts::index_match(const Shortcut *shortcut, size_t first) const
        {
            for (size_t i=first, n=vLinks.size(); i< n; ++i)
            {
                const Shortcut *link = vLinks.uget(i);
                if ((link != NULL) && (link->match(shortcut)))
                    return i;
            }

            return -1;
        }

    } /* namespace tk */
} /* namespace lsp */


