/*
 * Separator.h
 *
 *  Created on: 9 июл. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SEPARATOR_H_
#define LSP_PLUG_IN_TK_WIDGETS_SIMPLE_SEPARATOR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Simple separator widget
         */
        class Separator: public Widget
        {
            public:
                static const w_class_t    metadata;

            protected:
                prop::Orientation       sOrientation;
                prop::Color             sColor;
                prop::SizeRange         sSizeRange;
                prop::Integer           sThickness;

            protected:
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);

            public:
                explicit Separator(Display *dpy);
                virtual ~Separator();

                virtual status_t init();

            public:
                inline Color               *color()                 { return &sColor;       }
                inline const Color         *color() const           { return &sColor;       }

                inline Orientation         *orientation()           { return &sOrientation; }
                inline const Orientation   *orientation() const     { return &sOrientation; }

                inline SizeRange           *size()                  { return &sSizeRange;   }
                inline const SizeRange     *size() const            { return &sSizeRange;   }

                inline Integer             *thickness()             { return &sThickness;   }
                inline const Integer       *thickness() const       { return &sThickness;   }

            public:
                virtual void render(ws::ISurface *s, bool force);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_Separator_H_ */
