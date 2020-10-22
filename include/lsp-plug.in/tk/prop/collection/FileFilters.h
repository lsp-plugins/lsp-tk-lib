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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_FILEFILTERS_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_FILEFILTERS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Set of file filters for searching files
         */
        class FileFilters: public Property
        {
            private:
                FileFilters & operator = (const FileFilters &);

            protected:
                class Listener: public prop::Listener
                {
                    protected:
                        bool            bLock;
                        FileFilters    *pFilters;

                    public:
                        inline Listener(FileFilters *prop)
                        {
                            bLock       = false;
                            pFilters    = prop;
                        }

                        virtual void    notify(Property *prop);
                        inline void     set_lock(bool lock)     { bLock = lock;             }
                };

            protected:
                lltl::parray<FileMask>          vItems;
                Listener                        sListener;
                atom_t                          nProperty;
                i18n::IDictionary              *pDict;

            protected:
                void                sync();
                void                commit(Property *prop);
                status_t            bind(atom_t property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const char *property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const LSPString *property, Style *style, i18n::IDictionary *dict);
                status_t            unbind();

                FileMask           *create_item();
                static void         destroy_item(FileMask *mask);
                static void         destroy_items(lltl::parray<FileMask> *list);

            protected:
                explicit FileFilters(prop::Listener *listener = NULL);
                ~FileFilters();

            public:
                /**
                 * Get size of the array in elements
                 * @return size of the array in elements
                 */
                inline size_t       size() const            { return vItems.size();         }

                /**
                 * Get capacity of the array
                 * @return capacity
                 */
                inline size_t       capacity() const        { return vItems.capacity();     }

                /**
                 * Get the value by specified index
                 * @param index index of the value
                 * @return the value or 0 if the bad index has been specified
                 */
                inline const FileMask   *get(size_t index) const    { return vItems.get(index);     }
                inline FileMask         *get(size_t index)          { return vItems.get(index);     }

                inline ssize_t      index_of(FileMask *item) const  { return vItems.index_of(item); }
                inline bool         contains(FileMask *item) const  { return vItems.contains(item); }

            public:
                /**
                 * Clear the collection, trims the array to 0 elements
                 */
                void                clear();

                /**
                 * Reserve space but do not resize
                 * @param size size of collection
                 * @return status of operation
                 */
                inline status_t     reserve(size_t size)                    { return (vItems.reserve(size)) ? STATUS_OK : STATUS_NO_MEM;    }

                /**
                 * Prepend value
                 * @param v value to prepend
                 * @return status of operation
                 */
                FileMask           *prepend();

                /**
                 * Append values
                 * @param v values to append
                 * @param count number of values to append
                 * @return status of operation
                 */
                FileMask           *append();

                /**
                 * Add value (same as append)
                 * @param v value to add
                 * @return status of operation
                 */
                FileMask           *add();

                /**
                 * Insert values
                 * @param idx index to insert
                 * @param v values to insert
                 * @param count number of values to insert
                 * @return status of operation
                 */
                FileMask           *insert(size_t idx);

                /**
                 * Remove items
                 * @param idx index of item to remove
                 * @param count number of items to remove
                 * @return status of operation
                 */
                status_t            remove(size_t idx, size_t count);

                /**
                 * Remove the item
                 * @param idx index of item to remove
                 * @return status of operation
                 */
                status_t            remove(size_t idx);

                /**
                 * Remove item by pointer
                 * @param item pointer to item
                 * @return status of operation
                 */
                status_t            premove(FileMask *item);

                /**
                 * Swap contents with another array
                 * @param src array to perform swap
                 */
                void                swap(FileFilters *src);
                inline void         swap(FileFilters &src)                  { swap(&src);                       }
        };

        namespace prop
        {
            class FileFilters: public tk::FileFilters
            {
                private:
                    FileFilters & operator = (const FileFilters &);

                public:
                    explicit FileFilters(prop::Listener *listener = NULL): tk::FileFilters(listener) {}

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t id, Style *style, i18n::IDictionary *dict)              { return tk::FileFilters::bind(id, style, dict); }
                    inline status_t     bind(const LSPString *id, Style *style, i18n::IDictionary *dict)    { return tk::FileFilters::bind(id, style, dict); }
                    inline status_t     bind(const char *id, Style *style, i18n::IDictionary *dict)         { return tk::FileFilters::bind(id, style, dict); }
                    inline status_t     bind(Style *style, i18n::IDictionary *dict)                         { return tk::FileFilters::bind(LSP_TK_PROP_LANGUAGE, style, dict); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                            { return tk::FileFilters::unbind();             }

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                         }
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_FILEFILTERS_H_ */
