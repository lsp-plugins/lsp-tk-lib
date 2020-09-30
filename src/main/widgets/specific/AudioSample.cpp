/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 28 сент. 2020 г.
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
        const w_class_t AudioSample::metadata           = { "AudioSample", &WidgetContainer::metadata };

        AudioSample::AudioSample(Display *dpy):
            WidgetContainer(dpy),
            vChannels(&sProperties, &sIListener),
            sWaveBorder(&sProperties),
            sFadeInBorder(&sProperties),
            sFadeOutBorder(&sProperties),
            sLineWidth(&sProperties),
            sConstraints(&sProperties),
            sActive(&sProperties),
            sSGroups(&sProperties),
            sMainText(&sProperties),
            sMainLayout(&sProperties),
            sMainFont(&sProperties),
            sMainColor(&sProperties),
            sMainVisibility(&sProperties),
            sLabelFont(&sProperties),
            sLabelBgColor(&sProperties),
            sLabelRadius(&sProperties),
            sBorder(&sProperties),
            sBorderRadius(&sProperties),
            sGlass(&sProperties),
            sColor(&sProperties),
            sBorderColor(&sProperties),
            sGlassColor(&sProperties),
            sIPadding(&sProperties)
        {
            for (size_t i=0; i<LABELS; ++i)
            {
                sLabel[i].listener(&sProperties);
                sLabelColor[i].listener(&sProperties);
                sLabelLayout[i].listener(&sProperties);
                sLabelVisibility[i].listener(&sProperties);
            }

            sGraph.nLeft        = 0;
            sGraph.nTop         = 0;
            sGraph.nWidth       = 0;
            sGraph.nHeight      = 0;

            pClass              = &metadata;
        }

        AudioSample::~AudioSample()
        {
            do_destroy();
        }

        void AudioSample::destroy()
        {
            WidgetContainer::destroy();
            do_destroy();
        }

        void AudioSample::do_destroy()
        {
            // Unlink all items
            for (size_t i=0, n=vChannels.size(); i<n; ++i)
            {
                AudioChannel *item   = vChannels.uget(i);
                if (item == NULL)
                    continue;

                unlink_widget(item);
            }

            // Flush containers
            vChannels.flush();
            vVisible.flush();
        }

        status_t AudioSample::init()
        {
            status_t res = WidgetContainer::init();
            if (res != STATUS_OK)
                return res;

            // Init listener
            sIListener.bind_all(this, on_add_item, on_remove_item);

            // Bind properties
            sWaveBorder.bind("wave.border", &sStyle);
            sFadeInBorder.bind("fade_in.border", &sStyle);
            sFadeOutBorder.bind("fade_out.border", &sStyle);
            sLineWidth.bind("line.width", &sStyle);
            sConstraints.bind("size.constraints", &sStyle);
            sActive.bind("active", &sStyle);
            sSGroups.bind("stereo_groups", &sStyle);
            sMainText.bind(&sStyle, pDisplay->dictionary());
            sMainLayout.bind("main.layout", &sStyle);
            sMainFont.bind("main.font", &sStyle);
            sMainVisibility.bind("main.visibility", &sStyle);
            sLabelFont.bind("label.font", &sStyle);
            sLabelBgColor.bind("label.bg.color", &sStyle);
            sLabelRadius.bind("label.radius", &sStyle);
            sBorder.bind("border.size", &sStyle);
            sBorderRadius.bind("border.radius", &sStyle);
            sGlass.bind("glass", &sStyle);
            sColor.bind("color", &sStyle);
            sBorderColor.bind("border.color", &sStyle);
            sGlassColor.bind("glass.color", &sStyle);
            sIPadding.bind("padding.internal", &sStyle);

            for (size_t i=0; i<LABELS; ++i)
            {
                LSPString id;

                sLabel[i].bind(&sStyle, pDisplay->dictionary());
                id.fmt_ascii("label.%d.text.color", int(i));
                sLabelColor[i].bind(&id, &sStyle);
                id.fmt_ascii("label.%d.layout", int(i));
                sLabelLayout[i].bind(&id, &sStyle);
                id.fmt_ascii("label.%d.visibility", int(i));
                sLabelVisibility[i].bind(&id, &sStyle);
            }

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sWaveBorder.init(sclass, 1);
                sFadeInBorder.init(sclass, 1);
                sFadeOutBorder.init(sclass, 1);
                sLineWidth.init(sclass, 1);
                sConstraints.init(sclass);
                sActive.init(sclass, false);
                sSGroups.init(sclass, false);

                sMainLayout.init(sclass, 0.0f, 0.0f);
                sMainFont.init(sclass, 16.0f);
                sMainColor.init(sclass, "#00ff00");
                sMainVisibility.init(sclass, false);
                sLabelFont.init(sclass, 10.0f);
                sLabelBgColor.init(sclass, "#88000000");
                sLabelRadius.init(sclass, 2);

                sBorder.init(sclass, 4);
                sBorderRadius.init(sclass, 12);
                sGlass.init(sclass, true);
                sColor.init(sclass, "#000000");
                sBorderColor.init(sclass, "#000000");
                sGlassColor.init(sclass, "#ffffff");
                sIPadding.init(sclass, 1);

                for (size_t i=0; i<LABELS; ++i)
                {
                    sLabelColor[i].init(sclass, "#ffffff");
                    sLabelLayout[i].init(sclass, 0.0f, 0.0f);
                    sLabelVisibility[i].init(sclass, false);
                }
            }

            return STATUS_OK;
        }

        void AudioSample::property_changed(Property *prop)
        {
            WidgetContainer::property_changed(prop);

            if (vChannels.is(prop))
                query_resize();
            if (sWaveBorder.is(prop))
                query_resize();
            if (sFadeInBorder.is(prop))
                query_draw();
            if (sFadeOutBorder.is(prop))
                query_draw();
            if (sLineWidth.is(prop))
                query_draw();
            if (sConstraints.is(prop))
                query_resize();
            if (sSGroups.is(prop))
                query_resize();

            if ((sMainText.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainLayout.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if ((sMainFont.is(prop)) && (sMainVisibility.get()))
                query_draw();
            if (sMainVisibility.is(prop))
                query_draw();
            if (sLabelFont.is(prop))
                query_draw();
            if (sLabelBgColor.is(prop))
                query_draw();
            if (sLabelRadius.is(prop))
                query_draw();

            if (sBorder.is(prop))
                query_resize();
            if (sBorderRadius.is(prop))
                query_resize();
            if (sGlass.is(prop))
                query_draw();
            if (sColor.is(prop))
                query_draw();
            if (sBorderColor.is(prop))
                query_draw();
            if (sGlassColor.is(prop))
                query_draw();
            if (sIPadding.is(prop))
                query_resize();

            for (size_t i=0; i<LABELS; ++i)
            {
                if (sLabelColor[i].is(prop))
                    query_draw();
                if (sLabelLayout[i].is(prop))
                    query_draw();
                if (sLabelVisibility[i].is(prop))
                    query_draw();
            }
        }

        void AudioSample::size_request(ws::size_limit_t *r)
        {
            // TODO
        }

        void AudioSample::on_add_item(void *obj, Property *prop, Widget *w)
        {
            AudioChannel *item = widget_cast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            item->set_parent(_this);
            _this->query_resize();
        }

        void AudioSample::on_remove_item(void *obj, Property *prop, Widget *w)
        {
            AudioChannel *item = widget_cast<AudioChannel>(w);
            if (item == NULL)
                return;

            AudioSample *_this = widget_ptrcast<AudioSample>(obj);
            if (_this == NULL)
                return;

            // Remove widget from supplementary structures
            _this->unlink_widget(item);
            _this->query_resize();
        }

        status_t AudioSample::add(Widget *widget)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(widget);
            return (item != NULL) ? vChannels.add(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove(Widget *child)
        {
            AudioChannel *item  = widget_cast<AudioChannel>(child);
            return (item != NULL) ? vChannels.premove(item) : STATUS_BAD_TYPE;
        }

        status_t AudioSample::remove_all()
        {
            vChannels.clear();
            return STATUS_OK;
        }

        void AudioSample::draw_channel1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_fades1(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_channel2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw_fades2(const ws::rectangle_t *r, ws::ISurface *s, AudioChannel *c1, AudioChannel *c2, size_t samples, float scaling, float bright)
        {
            // TODO
        }

        void AudioSample::draw(ws::ISurface *s)
        {
            // TODO
        }
    }
}


