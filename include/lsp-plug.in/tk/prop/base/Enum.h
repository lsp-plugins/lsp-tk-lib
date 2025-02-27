/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 8 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_
#define LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Integering property interface
         */
        class Enum: public SimpleProperty
        {
            protected:
                ssize_t             nValue;
                const prop::enum_t *pEnum;

            public:
                using SimpleProperty::override;

            protected:
                status_t            unbind();
                status_t            bind(atom_t property, Style *style);
                status_t            bind(const char *property, Style *style);
                status_t            bind(const LSPString *property, Style *style);
                ssize_t             set(ssize_t v);
                ssize_t             init(Style *style, ssize_t v);
                ssize_t             override(Style *style, ssize_t v);

                virtual void        push() override;
                virtual void        commit(atom_t property) override;

            protected:
                explicit Enum(const prop::enum_t *xenum, size_t dfl, prop::Listener *listener = NULL);
                Enum(const Enum &) = delete;
                Enum(Enum &&) = delete;
                virtual ~Enum() override;

                Enum & operator = (const Enum &) = delete;
                Enum & operator = (Enum &&) = delete;

            public:
                status_t            parse(const char *value);
                status_t            parse(const LSPString *value);
                inline ssize_t      index() const               { return nValue;            }
                inline ssize_t      set_index(ssize_t v)        { return set(v);            }
        };
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_BASE_ENUM_H_ */
