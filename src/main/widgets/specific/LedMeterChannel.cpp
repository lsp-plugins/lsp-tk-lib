/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 22 сент. 2020 г.
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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LedMeterChannel::metadata           = { "LedMeterChannel", &Widget::metadata };

        LedMeterChannel::LedMeterChannel(Display *dpy):
            Widget(dpy),
            sValue(&sProperties),
            sPeak(&sProperties),
            sBalance(&sProperties),
            sValueColor(&sProperties),
            sValueRanges(&sProperties),
            sPeakColor(&sProperties),
            sPeakRanges(&sProperties),
            sTextColor(&sProperties),
            sTextRanges(&sProperties),
            sBalanceColor(&sProperties),
            sText(&sProperties),
            sEstText(&sProperties),
            sPeakVisible(&sProperties),
            sBalanceVisible(&sProperties),
            sTextVisible(&sProperties),
            sReversive(&sProperties),
            sActive(&sProperties),
            sMinSegments(&sProperties),
            sConstraints(&sProperties),
            sFont(&sProperties),
            sBorder(&sProperties)
        {
            sAMeter.nLeft   = 0;
            sAMeter.nTop    = 0;
            sAMeter.nWidth  = 0;
            sAMeter.nHeight = 0;

            sAText.nLeft    = 0;
            sAText.nTop     = 0;
            sAText.nWidth   = 0;
            sAText.nHeight  = 0;

            pClass          = &metadata;
        }

        LedMeterChannel::~LedMeterChannel()
        {
        }

        void LedMeterChannel::destroy()
        {
            Widget::destroy();
        }

        status_t LedMeterChannel::init()
        {
            status_t res = Widget::init();
            if (res != STATUS_OK)
                return res;

            sValue.bind("value", &sStyle);
            sPeak.bind("peak", &sStyle);
            sBalance.bind("balance", &sStyle);
            sValueColor.bind("value.color", &sStyle);
            sValueRanges.bind("value.ranges", &sStyle);
            sPeakColor.bind("peak.color", &sStyle);
            sPeakRanges.bind("peak.ranges", &sStyle);
            sTextColor.bind("text.color", &sStyle);
            sTextRanges.bind("text.ranges", &sStyle);
            sBalanceColor.bind("balance.color", &sStyle);
            sText.bind(&sStyle, pDisplay->dictionary());
            sEstText.bind(&sStyle, pDisplay->dictionary());
            sPeakVisible.bind("peak.visible", &sStyle);
            sBalanceVisible.bind("balance.visible", &sStyle);
            sTextVisible.bind("text.visible", &sStyle);
            sReversive.bind("reversive", &sStyle);
            sActive.bind("active", &sStyle);
            sMinSegments.bind("segments.min", &sStyle);
            sConstraints.bind("constraints", &sStyle);
            sFont.bind("font", &sStyle);
            sBorder.bind("border", &sStyle);

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sValue.init(sclass, 0.0f, 0.0f, 1.0f);
                sPeak.init(sclass, 0.0f);
                sBalance.init(sclass, 0.5f);
                sValueColor.init(sclass, "#00ff00");
                sValueRanges.init(sclass, "");
                sPeakColor.init(sclass, "#ff0000");
                sPeakRanges.init(sclass, "");
                sBalanceColor.init(sclass, "#ffff00");
                sTextColor.init(sclass, "#00ff00");
                sTextRanges.init(sclass, "");
                sBalanceColor.init(sclass, "#ffff00");
                sPeakVisible.init(sclass, false);
                sBalanceVisible.init(sclass, false);
                sTextVisible.init(sclass, false);
                sReversive.init(sclass, false);
                sActive.init(sclass, false);
                sMinSegments.init(sclass, 12);
                sConstraints.init(sclass);
                sFont.init(sclass, 9);
                sBorder.init(sclass, 2);

                sEstText.set_raw("+99.9");
            }

            return STATUS_OK;
        }

        void LedMeterChannel::property_changed(Property *prop)
        {
            Widget::property_changed(prop);

            if (sValue.is(prop))
                query_draw();
            if (sPeak.is(prop) && (sPeakVisible.get()))
                query_draw();
            if (sBalance.is(prop) && (sBalanceVisible.get()))
                query_draw();
            if (sValueColor.is(prop))
                query_draw();
            if (sValueRanges.is(prop))
                query_draw();
            if (sPeakColor.is(prop) && (sPeakVisible.get()))
                query_draw();
            if (sPeakRanges.is(prop) && (sPeakVisible.get()))
                query_draw();
            if (sBalanceColor.is(prop) && (sBalanceVisible.get()))
                query_draw();
            if (sText.is(prop) && (sTextVisible.get()))
                query_draw();
            if (sEstText.is(prop) && (sTextVisible.get()))
                query_resize();
            if (sPeakVisible.is(prop))
                query_draw();
            if (sBalanceVisible.is(prop))
                query_draw();
            if (sTextVisible.is(prop))
                query_draw();
            if (sReversive.is(prop))
                query_draw();
            if (sActive.is(prop))
                query_draw();
            if (sMinSegments.is(prop))
                query_resize();
            if (sConstraints.is(prop))
                query_resize();
            if (sFont.is(prop) && (sTextVisible.get()))
                query_resize();
            if (sBorder.is(prop))
                query_resize();
        }

        void LedMeterChannel::size_request(ws::size_limit_t *r)
        {
        }

        void LedMeterChannel::realize(const ws::rectangle_t *r)
        {
        }

        void LedMeterChannel::draw(ws::ISurface *s)
        {
        }
    }
}

