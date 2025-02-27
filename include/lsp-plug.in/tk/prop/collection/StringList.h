/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_STRINGLIST_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_STRINGLIST_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * String list: list containing string properties
         */
        class StringList: public SimpleProperty
        {
            protected:
                // Wrapper around the string
                class StringItem: public tk::String
                {
                    public:
                        explicit inline StringItem(i18n::IDictionary *dict, prop::Listener *listener = NULL): tk::String(NULL)
                        {
                            pDict       = dict;
                        }

                    public:
                        inline void invalidate(i18n::IDictionary *dict)
                        {
                            pDict       = dict;
                            tk::String::invalidate();
                        }
                };

                class Listener: public IStyleListener
                {
                    protected:
                        StringList         *pList;

                    public:
                        explicit Listener(StringList *lst)      { pList = lst; }

                    public:
                        virtual void        notify(atom_t property) override;
                };

                class Changes: public prop::Listener
                {
                    protected:
                        StringList         *pList;
                        bool                bEnabled;

                    public:
                        explicit Changes(StringList *lst)
                        {
                            bEnabled        = true;
                            pList           = lst;
                        }

                    public:
                        virtual void        notify(Property *prop) override;

                        inline void         enable(bool enable) { bEnabled = enable; }
                };

            protected:
                lltl::parray<StringItem>        vItems;         // List of strings
                Listener                        sListener;      // Style listener
                Changes                         sChanges;       // Change listener
                i18n::IDictionary              *pDict;          // Related dictionary

            protected:
                status_t            bind(atom_t property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const char *property, Style *style, i18n::IDictionary *dict);
                status_t            bind(const LSPString *property, Style *style, i18n::IDictionary *dict);
                status_t            unbind();
                StringItem         *create_item();
                void                sync();
                void                invalidate();
                virtual void        commit(atom_t property) override;

            protected:
                explicit StringList(prop::Listener *listener = NULL);
                StringList(const StringList &) = delete;
                StringList(StringList &&) = delete;
                virtual ~StringList() override;

                StringList & operator = (const StringList &) = delete;
                StringList & operator = (StringList &&) = delete;

            public:
                /**
                 * Insert string at the specified position
                 * @param index index to place the string
                 * @return inserted string or NULL on error
                 */
                String             *insert(size_t index);

                /**
                 * Append string to the end
                 * @return appended string or NULL on error
                 */
                String             *append();

                /**
                 * Prepend string at the beginning
                 * @return prepended string or NULL on error
                 */
                String             *prepend();

                /**
                 * Remove strings from the specified index
                 * @param index start index of the string
                 * @param count number of strings to remove
                 * @return status of operation
                 */
                status_t            remove(size_t index, size_t count);

                /**
                 * Clear the list.
                 */
                void                clear();

                /**
                 * Remove string at the specified index
                 * @param index index of the string
                 * @return status of operation
                 */
                status_t            remove(size_t index);

                /**
                 * Remove string from the list if it is present in the list
                 * @param s string to remove
                 * @return status of operation
                 */
                status_t            premove(const String *s);

                /**
                 * Get string at specified position
                 * @param index index of the string
                 * @return string object or NULL
                 */
                inline String      *get(size_t index)                   { return vItems.get(index); }

                /**
                 * Swap two strings
                 * @param i1 index of first string
                 * @param i2 index of last string
                 * @return status of operation
                 */
                status_t            swap(size_t i1, size_t i2);

                /**
                 * Get size of the list
                 * @return size of the list
                 */
                inline size_t       size() const                        { return vItems.size();     }
        };

        namespace prop
        {
            class StringList: public tk::StringList
            {
                public:
                    explicit StringList(prop::Listener *listener = NULL): tk::StringList(listener) {};
                    StringList(const StringList &) = delete;
                    StringList(StringList &&) = delete;

                    StringList & operator = (const StringList &) = delete;
                    StringList & operator = (StringList &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t id, Style *style, i18n::IDictionary *dict)              { return tk::StringList::bind(id, style, dict); }
                    inline status_t     bind(const LSPString *id, Style *style, i18n::IDictionary *dict)    { return tk::StringList::bind(id, style, dict); }
                    inline status_t     bind(const char *id, Style *style, i18n::IDictionary *dict)         { return tk::StringList::bind(id, style, dict); }
                    inline status_t     bind(Style *style, i18n::IDictionary *dict)                         { return tk::StringList::bind(LSP_TK_PROP_LANGUAGE, style, dict); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                    { return tk::StringList::unbind(); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_STRINGLIST_H_ */
