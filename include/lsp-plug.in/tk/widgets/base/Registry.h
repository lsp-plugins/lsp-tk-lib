/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_BASE_REGISTRY_H_
#define LSP_PLUG_IN_TK_WIDGETS_BASE_REGISTRY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/tk/widgets/cast.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Registry for widgets.
         * This class may be useful when there is a purpose of storing and identifying
         * dynamically-allocated widgets. The class automatically destroys and deallocates
         * all widgets which are added to the registry.
         * The registry also allows to map another, non-added widgets.
         */
        class Registry
        {
            private:
                Registry & operator = (const Registry &);
                Registry(const Registry &);

            protected:
                lltl::pphash<char, tk::Widget>      sMapping;           // Widget mapping by UID
                lltl::pphash<char, lltl::parray<tk::Widget>> sGroups;   // Widget groups
                lltl::parray<tk::Widget>            vWidgets;           // List of widgets

            protected:
                bool                        remove_item(lltl::parray<tk::Widget> *slist, tk::Widget *w);

            public:
                explicit Registry();
                ~Registry();

                /**
                 * Remove all mappings and destoy all added widgets
                 */
                void                        destroy();

            public:
                /**
                 * Add widget
                 * @param w widget to add
                 * @return status of operation
                 */
                status_t                    add(tk::Widget *w);

                /**
                 * Add and map widget
                 * @param uid unique identifier of the widget
                 * @param w widget to add
                 * @return status of operation
                 */
                status_t                    add(const char *uid, tk::Widget *w);

                /**
                 * Add and map widget
                 * @param uid unique identifier of the widget
                 * @param w widget to add
                 * @return status of operation
                 */
                status_t                    add(const LSPString *uid, tk::Widget *w);

                /**
                 * Map widget (assign unique identifier)
                 * @param uid unique identifier of widget
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    map(const char *uid, tk::Widget *w);

                /**
                 * Map widget (assign unique identifier)
                 * @param uid unique identifier of widget
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    map(const LSPString *uid, tk::Widget *w);

                /**
                 * Map widget to group
                 * @param uid unique group identifier
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    map_group(const char *uid, tk::Widget *w);

                /**
                 * Map widget to group
                 * @param uid unique group identifier
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    map_group(const LSPString *uid, tk::Widget *w);

                /**
                 * Unmap widget by it's identifier
                 * @param uid unique widget identifier
                 * @return status of operation
                 */
                status_t                    unmap(const char *uid);

                /**
                 * Unmap widget by it's identifier
                 * @param uid unique widget identifier
                 * @return status of operation
                 */
                status_t                    unmap(const LSPString *uid);

                /**
                 * Unmap widget by it's pointer to the instance
                 * @param w pointer to widget instance
                 * @return status of operation
                 */
                status_t                    unmap(const tk::Widget *w);

                /**
                 * Unmap all widgets passed as arguments
                 * @param w pointer to array of widgets
                 * @param n size of array
                 * @return number of unmapped widgets or negative error code
                 */
                ssize_t                     unmap(const tk::Widget * const *w, size_t n);

                /**
                 * Map widget to group
                 * @param uid unique group identifier
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    unmap_group(const char *uid, tk::Widget *w);

                /**
                 * Map widget to group
                 * @param uid unique group identifier
                 * @param w widget to map
                 * @return status of operation
                 */
                status_t                    unmap_group(const LSPString *uid, tk::Widget *w);

                /**
                 * Unmap widget from all previously mapped groups
                 * @param w widget to unmap
                 * @return status of operation
                 */
                status_t                    unmap_all_groups(tk::Widget *w);

                /**
                 * Find widget by it's uinque identifier and return the pointer to the instance
                 * @param uid unique identifier of widget
                 * @return the resolved widget or NULL
                 */
                tk::Widget                 *find(const char *uid);

                /**
                 * Find widget by it's uinque identifier and return the pointer to the instance
                 * @param uid unique identifier of widget
                 * @return the resolved widget or NULL
                 */
                tk::Widget                 *find(const LSPString *uid);

                /**
                 * Query all widgets that match the group name and add to passed list
                 * @param uid unique widget identifier
                 * @param dst destination list to store the widgets
                 * @return status of operation
                 */
                status_t                    query_group(const char *uid, lltl::parray<tk::Widget> *dst);

                /**
                 * Query all widgets that match the group name and add to passed list
                 * @param uid unique widget identifier
                 * @param dst destination list to store the widgets
                 * @return status of operation
                 */
                status_t                    query_group(const LSPString *uid, lltl::parray<tk::Widget> *dst);

                /**
                 * Get widget of specific type by it's unique identifier
                 * @param uid unique identifier of widget
                 * @return the resolved widget or NULL
                 */
                template <class Widget>
                    inline Widget          *get(const char *uid)
                    {
                        return tk::widget_cast<Widget>(find(uid));
                    }

                /**
                 * Get widget of specific type by it's unique identifier
                 * @param uid unique identifier of widget
                 * @return the resolved widget or NULL
                 */
                template <class Widget>
                    inline Widget          *get(const LSPString *uid)
                    {
                        return tk::widget_cast<Widget>(find(uid));
                    }

                /**
                 * Check that registry contains the widget
                 * @param w widget to check
                 * @return true if registry contains the widget
                 */
                bool                        contains(const tk::Widget *w) const;

                /**
                 * Check that registry has mapping
                 * @param uid unique identifier of widget to check
                 * @return true if there is mapping for specified identifier
                 */
                bool                        contains(const char *uid) const;

                /**
                 * Check that registry has mapping
                 * @param uid unique identifier of widget to check
                 * @return true if there is mapping for specified identifier
                 */
                bool                        contains(const LSPString *uid) const;

                /**
                 * Check that registry has widget mapped to the group
                 * @param uid unique identifier of group
                 * @param w widget to check
                 * @return true if there is mapping of specified widget to the specified group
                 */
                bool                        contains(const char *uid, const tk::Widget *w) const;

                /**
                 * Check that registry has widget mapped to the group
                 * @param uid unique identifier of group
                 * @param w widget to check
                 * @return true if there is mapping of specified widget to the specified group
                 */
                bool                        contains(const LSPString *uid, const tk::Widget *w) const;
        };
    } /* tk */
} /* lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_BASE_REGISTRY_H_ */
