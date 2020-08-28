/*
 * GraphText.h
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_
#define LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphText: public GraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                GraphText & operator = (const GraphText &);

            private:
                prop::String            sText;
                prop::Font              sFont;
                prop::Color             sColor;
                prop::Layout            sLayout;
                prop::TextLayout        sTextLayout;
                prop::Float             sHValue;
                prop::Float             sVValue;
                prop::Integer           sHAxis;
                prop::Integer           sVAxis;
                prop::Integer           sOrigin;

            protected:
                virtual void                property_changed(Property *prop);

            public:
                explicit GraphText(Display *dpy);
                virtual ~GraphText();

                virtual status_t            init();
                virtual void                destroy();

            public:
                LSP_TK_PROPERTY(String,             text,               &sText)
                LSP_TK_PROPERTY(Font,               font,               &sFont)
                LSP_TK_PROPERTY(Color,              color,              &sColor)
                LSP_TK_PROPERTY(Layout,             layout,             &sLayout)
                LSP_TK_PROPERTY(TextLayout,         text_layout,        &sTextLayout)
                LSP_TK_PROPERTY(Float,              hvalue,             &sHValue)
                LSP_TK_PROPERTY(Float,              vvalue,             &sVValue)
                LSP_TK_PROPERTY(Integer,            haxis,              &sHAxis)
                LSP_TK_PROPERTY(Integer,            vaxis,              &sVAxis)
                LSP_TK_PROPERTY(Integer,            origin,             &sOrigin)

            public:
                virtual void                render(ws::ISurface *s, const ws::rectangle_t *area, bool force);
        };
    }
}


#endif /* LSP_PLUG_IN_TK_WIDGETS_GRAPH_GRAPHTEXT_H_ */
