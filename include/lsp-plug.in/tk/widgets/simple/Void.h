/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 11 апр. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        // Style definition
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(Void, Widget)
                prop::SizeConstraints       sConstraints;
                prop::Color                 sColor;
                prop::Boolean               sFill;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * Void widget that holds some allocated space and fills it with specified color
         */
        class Void: public Widget
        {
            private:
                Void & operator = (const Void &);
                Void(const Void &);

            public:
                static const w_class_t    metadata;

            protected:
                prop::SizeConstraints       sConstraints;
                prop::Color                 sColor;
                prop::Boolean               sFill;

            public:
                explicit Void(Display *dpy);
                virtual ~Void();

            public:
                LSP_TK_PROPERTY(SizeConstraints,    constraints,        &sConstraints)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Boolean,            fill,               &sFill)

            protected:
                virtual void        size_request(ws::size_limit_t *r);
                virtual void        property_changed(Property *prop);

            public:
                virtual status_t    init();

                virtual void        render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_WIDGETS_SIMPLE_VOID_H_ */
