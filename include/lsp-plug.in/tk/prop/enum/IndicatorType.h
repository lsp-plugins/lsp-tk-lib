/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 25 февр. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_INDICATORTYPE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_INDICATORTYPE_H_

namespace lsp
{
    namespace tk
    {
        enum indicator_type_t
        {
            INDICATOR_SEGMENT,
            INDICATOR_PIXEL,
            INDICATOR_MODERN,
        };

        /**
         * Border style class of window
         */
        class IndicatorType: public Enum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit IndicatorType(prop::Listener *listener = NULL): Enum(ENUM, INDICATOR_SEGMENT, listener) {};
                IndicatorType(const IndicatorType &) = delete;
                IndicatorType(IndicatorType &&) = delete;
                IndicatorType & operator = (const IndicatorType &) = delete;
                IndicatorType & operator = (IndicatorType &&) = delete;

            public:
                inline indicator_type_t   get() const               { return indicator_type_t(nValue); }
                inline indicator_type_t   set(indicator_type_t v)   { return indicator_type_t(Enum::set(v)); };
        };

        namespace prop
        {
            class IndicatorType: public tk::IndicatorType
            {
                public:
                    explicit IndicatorType(prop::Listener *listener = NULL): tk::IndicatorType(listener) {};
                    IndicatorType(const IndicatorType &) = delete;
                    IndicatorType(IndicatorType &&) = delete;
                    IndicatorType & operator = (const IndicatorType &) = delete;
                    IndicatorType & operator = (IndicatorType &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::IndicatorType::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::IndicatorType::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::IndicatorType::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::IndicatorType::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_INDICATORTYPE_H_ */
