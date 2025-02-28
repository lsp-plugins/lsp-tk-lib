/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 9 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_TEXTSELECTION_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_TEXTSELECTION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Text Selection property.
         * Allows to control text selection in such widgets like Edit
         */
        class TextSelection: public MultiProperty
        {
            protected:
                enum property_t
                {
                    P_VALUE,
                    P_FIRST,
                    P_LAST,

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                ssize_t             nFirst;             // First element in selection
                ssize_t             nLast;              // Last element in selection
                ssize_t             nLimit;             // Selection limit, invisible property

            protected:
                virtual void        push() override;
                virtual void        commit(atom_t property) override;

                void                parse(const LSPString *s);

            protected:
                explicit TextSelection(prop::Listener *listener = NULL);
                TextSelection(const TextSelection &) = delete;
                TextSelection(TextSelection &&) = delete;
                virtual ~TextSelection() override;

                TextSelection & operator = (const TextSelection &) = delete;
                TextSelection & operator = (TextSelection &&) = delete;

            public:
                inline ssize_t      first() const               { return nFirst;                            }
                inline ssize_t      last() const                { return nLast;                             }
                inline bool         valid() const               { return (nFirst >= 0) && (nLast >= 0);     }
                inline bool         invalid() const             { return (nFirst < 0) || (nLast < 0);       }
                inline bool         is_empty() const            { return nFirst == nLast;                   }
                inline bool         non_empty() const           { return nFirst != nLast;                   }
                inline ssize_t      length() const              { return (nFirst < nLast) ? nLast - nFirst : nFirst - nLast; }
                inline bool         reverted() const            { return nFirst > nLast;                    }
                inline ssize_t      starting() const            { return lsp_min(nFirst, nLast);            }
                inline ssize_t      ending() const              { return lsp_max(nFirst, nLast);            }
                inline ssize_t      limit() const               { return nLimit;                            }

                void                set(ssize_t first, ssize_t last);
                void                set(ssize_t first);
                ssize_t             set_first(ssize_t value);
                ssize_t             set_last(ssize_t value);
                void                set_all();
                void                truncate();
                void                unset();
                inline void         clear()                     { unset();                                  }
        };

        namespace prop
        {
            /**
             * TextSelection property implementation
             */
            class TextSelection: public tk::TextSelection
            {
                public:
                    explicit TextSelection(prop::Listener *listener = NULL): tk::TextSelection(listener) {};
                    TextSelection(const TextSelection &) = delete;
                    TextSelection(TextSelection &&) = delete;

                    TextSelection & operator = (const TextSelection &) = delete;
                    TextSelection & operator = (TextSelection &&) = delete;

                public:
                    void                set_limit(ssize_t limit);

                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::TextSelection::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::TextSelection::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::TextSelection::bind(property, style, vAtoms, DESC, &sListener); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::TextSelection::unbind(vAtoms, DESC, &sListener); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_TEXTSELECTION_H_ */
