/*
 * Switch.cpp
 *
 *  Created on: 1 июл. 2017 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        static const float ANGLE = 15.0f * M_PI / 180.0f;
        
        const w_class_t Switch::metadata =      { "Switch", &Widget::metadata };

        Switch::Switch(Display *dpy):
            Widget(dpy),
            sColor(&sProperties),
            sTextColor(&sProperties),
            sBorderColor(&sProperties),
            sHoleColor(&sProperties),
            sBorder(&sProperties),
            sSizeRange(&sProperties),
            sAspect(&sProperties),
            sAngle(&sProperties),
            sDown(&sProperties)
        {
            nState      = 0;
            nBMask      = 0;

            pClass      = &metadata;
        }
        
        status_t Switch::init()
        {
            status_t result = Widget::init();
            if (result != STATUS_OK)
                return result;

            sColor.bind("color", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sHoleColor.bind("hole.color", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sSizeRange.bind("size.range", &sStyle);
            sAspect.bind("size.aspect", &sStyle);
            sAngle.bind("angle", &sStyle);
            sDown.bind("down", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sColor.init(sclass, "#444444");
                sTextColor.init(sclass, "#eeeeee");
                sBorderColor.init(sclass, "#444444");
                sHoleColor.init(sclass, "#000000");
                sBorder.init(sclass, 8);
                sSizeRange.init(sclass, 24);
                sAspect.init(sclass, 1.41);
            }

            pClass      = &metadata;

            handler_id_t id = 0;
            id = sSlots.add(SLOT_FOCUS_IN, slot_on_change, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        Switch::~Switch()
        {
        }

        status_t Switch::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            Switch *_this       = widget_ptrcast<Switch>(ptr);
            bool *value         = static_cast<bool *>(data);
            return _this->on_change(*value);
        }

        void Switch::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sColor.is(prop))
                query_draw();
            if (sTextColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sHoleColor.is(prop))
                query_draw();
            if (sBorder.is(prop))
                query_resize();
            if (sSizeRange.is(prop))
                query_resize();
            if (sAspect.is(prop))
                query_resize();
            if (sAngle.is(prop))
                query_resize();
            if (sDown.is(prop))
                sync_state(sDown.get());
        }

        void Switch::draw(ws::ISurface *s)
        {
            // Prepare palette
            lsp::Color bg_color(sBgColor);
            lsp::Color border(sBorderColor);
            lsp::Color bcl(sColor);
            lsp::Color font(sTextColor);
            lsp::Color hole(sHoleColor);

            float bright    = sBrightness.get();
            float scaling   = lsp_max(0.0f, sScaling.get());

            border.scale_lightness(bright);
            font.scale_lightness(bright);
            bcl.scale_lightness(bright);

            // Draw background
            ws::rectangle_t r   = sSize;
            r.nLeft             = 0;
            r.nTop              = 0;

            // Draw border (if present)
            bool aa     = s->set_antialiasing(true);
            ws::IGradient *g;
            size_t xbw          = lsp_max(0, sBorder.get());
            if (xbw > 0)
            {
                float delta     = sqrtf(r.nWidth * r.nWidth + r.nHeight * r.nHeight);
                size_t champ    = lsp_max(1, scaling * 2.0f);
                lsp::Color bc(border);
                float xb        = bc.lightness();

                // Draw champfer
                for (size_t i=0; i<champ; ++i)
                {
                    // Compute color
                    float bright = (xb * (i + 1.0f)) / (champ + 1);
                    bc.lightness(bright);

                    // Create gradient
                    g = s->radial_gradient(
                        r.nLeft + r.nWidth, r.nTop, 0,
                        r.nLeft + r.nWidth, r.nTop, delta * 2.0f
                    );

                    g->add_color(0.0, bc.red(), bc.green(), bc.blue());
                    g->add_color(1.0, 0.5 * bc.red(), 0.5 *  bc.green(), 0.5 * bc.blue());
                    s->wire_rect(g, r.nLeft, r.nTop, r.nWidth-1, r.nHeight-1, 1.0f);
                    delete g;

                    // Update rect
                    r.nLeft        += 1;
                    r.nTop         += 1;
                    r.nWidth       -= 2;
                    r.nHeight      -= 2;
                }

                // Draw border
                bc.lightness(xb);
                size_t zbw      = lsp_max(1, xbw * scaling);
                g = s->radial_gradient(
                    r.nLeft + r.nWidth, r.nTop, 0,
                    r.nLeft + r.nWidth, r.nTop, delta * 2.0f
                );

                g->add_color(0.0, bc.red(), bc.green(), bc.blue());
                g->add_color(1.0, 0.5 * bc.red(), 0.5 *  bc.green(), 0.5 * bc.blue());
                s->fill_rect(g, r.nLeft, r.nTop, r.nWidth, r.nHeight);
                delete g;

                // Update rect
                r.nLeft        += zbw;
                r.nTop         += zbw;
                r.nWidth       -= zbw << 1;
                r.nHeight      -= zbw << 1;
            }

            // Draw hole
            s->wire_rect(hole, r.nLeft, r.nTop, r.nWidth-1, r.nHeight-1, lsp_max(1.0f, scaling));
            r.nLeft        += 1;
            r.nTop         += 1;
            r.nWidth       -= 2;
            r.nHeight      -= 2;

            // Draw button
            size_t pos      = (nState & S_PRESSED) ? 1 : (nState & S_TOGGLED) ? 2 : 0;
            size_t angle    = sAngle.get() & 3;
            if (angle & 2)
                pos     = 2 - pos;
            float radius    = ((angle & 1) ? r.nWidth : r.nHeight) * 0.5f / cosf(ANGLE);

            // Draw button
            ssize_t dw1 = radius * (sinf((pos + 1) * ANGLE) - sinf(ANGLE));
            ssize_t dw2 = radius * (sinf((3 - pos) * ANGLE) - sinf(ANGLE));
            float bc    = bcl.lightness();
            float b1    = (pos * 0.1) + bc;
            float b2    = ((2 - pos) * 0.1) + bc;
            float b0    = 0.1 * bc;
            size_t wid  = (angle & 1) ? r.nHeight : r.nWidth;
            ssize_t cx  = (wid + dw1 - dw2) >> 1;
            ssize_t dc1 = cx - dw1;
            ssize_t dc2 = wid - (cx + dw2);

            for (ssize_t i=0; i < dw1; ++i)
            {
                float arg = i + 1.0;
                float bright = (b1 - b0) * arg / dw1 + b0;
                bcl.lightness(bright);

                if (angle & 1)
                    s->fill_rect(bcl, r.nLeft, r.nTop + i, r.nWidth, dw1 - i);
                else
                    s->fill_rect(bcl, r.nLeft + i, r.nTop, dw1 - i, r.nHeight);
            }

            for (ssize_t i=0; i < dc1; ++i)
            {
                float bright = (b1 - bc) * (dc1 - i) / dc1 + bc;
                bcl.lightness(bright);

                if (angle & 1)
                    s->fill_rect(bcl, r.nLeft, r.nTop + dw1 + i, r.nWidth, dc1 - i);
                else
                    s->fill_rect(bcl, r.nLeft + dw1 + i, r.nTop, dc1 - i, r.nHeight);
            }

            for (ssize_t i=0; i < dw2; ++i)
            {
                float arg = i + 1.0;
                float bright = (b2 - b0) * arg / dw2 + b0;
                bcl.lightness(bright);

                if (angle & 1)
                    s->fill_rect(bcl, r.nLeft, r.nTop + r.nHeight - dw2, r.nWidth, dw2 - i);
                else
                    s->fill_rect(bcl, r.nLeft + r.nWidth - dw2, r.nTop, dw2 - i, r.nHeight);
            }

            for (ssize_t i=0; i < dc2; ++i)
            {
                float bright = (b2 - bc) * (dc2 - i) / dc2 + bc;
                bcl.lightness(bright);

                if (angle & 1)
                    s->fill_rect(bcl, r.nLeft, r.nTop + cx, r.nWidth, dc2 - i);
                else
                    s->fill_rect(bcl, r.nLeft + cx, r.nTop, dc2 - i, r.nHeight);
            }

            // Draw symbols
            bc          = font.lightness();
            b1          = bc - ((2 - pos) * 0.1);
            b2          = bc - (pos * 0.1);
            wid        -= dw1 + dw2;
            float s1    = wid * 0.125f;
            float s2    = wid * 0.125f;

            font.lightness((angle & 2) ? b2 : b1);
            s->set_antialiasing(true);

            ssize_t w = ssize_t(r.nWidth >> 1), h = ssize_t(r.nHeight >> 1);

            if (angle & 1)
                s->wire_arc(r.nLeft + w, r.nTop + cx + (angle - 2) * (wid >> 2), s1, 0, M_PI * 2.0, 2.0f * scaling, font);
            else
                s->wire_arc(r.nLeft + cx + (angle - 1) * (wid >> 2), r.nTop + h, s1, 0, M_PI * 2.0, 2.0f * scaling, font);

            font.lightness((angle & 2) ? b1 : b2);
            if (angle & 1)
                s->line(
                    r.nLeft + w, r.nTop + cx - (angle - 2) * (wid >> 2) + s2,
                    r.nLeft + w, r.nTop + cx - (angle - 2) * (wid >> 2) - s2,
                    2.0f * scaling, font
                );
            else
                s->line(
                    r.nLeft + cx - (angle - 1) * (wid >> 2) + s2, r.nTop + h,
                    r.nLeft + cx - (angle - 1) * (wid >> 2) - s2, r.nTop + h,
                    2.0f * scaling, font
                );

            s->set_antialiasing(aa);
        }

        void Switch::on_click(bool down)
        {
            lsp_trace("switch clicked: down=%s", (down) ? "true" : "false");
            sSlots.execute(SLOT_CHANGE, this);
        }

        void Switch::size_request(ws::size_limit_t *r)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());
            float aspect        = lsp_max(1.0f, sAspect.get());
            size_t angle        = sAngle.get();

            // Add border if present
            size_t x_space      = lsp_min(1, scaling);  // hole - extra space at each side
            size_t bw           = lsp_max(0, sBorder.get());
            if (bw > 0)
                x_space            += lsp_max(1, bw * scaling) + lsp_max(1, scaling * 2.0f); // border + chamfer

            // Compute minimum width and height vor vertical position
            r->nMinHeight       = lsp_max(8, sSizeRange.min());
            r->nMaxHeight       = lsp_max(r->nMinHeight, sSizeRange.max());
            r->nMinWidth        = lsp_max(8, r->nMinHeight* aspect);
            r->nMaxWidth        = lsp_max(r->nMinWidth, r->nMaxHeight * aspect);

            // Apply rotation
            if (angle & 1)
            {
                swap(r->nMinWidth, r->nMinHeight);
                swap(r->nMaxWidth, r->nMaxHeight);
            }

            // Scale and add extra space
            r->nMinWidth        = scaling * r->nMinWidth  + x_space * 2;
            r->nMinHeight       = scaling * r->nMinHeight + x_space * 2;
            r->nMaxWidth        = scaling * r->nMaxWidth  + x_space * 2;
            r->nMaxHeight       = scaling * r->nMaxHeight + x_space * 2;
        }

        bool Switch::check_mouse_over(ssize_t x, ssize_t y)
        {
            float scaling       = lsp_max(0.0f, sScaling.get());

            // Add border if present
            size_t x_space      = lsp_min(1, scaling);  // hole - extra space at each side
            size_t bw           = lsp_max(0, sBorder.get());
            if (bw > 0)
                x_space            += lsp_max(1, bw * scaling) + lsp_max(1, scaling * 2.0f); // border + chamfer

            x                  -= sSize.nLeft + x_space;
            y                  -= sSize.nTop  + x_space;
            ssize_t w           = sSize.nWidth  - x_space * 2;
            ssize_t h           = sSize.nHeight - x_space * 2;

            return (x >= 0) && (y >= 0) && (x < w) && (y < h);
        }

        status_t Switch::on_mouse_down(const ws::event_t *e)
        {
            nBMask         |= (1 << e->nCode);

            bool pressed    = (nBMask == (1 << ws::MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));
            bool is_pressed = nState & S_PRESSED;

            if (pressed != is_pressed)
            {
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                query_draw();
            }

            return STATUS_OK;
        }

        status_t Switch::on_mouse_up(const ws::event_t *e)
        {
            nBMask         &= ~(1 << e->nCode);
            bool pressed    = ((e->nCode == ws::MCB_LEFT) && (nBMask == 0)) || ((e->nCode != ws::MCB_LEFT) && (nBMask == (1 << ws::MCB_LEFT)));
            if (pressed)
                pressed     = (check_mouse_over(e->nLeft, e->nTop));
            if (nBMask == 0)
                pressed     = false;

            bool is_pressed = nState & S_PRESSED;
            if (pressed != is_pressed)
            {
                if (is_pressed)
                    nState     ^= S_TOGGLED;
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                if (nBMask == 0)
                    on_click(nState & S_TOGGLED);

                query_draw();
            }
            return STATUS_OK;
        }

        status_t Switch::on_mouse_move(const ws::event_t *e)
        {
            bool pressed    = (nBMask == (1 << ws::MCB_LEFT)) && (check_mouse_over(e->nLeft, e->nTop));
            bool is_pressed = nState & S_PRESSED;

            if (pressed != is_pressed)
            {
                if (pressed)
                    nState     |= S_PRESSED;
                else
                    nState     &= ~S_PRESSED;

                query_draw();
            }

            return STATUS_OK;
        }

        status_t Switch::on_change(bool set)
        {
            return STATUS_OK;
        }

        void Switch::sync_state(bool down)
        {
            // Do not react if state does not change
            if (bool(nState & S_TOGGLED) == down)
                return;

            nState = (down) ? nState | S_TOGGLED : nState & ~S_TOGGLED;
            query_draw();
        }
    } /* namespace ctl */
} /* namespace lsp */
