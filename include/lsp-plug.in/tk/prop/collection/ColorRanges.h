/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_COLORRANGES_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_COLORRANGES_H_

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
        class ColorRanges: public SimpleProperty
        {
            private:
                ColorRanges & operator = (const ColorRanges &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        ColorRanges        *pList;

                    public:
                        explicit Listener(ColorRanges *lst)     { pList = lst; }

                    public:
                        virtual void        notify(atom_t property);
                };

                class Changes: public prop::Listener
                {
                    protected:
                        ColorRanges        *pList;
                        bool                bEnabled;

                    public:
                        explicit Changes(ColorRanges *lst)
                        {
                            bEnabled        = true;
                            pList           = lst;
                        }

                    public:
                        virtual void        notify(Property *prop);

                        inline void         enable(bool enable) { bEnabled = enable; }
                };

            protected:
                lltl::parray<ColorRange>        vItems;         // List of strings
                Listener                        sListener;      // Style listener
                Changes                         sChanges;       // Change listener

            protected:
                ColorRange         *create_item();
                void                sync();
                void                commit(atom_t property);
                status_t            parse_items(lltl::parray<ColorRange> *out, const LSPString *src);
                void                destroy_items(lltl::parray<ColorRange> *out);
                bool                deploy_items(lltl::parray<ColorRange> *out);

            protected:
                explicit ColorRanges(prop::Listener *listener = NULL);
                ~ColorRanges();

            public:
                /**
                 * Insert color range at the specified position
                 * @param index index to place the string
                 * @return inserted color range or NULL on error
                 */
                ColorRange         *insert(size_t index);

                /**
                 * Append color range to the end
                 * @return appended color range or NULL on error
                 */
                ColorRange          *append();

                /**
                 * Prepend string at the beginning
                 * @return prepended color range or NULL on error
                 */
                ColorRange          *prepend();

                /**
                 * Remove color ranges from the specified index
                 * @param index start index of the color range
                 * @param count number of strings to remove
                 * @return status of operation
                 */
                status_t            remove(size_t index, size_t count);

                /**
                 * Remove string at the specified index
                 * @param index index of the color range
                 * @return status of operation
                 */
                status_t            remove(size_t index);

                /**
                 * Remove string from the list if it is present in the list
                 * @param s color range to remove
                 * @return status of operation
                 */
                status_t            premove(const ColorRange *s);

                /**
                 * Get string at specified position
                 * @param index index of the color range
                 * @return string object or NULL
                 */
                inline ColorRange  *get(size_t index)                   { return vItems.get(index); }

                /**
                 * Swap two color ranges
                 * @param i1 index of first color range
                 * @param i2 index of last color range
                 * @return status of operation
                 */
                status_t            swap(size_t i1, size_t i2);

                /**
                 * Get size of the list
                 * @return size of the list
                 */
                inline size_t       size() const                        { return vItems.size();     }
        };
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_COLORRANGES_H_ */
