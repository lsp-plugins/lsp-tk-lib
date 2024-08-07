/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 12 июн. 2021 г.
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
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        Registry::Registry()
        {
        }

        Registry::~Registry()
        {
            destroy();
        }

        void Registry::destroy()
        {
            // Destroy all widget groups
            lltl::parray<lltl::parray<Widget>> v;
            sGroups.values(&v);
            sGroups.flush();

            for (size_t i=0, n=v.size(); i<n; ++i)
            {
                lltl::parray<Widget> *vv = v.uget(i);
                if (vv != NULL)
                {
                    vv->flush();
                    delete vv;
                }
            }

            // Clear widget mapping
            sMapping.flush();

            // Destroy all widgets in reverse order
            for (size_t i=vWidgets.size(); (i--) > 0;)
            {
                tk::Widget *w = vWidgets.uget(i);
//                lsp_trace("destroy widget ptr=%p, class=%s", w, w->get_class()->name);

                if (w != NULL)
                {
                    w->destroy();
                    delete w;
                }
            }

            vWidgets.flush();
        }

        status_t Registry::add(tk::Widget *w)
        {
            if (w == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (vWidgets.contains(w))
                return STATUS_ALREADY_EXISTS;
            else if (!vWidgets.add(w))
                return STATUS_NO_MEM;

//            lsp_trace("w = %p (%s)", w, w->get_class()->name);

            return STATUS_OK;
        }

        status_t Registry::add(const char *uid, tk::Widget *w)
        {
            if ((uid == NULL) || (w == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (vWidgets.contains(w))
                return STATUS_ALREADY_EXISTS;
            else if (!vWidgets.add(w))
                return STATUS_NO_MEM;

            if (!sMapping.create(uid, w))
            {
                vWidgets.premove(w);
                return (sMapping.contains(uid)) ? STATUS_DUPLICATED : STATUS_NO_MEM;
            }

            //lsp_trace("w = %p (%s)", w, uid);

            return STATUS_OK;
        }

        status_t Registry::add(const LSPString *uid, tk::Widget *w)
        {
            return (uid != NULL) ? add(uid->get_utf8(), w) : STATUS_BAD_ARGUMENTS;
        }

        status_t Registry::map(const char *uid, tk::Widget *w)
        {
            if ((uid == NULL) || (w == NULL))
                return STATUS_BAD_ARGUMENTS;

            if (!sMapping.create(uid, w))
                return (sMapping.contains(uid)) ? STATUS_DUPLICATED : STATUS_NO_MEM;

            return STATUS_OK;
        }

        status_t Registry::map(const LSPString *uid, tk::Widget *w)
        {
            return (uid != NULL) ? map(uid->get_utf8(), w) : STATUS_BAD_ARGUMENTS;
        }

        status_t Registry::map_group(const char *uid, tk::Widget *w)
        {
            lltl::parray<Widget> *list = sGroups.get(uid);
            if (list == NULL)
            {
                list = new lltl::parray<Widget>();
                if (list == NULL)
                    return STATUS_NO_MEM;
                if (!sGroups.create(uid, list))
                {
                    delete list;
                    return STATUS_NO_MEM;
                }
            }

            if (list->contains(w))
                return STATUS_DUPLICATED;

            return (list->add(w)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Registry::map_group(const LSPString *uid, tk::Widget *w)
        {
            return (uid != NULL) ? map_group(uid->get_utf8(), w) : STATUS_BAD_ARGUMENTS;
        }

        status_t Registry::unmap(const char *uid)
        {
            if (uid == NULL)
                return STATUS_BAD_ARGUMENTS;
            return (sMapping.remove(uid, NULL)) ? STATUS_OK : STATUS_NOT_FOUND;
        }

        status_t Registry::unmap(const LSPString *uid)
        {
            return (uid != NULL) ? unmap(uid->get_utf8()) : STATUS_BAD_ARGUMENTS;
        }

        status_t Registry::unmap(const tk::Widget *w)
        {
            // TODO: add two-way mapping
            lltl::parray<char> keys;
            lltl::parray<tk::Widget> values;

            if (!sMapping.items(&keys, &values))
                return STATUS_NO_MEM;

            for (size_t i=0, n=values.size(); i<n; ++i)
            {
                tk::Widget *xw = values.uget(i);
                if (w == xw)
                    return (sMapping.remove(keys.uget(i), NULL)) ? STATUS_OK : STATUS_NO_MEM;
            }

            return STATUS_NOT_FOUND;
        }

        ssize_t Registry::unmap(const tk::Widget * const *w, size_t n)
        {
            lltl::parray<char> keys;
            lltl::parray<tk::Widget> values;
            lltl::parray<tk::Widget> list;

            if (!sMapping.items(&keys, &values))
                return -STATUS_NO_MEM;

            // Prepare sorted list of widgets
            if (!list.add_n(n, const_cast<tk::Widget **>(w)))
                return -STATUS_NO_MEM;
            list.qsort(lltl::ptr_cmp_func);

            // Do main loop
            size_t unmapped = 0;
            for (size_t i=0, n=values.size(); i<n; ++i)
            {
                tk::Widget *xw = values.uget(i);
                if (remove_item(&list, xw))
                {
                    sMapping.remove(keys.uget(i), NULL);
                    ++unmapped;
                }
            }

            return unmapped;
        }

        status_t Registry::unmap_group(const char *uid, tk::Widget *w)
        {
            lltl::parray<Widget> *list = sGroups.get(uid);
            if (list == NULL)
                return STATUS_NOT_FOUND;

            return (list->premove(w)) ? STATUS_OK : STATUS_NOT_FOUND;
        }

        status_t Registry::unmap_group(const LSPString *uid, tk::Widget *w)
        {
            return (uid != NULL) ? unmap(uid->get_utf8()) : STATUS_BAD_ARGUMENTS;
        }

        status_t Registry::unmap_all_groups(tk::Widget *w)
        {
            lltl::parray<lltl::parray<Widget>> v;
            sGroups.values(&v);
            size_t removed = 0;

            for (size_t i=0, n=sGroups.size(); i<n; ++i)
            {
                lltl::parray<Widget> *vv = v.uget(i);
                if (vv == NULL)
                    continue;

                if (vv->premove(w))
                    ++removed;
            }

            return (removed > 0) ? STATUS_OK : STATUS_NOT_FOUND;
        }

        bool Registry::remove_item(lltl::parray<tk::Widget> *slist, tk::Widget *w)
        {
            // Use binary search
            ssize_t first = 0, last = slist->size() - 1;
            while (first <= last)
            {
                ssize_t mid = (first + last) >> 1;
                tk::Widget *c = slist->uget(mid);

                if (w < c)
                    last        = mid - 1;
                else if (w > c)
                    first       = mid + 1;
                else
                {
                    slist->remove(mid);
                    return true;
                }
            }

            return false;
        }

        tk::Widget *Registry::find(const char *uid)
        {
            return sMapping.get(uid);
        }

        tk::Widget *Registry::find(const LSPString *uid)
        {
            return (uid != NULL) ? find(uid->get_utf8()) : NULL;
        }

        bool Registry::contains(const tk::Widget *w) const
        {
            return vWidgets.contains(w);
        }

        bool Registry::contains(const char *uid) const
        {
            return (uid != NULL) ? sMapping.contains(uid) : false;
        }

        bool Registry::contains(const LSPString *uid) const
        {
            return (uid != NULL) ? contains(uid->get_utf8()) : false;
        }

        bool Registry::contains(const char *uid, const tk::Widget *w) const
        {
            lltl::parray<Widget> *list = sGroups.get(uid);
            return (list != NULL) ? list->contains(w) : false;
        }

        bool Registry::contains(const LSPString *uid, const tk::Widget *w) const
        {
            return (uid != NULL) ? contains(uid->get_utf8(), w) : false;
        }

        status_t Registry::query_group(const char *uid, lltl::parray<tk::Widget> *dst)
        {
            lltl::parray<Widget> *list = sGroups.get(uid);
            if (list == NULL)
                return STATUS_NOT_FOUND;

            return (dst->add(list)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Registry::query_group(const LSPString *uid, lltl::parray<tk::Widget> *dst)
        {
            return (uid != NULL) ? query_group(uid->get_utf8(), dst) : STATUS_BAD_ARGUMENTS;
        }

    } /* namespace tk */
} /* namespace lsp */

