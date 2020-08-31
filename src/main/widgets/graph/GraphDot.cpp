/*
 * GraphDot.cpp
 *
 *  Created on: 20 авг. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t GraphDot::metadata             = { "GraphDot", &GraphItem::metadata };

        GraphDot::param_t::param_t(GraphDot *dot, prop::Listener *lst):
            sEditable(lst),
            sValue(lst),
            sStep(lst)
        {
            pDot        = dot;
        }

        void GraphDot::param_t::property_changed(Property *prop)
        {
            if (sEditable.is(prop))
                pDot->query_draw();
            if (sValue.is(prop))
                pDot->query_draw();
            if (sStep.is(prop))
                { /* Nothing */ }
        }

        void GraphDot::param_t::init(const char *prefix)
        {
            Style *style  = pDot->style();
            LSPString id;
            id.fmt_ascii("%s.editable");
            sEditable.bind(&id, style);
            id.fmt_ascii("%s.value");
            sValue.bind(&id, style);
            id.fmt_ascii("%s.step");
            sStep.bind(&id, style);

            Style *sclass = pDot->style_class();
            if (sclass != NULL)
            {
                id.fmt_ascii("%s.editable");
                sEditable.init(sclass, false);
                id.fmt_ascii("%s.value");
                sValue.init(sclass, 0.0f, -1.0f, 1.0f);
                id.fmt_ascii("%s.step");
                sStep.init(sclass, 1.0f, 10.0f, 0.1f);
            }
        }

        GraphDot::GraphDot(Display *dpy):
            GraphItem(dpy),
            sHValue(this, &sProperties),
            sVValue(this, &sProperties),
            sZValue(this, &sProperties),
            sOrigin(&sProperties),
            sHAxis(&sProperties),
            sVAxis(&sProperties),
            sSize(&sProperties),
            sHoverSize(&sProperties),
            sBorderSize(&sProperties),
            sHoverBorderSize(&sProperties),
            sGap(&sProperties),
            sHoverGap(&sProperties),
            sColor(&sProperties),
            sHoverColor(&sProperties),
            sBorderColor(&sProperties),
            sHoverBorderColor(&sProperties)
        {
            nXFlags             = 0;
            nMBState            = 0;
            nMouseX             = 0;
            nMouseY             = 0;

            pClass              = &metadata;
        }

        GraphDot::~GraphDot()
        {
        }

        void GraphDot::destroy()
        {
        }

        status_t GraphDot::init()
        {
            status_t res        = GraphItem::init();
            if (res != STATUS_OK)
                return res;

            // Init values
            sHValue.init("hvalue");
            sVValue.init("vvalue");
            sZValue.init("zvalue");

            // Init style
            sOrigin.bind("origin", &sStyle);
            sHAxis.bind("haxis", &sStyle);
            sVAxis.bind("vaxis", &sStyle);
            sSize.bind("size", &sStyle);
            sHoverSize.bind("hover.size", &sStyle);
            sBorderSize.bind("border.size", &sStyle);
            sHoverBorderSize.bind("hover.border.size", &sStyle);
            sGap.bind("gap", &sStyle);
            sHoverGap.bind("hover.gap", &sStyle);
            sColor.bind("color", &sStyle);
            sHoverColor.bind("hover.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sHoverBorderColor.bind("hover.border.color", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sOrigin.init(sclass, 0);
                sHAxis.init(sclass, 0);
                sVAxis.init(sclass, 0);
                sSize.init(sclass, 4);
                sHoverSize.init(sclass, 4);
                sBorderSize.init(sclass, 12);
                sHoverBorderSize.init(sclass, 12);
                sGap.init(sclass, 1);
                sHoverGap.init(sclass, 1);
                sColor.init(sclass, "#cccccc");
                sHoverColor.init(sclass, "#ffffff");
                sBorderColor.init(sclass, "#cccccc");
                sHoverBorderColor.init(sclass, "#ffffff");
            }

            return STATUS_OK;
        }

        void GraphDot::property_changed(Property *prop)
        {
            GraphItem::property_changed(prop);
            sHValue.property_changed(prop);
            sVValue.property_changed(prop);
            sZValue.property_changed(prop);

            if (sOrigin.is(prop))
                query_draw();
            if (sHAxis.is(prop))
                query_draw();
            if (sVAxis.is(prop))
                query_draw();
            if (sSize.is(prop))
                query_draw();
            if (sHoverSize.is(prop))
                query_draw();
            if (sBorderSize.is(prop))
                query_draw();
            if (sHoverBorderSize.is(prop))
                query_draw();
            if (sGap.is(prop))
                query_draw();
            if (sHoverGap.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sHoverColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sHoverBorderColor.is(prop))
                query_draw();
        }

        void GraphDot::render(ws::ISurface *s, const ws::rectangle_t *area, bool force)
        {
        }

        status_t GraphDot::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            GraphDot *_this = widget_ptrcast<GraphDot>(ptr);
            return (_this != NULL) ? _this->on_change() : STATUS_BAD_ARGUMENTS;
        }

        status_t GraphDot::on_change()
        {
            lsp_trace("hvalue = %f, vvalue=%f, zvalue=%f",
                    sHValue.sValue.get(),
                    sVValue.sValue.get(),
                    sZValue.sValue.get()
                );
            return STATUS_OK;
        }

        void GraphDot::apply_motion(ssize_t x, ssize_t y, size_t flags)
        {
        }

        bool GraphDot::inside(ssize_t x, ssize_t y)
        {
            // TODO
            return false;
        }

        status_t GraphDot::on_mouse_in(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_out(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_down(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_up(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }

        status_t GraphDot::on_mouse_move(const ws::event_t *e)
        {
            // TODO
            return STATUS_OK;
        }
    }
}


