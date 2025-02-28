/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 июн. 2021 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_TEXTADJUST_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_TEXTADJUST_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Border style class of window
         */
        class TextAdjust: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit TextAdjust(prop::Listener *listener = NULL): Enum(ENUM, TA_NONE, listener) {};
                TextAdjust(const TextAdjust &) = delete;
                TextAdjust(TextAdjust &&) = delete;
                TextAdjust & operator = (const TextAdjust &) = delete;
                TextAdjust & operator = (TextAdjust &&) = delete;

            public:
                inline text_adjust_t        get() const     { return text_adjust_t(nValue); }

                inline text_adjust_t        set(text_adjust_t v)
                    { return text_adjust_t(Enum::set(v)); };

                inline bool                 is_toupper() const      { return nValue == TA_TOUPPER;  }
                inline bool                 is_lower() const        { return nValue == TA_TOLOWER;  }
                inline bool                 is_none() const         { return nValue == TA_NONE;     }

                inline text_adjust_t        set_toupper()           { return set(TA_TOUPPER);       }
                inline text_adjust_t        set_tolower()           { return set(TA_TOLOWER);       }
                inline text_adjust_t        set_none()              { return set(TA_NONE);          }

                status_t                    apply(LSPString *dst, const LSPString *src) const;
                status_t                    apply(LSPString *dst) const;
        };

        namespace prop
        {
            class TextAdjust: public tk::TextAdjust
            {
                public:
                    explicit TextAdjust(prop::Listener *listener = NULL): tk::TextAdjust(listener) {};
                    TextAdjust(const TextAdjust &) = delete;
                    TextAdjust(TextAdjust &&) = delete;
                    TextAdjust & operator = (const TextAdjust &) = delete;
                    TextAdjust & operator = (TextAdjust &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::TextAdjust::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::TextAdjust::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::TextAdjust::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::TextAdjust::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_ENUM_TEXTADJUST_H_ */
