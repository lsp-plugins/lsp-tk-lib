/*
 * SizeConstraints.cpp
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/prop.h>
#include <lsp-plug.in/tk/sys/Display.h>
#include <lsp-plug.in/tk/base.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t SizeConstraints::DESC[] =
        {
            { "",               PT_STRING   },
            { ".min_width",     PT_INT      },
            { ".min_height",    PT_INT      },
            { ".max_width",     PT_INT      },
            { ".max_height",    PT_INT      },
            { NULL,             PT_UNKNOWN  }
        };

        void SizeConstraints::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        SizeConstraints::SizeConstraints(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            sValue.nMinWidth    = -1;
            sValue.nMinHeight   = -1;
            sValue.nMaxWidth    = -1;
            sValue.nMaxHeight   = -1;
        }

        SizeConstraints::~SizeConstraints()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void SizeConstraints::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ws::size_limit_t &p   = sValue;

            ssize_t v;
            if ((property == vAtoms[P_MIN_WIDTH]) && (pStyle->get_int(vAtoms[P_MIN_WIDTH], &v) == STATUS_OK))
                p.nMinWidth     = lsp_max(v, -1);
            if ((property == vAtoms[P_MIN_HEIGHT]) && (pStyle->get_int(vAtoms[P_MIN_HEIGHT], &v) == STATUS_OK))
                p.nMinHeight    = lsp_max(v, -1);
            if ((property == vAtoms[P_MAX_WIDTH]) && (pStyle->get_int(vAtoms[P_MAX_WIDTH], &v) == STATUS_OK))
                p.nMaxWidth     = lsp_max(v, -1);
            if ((property == vAtoms[P_MAX_HEIGHT]) && (pStyle->get_int(vAtoms[P_MAX_HEIGHT], &v) == STATUS_OK))
                p.nMaxHeight    = lsp_max(v, -1);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[4];
                size_t n = Property::parse_ints(xv, 4, &s);
                if (n == 4)
                {
                    p.nMinWidth     = lsp_max(xv[0], -1);
                    p.nMinHeight    = lsp_max(xv[1], -1);
                    p.nMaxWidth     = lsp_max(xv[2], -1);
                    p.nMaxHeight    = lsp_max(xv[3], -1);
                }
                else if (n == 2)
                {
                    p.nMinWidth     = -1;
                    p.nMinHeight    = -1;
                    p.nMaxWidth     = lsp_max(xv[0], -1);
                    p.nMaxHeight    = lsp_max(xv[1], -1);
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void SizeConstraints::sync()
        {
            if (pStyle == NULL)
            {
                if (pListener != NULL)
                    pListener->notify(this);
                return;
            }

            pStyle->begin();
            {
                ws::size_limit_t &p       = sValue;

                // Simple components
                if (vAtoms[P_MIN_WIDTH] >= 0)
                    pStyle->set_int(vAtoms[P_MIN_WIDTH], p.nMinWidth);
                if (vAtoms[P_MIN_HEIGHT] >= 0)
                    pStyle->set_int(vAtoms[P_MIN_HEIGHT], p.nMinHeight);
                if (vAtoms[P_MAX_WIDTH] >= 0)
                    pStyle->set_int(vAtoms[P_MAX_WIDTH], p.nMaxWidth);
                if (vAtoms[P_MAX_HEIGHT] >= 0)
                    pStyle->set_int(vAtoms[P_MAX_HEIGHT], p.nMaxHeight);

                // Compound objects
                LSPString s;
                if (vAtoms[P_VALUE] >= 0)
                {
                    if (s.fmt_ascii("%ld %ld %ld %ld", long(p.nMinWidth), long(p.nMinHeight), long(p.nMaxWidth), long(p.nMaxHeight)))
                        pStyle->set_string(vAtoms[P_VALUE], &s);
                }
            }
            pStyle->end();
        }

        void SizeConstraints::get(ssize_t *min_width, ssize_t *min_height, ssize_t *max_width, ssize_t *max_height) const
        {
            *min_width  = sValue.nMinWidth;
            *min_height = sValue.nMinHeight;
            *max_width  = sValue.nMaxWidth;
            *max_height = sValue.nMaxHeight;
        }

        void SizeConstraints::get(ssize_t &min_width, ssize_t &min_height, ssize_t &max_width, ssize_t &max_height) const
        {
            min_width   = sValue.nMinWidth;
            min_height  = sValue.nMinHeight;
            max_width   = sValue.nMaxWidth;
            max_height  = sValue.nMaxHeight;
        }

        ssize_t SizeConstraints::set_min_width(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMinWidth;
            if (old == value)
                return value;

            sValue.nMinWidth    = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_min_height(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMinHeight;
            if (old == value)
                return value;

            sValue.nMinHeight   = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_max_width(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMaxWidth;
            if (old == value)
                return value;

            sValue.nMaxWidth    = value;
            sync();
            return old;
        }

        ssize_t SizeConstraints::set_max_height(ssize_t value)
        {
            value               = lsp_max(value, -1);
            ssize_t old         = sValue.nMaxHeight;
            if (old == value)
                return value;

            sValue.nMaxHeight   = value;
            sync();
            return old;
        }

        void SizeConstraints::set_min(ssize_t width, ssize_t height)
        {
            width               = lsp_max(width, -1);
            height              = lsp_max(height, -1);
            if ((sValue.nMinWidth == width) &&
                (sValue.nMinHeight == height))
                return;

            sValue.nMinWidth    = width;
            sValue.nMinHeight   = height;
            sync();
        }

        void SizeConstraints::set_max(ssize_t width, ssize_t height)
        {
            width               = lsp_max(width, -1);
            height              = lsp_max(height, -1);
            if ((sValue.nMaxWidth == width) &&
                (sValue.nMaxHeight == height))
                return;

            sValue.nMaxWidth    = width;
            sValue.nMaxHeight   = height;
            sync();
        }

        void SizeConstraints::set_width(ssize_t min, ssize_t max)
        {
            min                 = lsp_max(min, -1);
            max                 = lsp_max(max, -1);
            if ((sValue.nMinWidth == min) &&
                (sValue.nMaxWidth == max))
                return;

            sValue.nMinWidth    = min;
            sValue.nMaxWidth    = max;
            sync();
        }

        void SizeConstraints::set_height(ssize_t min, ssize_t max)
        {
            min                 = lsp_max(min, -1);
            max                 = lsp_max(max, -1);
            if ((sValue.nMinHeight == min) &&
                (sValue.nMaxHeight == max))
                return;

            sValue.nMinHeight   = min;
            sValue.nMaxHeight   = max;
            sync();
        }

        void SizeConstraints::set(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
        {
            min_width           = lsp_max(min_width, -1);
            min_height          = lsp_max(min_height, -1);
            max_width           = lsp_max(max_width, -1);
            max_height          = lsp_max(max_height, -1);

            if ((sValue.nMinWidth == min_width) &&
                (sValue.nMinHeight == min_height) &&
                (sValue.nMaxWidth == max_width) &&
                (sValue.nMaxHeight == max_height))
                return;

            sValue.nMinWidth    = min_width;
            sValue.nMinHeight   = min_height;
            sValue.nMaxWidth    = max_width;
            sValue.nMaxHeight   = max_height;
            sync();
        }

        void SizeConstraints::set(const ws::size_limit_t *p)
        {
            ssize_t min_width   = lsp_max(p->nMinWidth, -1);
            ssize_t min_height  = lsp_max(p->nMinHeight, -1);
            ssize_t max_width   = lsp_max(p->nMaxWidth, -1);
            ssize_t max_height  = lsp_max(p->nMaxHeight, -1);

            if ((sValue.nMinWidth == min_width) &&
                (sValue.nMinHeight == min_height) &&
                (sValue.nMaxWidth == max_width) &&
                (sValue.nMaxHeight == max_height))
                return;

            sValue.nMinWidth    = min_width;
            sValue.nMinHeight   = min_height;
            sValue.nMaxWidth    = max_width;
            sValue.nMaxHeight   = max_height;
            sync();
        }

        void SizeConstraints::set(const SizeConstraints *p)
        {
            if ((sValue.nMinWidth == p->sValue.nMinWidth) &&
                (sValue.nMinHeight == p->sValue.nMinHeight) &&
                (sValue.nMaxWidth == p->sValue.nMaxWidth) &&
                (sValue.nMaxHeight == p->sValue.nMaxHeight))
                return;

            sValue.nMinWidth    = p->sValue.nMinWidth;
            sValue.nMinHeight   = p->sValue.nMinHeight;
            sValue.nMaxWidth    = p->sValue.nMaxWidth;
            sValue.nMaxHeight   = p->sValue.nMaxHeight;
            sync();
        }

        void SizeConstraints::compute(ws::size_limit_t *limit, float scale)
        {
            scale               = lsp_max(scale, 0.0f);
            limit->nMinWidth    = (sValue.nMinWidth >= 0) ? sValue.nMinWidth * scale : -1;
            limit->nMinHeight   = (sValue.nMinHeight >= 0) ? sValue.nMinHeight * scale : -1;
            limit->nMaxWidth    = (sValue.nMaxWidth >= 0) ? sValue.nMaxWidth * scale : -1;
            limit->nMaxHeight   = (sValue.nMaxHeight >= 0) ? sValue.nMaxHeight * scale : -1;
        }

        void SizeConstraints::apply(ws::size_limit_t *sc, float scale)
        {
            ws::size_limit_t l;
            compute(&l, scale);

            // Compute maximum width
            if ((sc->nMaxWidth >= 0) && (l.nMaxWidth >= 0))
                sc->nMaxWidth   = lsp_min(sc->nMaxWidth, l.nMaxWidth);
            else
                sc->nMaxWidth   = l.nMaxWidth;

            // Compute maximum height
            if ((sc->nMaxHeight) && (l.nMaxHeight >= 0))
                sc->nMaxHeight  = lsp_min(sc->nMaxHeight, l.nMaxHeight);
            else
                sc->nMaxHeight  = l.nMaxHeight;

            // Compute minimum width
            if ((l.nMinWidth >= 0) && (sc->nMinWidth >= 0))
                sc->nMinWidth   = lsp_max(sc->nMinWidth, l.nMinWidth);
            else
                sc->nMinWidth   = l.nMinWidth;

            // Compute minimum height
            if ((sc->nMinHeight >= 0) && (l.nMinHeight >= 0))
                sc->nMinHeight  = lsp_max(sc->nMinHeight, l.nMinHeight);
            else
                sc->nMinHeight  = l.nMinHeight;

            // Maximum width should not be less than minimum width
            if ((sc->nMinWidth >= 0) && (sc->nMaxWidth >= 0))
                sc->nMinWidth   = lsp_max(sc->nMinWidth, sc->nMaxWidth);

            // Maximum height should not be less than minimum height
            if ((sc->nMinHeight >= 0) && (sc->nMaxHeight >= 0))
                sc->nMinHeight  = lsp_max(sc->nMinHeight, sc->nMaxHeight);
        }

        namespace prop
        {
            status_t SizeConstraints::bind(const char *property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;

                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, widget->style(), widget->display()
                    );
            }

            status_t SizeConstraints::bind(atom_t property, Widget *widget)
            {
                if (widget == NULL)
                    return STATUS_BAD_ARGUMENTS;
                Display *dpy = widget->display();
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        dpy->atom_name(property), widget->style(), widget->display()
                    );
            }

            status_t SizeConstraints::bind(const char *property, Style *style, Display *dpy)
            {
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        property, style, dpy
                    );
            }

            status_t SizeConstraints::bind(atom_t property, Style *style, Display *dpy)
            {
                if (dpy == NULL)
                    return STATUS_BAD_ARGUMENTS;
                return MultiProperty::bind
                    (
                        vAtoms, DESC, &sListener,
                        dpy->atom_name(property), style, dpy
                    );
            }
        }
    } /* namespace tk */
} /* namespace lsp */


