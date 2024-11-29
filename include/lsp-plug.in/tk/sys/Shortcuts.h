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

#ifndef LSP_PLUG_IN_TK_UTIL_SHORTCUTS_H_
#define LSP_PLUG_IN_TK_UTIL_SHORTCUTS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/tk/prop/multi/Shortcut.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Class that contains shortctucs, tracks events and executes shortcut handlers
         */
        class Shortcuts
        {
            protected:
                tk::prop::Listener         *pListener;
                lltl::parray<Shortcut>      vLinks;

            public:
                explicit Shortcuts(tk::prop::Listener *listener = NULL);
                Shortcuts(const Shortcuts &) = delete;
                Shortcuts(Shortcuts &&) = delete;
                virtual ~Shortcuts();

                Shortcuts(const Shortcut &) = delete;
                Shortcuts(Shortcut &&) = delete;

            public:
                /**
                 * Get number of elements
                 * @return number of elements
                 */
                size_t          size() const;

                /**
                 * Append one new shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *append();

                /**
                 * Append one new shotcut link
                 * @param scut shortcut combination to use
                 * @return new shortcut link or null on error
                 */
                Shortcut       *append(const Shortcut *scut);

                /**
                 * Append one new shortcut link
                 * @param scut shortcut combination to use
                 * @return new shortcut link or null on error
                 */
                Shortcut       *append(const Shortcut &scut);

                /**
                 * Append one new shortcut link
                 * @param key key code
                 * @param mod key modifiers
                 * @return new shortcut link or null on error
                 */
                Shortcut       *append(ws::code_t key, size_t mod);

                /**
                 * Prepend one new shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *prepend();

                /**
                 * Prepend one new shortcut link
                 * @param scut shortcut combination to use
                 * @return new shortcut link or null on error
                 */
                Shortcut       *prepend(const Shortcut *scut);

                /**
                 * Prepend one new shortcut link
                 * @param scut shortcut combination to use
                 * @return new shortcut link or null on error
                 */
                Shortcut       *prepend(const Shortcut &scut);

                /**
                 * Prepend one new shortcut link
                 * @param key key code
                 * @param mod key modifiers
                 * @return new shortcut link or null on error
                 */
                Shortcut       *prepend(ws::code_t key, size_t mod);

                /**
                 * Add one new shortcut link at the specified index
                 * @param index to add shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *insert(size_t index);

                /**
                 * Add one new shortcut link at the specified index
                 * @param scut shortcut combination to use
                 * @param index to add shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *insert(const Shortcut *scut, size_t index);

                /**
                 * Add one new shortcut link at the specified index
                 * @param scut shortcut combination to use
                 * @param index to add shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *insert(const Shortcut &scut, size_t index);

                /**
                 * Insert one new shortcut link
                 * @param key key code
                 * @param mod key modifiers
                 * @param index to add shortcut link
                 * @return new shortcut link or null on error
                 */
                Shortcut       *insert(ws::code_t key, size_t mod, size_t index);

                /**
                 * Get shortcut link at the specified position
                 * @param index index of the element
                 * @return shortcut link at the specified position
                 */
                Shortcut       *get(size_t index);

                /**
                 * Remove shotcut link at the specified position
                 * @param index index of item to remove
                 * @return true if item has been removed
                 */
                bool            remove(size_t index);

                /**
                 * Remove shotcut link that has exact key combination
                 * @param index index of item to remove
                 * @return true if item has been removed
                 */
                bool            remove(const Shortcut * scut);

                /**
                 * Remove shotcut link that has exact key combination
                 * @param index index of item to remove
                 * @return true if item has been removed
                 */
                bool            remove(const Shortcut & scut);

                /**
                 * Remove all shortcuts that are same to the specified one
                 * @param scut the shortcut to look for
                 * @return number of removed items
                 */
                size_t          remove_all(const Shortcut * scut);

                /**
                 * Remove all shortcuts that are same to the specified one
                 * @param scut the shortcut to look for
                 * @return number of removed items
                 */
                size_t          remove_all(const Shortcut & scut);

                /**
                 * Remove shotcut instance from the list
                 * @param item pointer to the shotcut link to remove
                 * @return true if item has been removed
                 */
                bool            premove(Shortcut *item);

                /**
                 * Get the index of the spcified shortcut link instance
                 * @param item item tho check
                 * @return true if list contains the specified shotcut link
                 */
                ssize_t         index_of_instance(const Shortcut *item) const;

                /**
                 * Check whether list contains the specific shortcut instance
                 * @param shortcut shortcut to search
                 * @return true if shortcut contains the specific shortctut
                 */
                bool            contains_instance(const Shortcut *shortcut) const;

                /**
                 * Check if list contains the specified shortcut
                 * @param item item tho check
                 * @return true if list contains the specified shotcut link
                 */
                bool            contains(const Shortcut *item) const;

                /**
                 * Find first matching handler for the shortcut and return the handling slot.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return pointer to the handling slot or null if not found.
                 */
                Slot           *handler_of(const Shortcut *shortcut, size_t first=0);

                /**
                 * Find first handler for the shortcut and return the handling slot.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return pointer to the handling slot or null if not found.
                 */
                Slot           *handler_match(const Shortcut *shortcut, size_t first=0);

                /**
                 * Find first handler with the same shortcut and return the index of the record.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return index of the first occurrence or negative value if not found.
                 */
                ssize_t         index_of(const Shortcut *shortcut, size_t first=0) const;

                /**
                 * Find first handler with the same shortcut and return the index of the record.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return index of the first occurrence or negative value if not found.
                 */
                ssize_t         index_of(const Shortcut & shortcut, size_t first=0) const;

                /**
                 * Find first matching handler for the shortcut and return the index of the record.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return index of the first occurrence or negative value if not found.
                 */
                ssize_t         index_match(const Shortcut *shortcut, size_t first=0) const;

                /**
                 * Find first matching handler for the shortcut and return the index of the record.
                 * Note that there can be multiple shortcuts, so use the 'first' parameter to
                 * look for next elements.
                 * @param shortcut shortcut to test
                 * @param first the index to start searching from
                 * @return index of the first occurrence or negative value if not found.
                 */
                ssize_t         index_match(const Shortcut & shortcut, size_t first=0) const;

                /**
                 * Remove all shortcut links
                 */
                void            clear();

        };
    } /* namespace tk */
} /* namespace lsp */

#endif /* MODULES_LSP_TK_LIB_INCLUDE_LSP_PLUG_IN_TK_UTIL_SHORTCUTS_H_ */
