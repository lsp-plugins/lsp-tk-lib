/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 10 мая 2020 г.
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
        const prop::desc_t SizeConstraints::DESC[] =
        {
            { "",               PT_STRING   },
            { ".width.min",     PT_INT      },
            { ".height.min",    PT_INT      },
            { ".width.max",     PT_INT      },
            { ".height.max",    PT_INT      },
            { NULL,             PT_UNKNOWN  }
        };

        void SizeConstraints::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        SizeConstraints::SizeConstraints(prop::Listener *listener):
            MultiProperty(vAtoms, P_COUNT, listener),
            sListener(this)
        {
            sValue.nMinWidth    = -1;
            sValue.nMinHeight   = -1;
            sValue.nMaxWidth    = -1;
            sValue.nMaxHeight   = -1;
            sValue.nPreWidth    = -1;
            sValue.nPreHeight   = -1;
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
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
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
            if (pListener != NULL)
                pListener->notify(this);
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
            limit->nMinWidth    = (sValue.nMinWidth  >= 0) ? sValue.nMinWidth  * scale : -1;
            limit->nMinHeight   = (sValue.nMinHeight >= 0) ? sValue.nMinHeight * scale : -1;
            limit->nMaxWidth    = (sValue.nMaxWidth  >= 0) ? sValue.nMaxWidth  * scale : -1;
            limit->nMaxHeight   = (sValue.nMaxHeight >= 0) ? sValue.nMaxHeight * scale : -1;
            limit->nPreWidth    = -1;
            limit->nPreHeight   = -1;
        }

        void SizeConstraints::tcompute(ws::size_limit_t *limit, float scale)
        {
            scale               = lsp_max(scale, 0.0f);
            limit->nMinWidth    = (sValue.nMinHeight >= 0) ? sValue.nMinHeight * scale : -1;
            limit->nMinHeight   = (sValue.nMinWidth  >= 0) ? sValue.nMinWidth  * scale : -1;
            limit->nMaxWidth    = (sValue.nMaxHeight >= 0) ? sValue.nMaxHeight * scale : -1;
            limit->nMaxHeight   = (sValue.nMaxWidth  >= 0) ? sValue.nMaxWidth  * scale : -1;
            limit->nPreWidth    = -1;
            limit->nPreHeight   = -1;
        }

        void SizeConstraints::apply(ws::size_limit_t *dst, const ws::size_limit_t *src, const ws::size_limit_t *rule)
        {
            // Compute maximum width & height
            if (src->nMaxWidth >= 0)
                dst->nMaxWidth  = (rule->nMaxWidth >= 0) ? lsp_min(src->nMaxWidth, rule->nMaxWidth) : src->nMaxWidth;
            else
                dst->nMaxWidth  = rule->nMaxWidth;

            if (src->nMaxHeight >= 0)
                dst->nMaxHeight = (rule->nMaxHeight >= 0) ? lsp_min(src->nMaxHeight, rule->nMaxHeight) : src->nMaxHeight;
            else
                dst->nMaxHeight = rule->nMaxHeight;

            // Compute minimum width & height
            if (src->nMinWidth >= 0)
                dst->nMinWidth  = (rule->nMinWidth >= 0) ? lsp_max(src->nMinWidth, rule->nMinWidth) : src->nMinWidth;
            else
                dst->nMinWidth  = rule->nMinWidth;

            if (src->nMinHeight >= 0)
                dst->nMinHeight = (rule->nMinHeight >= 0) ? lsp_max(src->nMinHeight, rule->nMinHeight) : src->nMinHeight;
            else
                dst->nMinHeight = rule->nMinHeight;

            // Maximum width should not be less than minimum width
            if ((dst->nMinWidth >= 0) && (dst->nMaxWidth >= 0))
                dst->nMaxWidth  = lsp_max(dst->nMinWidth, dst->nMaxWidth);

            // Maximum height should not be less than minimum height
            if ((dst->nMinHeight >= 0) && (dst->nMaxHeight >= 0))
                dst->nMaxHeight = lsp_max(dst->nMinHeight, dst->nMaxHeight);

            // Preferred width and height adjustments
            if (src->nPreWidth >= 0)
            {
                if (src->nMaxWidth >= 0)
                    dst->nPreWidth   = lsp_min(src->nPreWidth, src->nMaxWidth);
                if (src->nMinWidth >= 0)
                    dst->nPreWidth   = lsp_max(src->nPreWidth, src->nMinWidth);
            }
            else
                dst->nPreWidth      = -1;

            if (src->nPreHeight >= 0)
            {
                if (src->nMaxHeight >= 0)
                    dst->nPreHeight  = lsp_min(src->nPreHeight, src->nMaxHeight);
                if (src->nMinHeight >= 0)
                    dst->nPreHeight  = lsp_max(src->nPreHeight, src->nMinHeight);
            }
            else
                dst->nPreHeight     = -1;
        }

        void SizeConstraints::apply(ws::size_limit_t *dst, const ws::size_limit_t *src, float scale)
        {
            // Compute self parameters
            ws::size_limit_t l;
            compute(&l, scale);
            apply(dst, src, &l);
        }

        void SizeConstraints::tapply(ws::size_limit_t *dst, const ws::size_limit_t *src, float scale)
        {
            // Compute self parameters
            ws::size_limit_t l;
            tcompute(&l, scale);
            apply(dst, src, &l);
        }

        void SizeConstraints::apply(ws::rectangle_t *rect, float scale)
        {
            ws::size_limit_t l;
            compute(&l, lsp_max(0.0f, scale));
            apply(rect, &l);
        }

        void SizeConstraints::tapply(ws::rectangle_t *rect, float scale)
        {
            ws::size_limit_t l;
            compute(&l, lsp_max(0.0f, scale));
            tapply(rect, &l);
        }

        void SizeConstraints::apply(ws::rectangle_t *dst, const ws::size_limit_t *sc)
        {
            if ((sc->nMaxWidth  >= 0) && (dst->nWidth  > sc->nMaxWidth))
                dst->nWidth     = sc->nMaxWidth;
            if ((sc->nMaxHeight >= 0) && (dst->nHeight > sc->nMaxHeight))
                dst->nHeight    = sc->nMaxHeight;
            if ((sc->nMinWidth  >= 0) && (dst->nWidth  < sc->nMinWidth))
                dst->nWidth     = sc->nMinWidth;
            if ((sc->nMinHeight >= 0) && (dst->nHeight < sc->nMinHeight))
                dst->nHeight    = sc->nMinHeight;
        }

        void SizeConstraints::tapply(ws::rectangle_t *dst, const ws::size_limit_t *sc)
        {
            if ((sc->nMaxHeight >= 0) && (dst->nWidth  > sc->nMaxHeight))
                dst->nWidth     = sc->nMaxHeight;
            if ((sc->nMaxWidth  >= 0) && (dst->nHeight > sc->nMaxWidth ))
                dst->nHeight    = sc->nMaxWidth;
            if ((sc->nMinHeight >= 0) && (dst->nWidth  < sc->nMinHeight))
                dst->nWidth     = sc->nMinHeight;
            if ((sc->nMinWidth  >= 0) && (dst->nHeight < sc->nMinWidth ))
                dst->nHeight    = sc->nMinWidth;
        }

        void SizeConstraints::apply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *sc)
        {
            dst->nLeft      = src->nLeft;
            dst->nTop       = src->nTop;
            dst->nWidth     = ((sc->nMaxWidth  >= 0) && (src->nWidth  > sc->nMaxWidth))  ? sc->nMaxWidth  : src->nWidth;
            dst->nHeight    = ((sc->nMaxHeight >= 0) && (src->nHeight > sc->nMaxHeight)) ? sc->nMaxHeight : src->nHeight;

            if ((sc->nMinWidth  >= 0) && (dst->nWidth  < sc->nMinWidth ))
                dst->nWidth     = sc->nMinWidth;
            if ((sc->nMinHeight >= 0) && (dst->nHeight < sc->nMinHeight))
                dst->nHeight    = sc->nMinHeight;
        }

        void SizeConstraints::tapply(ws::rectangle_t *dst, const ws::rectangle_t *src, const ws::size_limit_t *sc)
        {
            dst->nLeft      = src->nLeft;
            dst->nTop       = src->nTop;
            dst->nWidth     = ((sc->nMaxHeight >= 0) && (src->nWidth  > sc->nMaxHeight)) ? sc->nMaxHeight : src->nWidth;
            dst->nHeight    = ((sc->nMaxWidth  >= 0) && (src->nHeight > sc->nMaxWidth )) ? sc->nMaxWidth  : src->nHeight;

            if ((sc->nMinHeight >= 0) && (dst->nWidth  < sc->nMinHeight))
                dst->nWidth     = sc->nMinHeight;
            if ((sc->nMinWidth  >= 0) && (dst->nHeight < sc->nMinWidth ))
                dst->nHeight    = sc->nMinWidth;
        }

        namespace prop
        {
            status_t SizeConstraints::init(Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
            {
                ws::size_limit_t sl;
                sl.nMinWidth    = min_width;
                sl.nMinHeight   = min_height;
                sl.nMaxWidth    = max_width;
                sl.nMaxHeight   = max_height;

                return init(style, &sl);
            }

            status_t SizeConstraints::init(Style *style)
            {
                ws::size_limit_t sl;
                sl.nMinWidth    = -1;
                sl.nMinHeight   = -1;
                sl.nMaxWidth    = -1;
                sl.nMaxHeight   = -1;

                return init(style, &sl);
            }

            status_t SizeConstraints::init(Style *style, const ws::size_limit_t *p)
            {
                if ((pStyle == NULL) || (p == NULL))
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_MIN_WIDTH], p->nMinWidth);
                    style->create_int(vAtoms[P_MIN_HEIGHT], p->nMinHeight);
                    style->create_int(vAtoms[P_MAX_WIDTH], p->nMaxWidth);
                    style->create_int(vAtoms[P_MAX_HEIGHT], p->nMaxHeight);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%ld %ld %ld %ld", long(p->nMinWidth), long(p->nMinHeight), long(p->nMaxWidth), long(p->nMaxHeight));
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }

            status_t SizeConstraints::override(Style *style, ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height)
            {
                ws::size_limit_t sl;
                sl.nMinWidth    = min_width;
                sl.nMinHeight   = min_height;
                sl.nMaxWidth    = max_width;
                sl.nMaxHeight   = max_height;

                return override(style, &sl);
            }

            status_t SizeConstraints::override(Style *style)
            {
                ws::size_limit_t sl;
                sl.nMinWidth    = -1;
                sl.nMinHeight   = -1;
                sl.nMaxWidth    = -1;
                sl.nMaxHeight   = -1;

                return override(style, &sl);
            }

            status_t SizeConstraints::override(Style *style, const ws::size_limit_t *p)
            {
                if ((pStyle == NULL) || (p == NULL))
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->override_int(vAtoms[P_MIN_WIDTH], p->nMinWidth);
                    style->override_int(vAtoms[P_MIN_HEIGHT], p->nMinHeight);
                    style->override_int(vAtoms[P_MAX_WIDTH], p->nMaxWidth);
                    style->override_int(vAtoms[P_MAX_HEIGHT], p->nMaxHeight);

                    // Compound objects
                    LSPString s;
                    s.fmt_ascii("%ld %ld %ld %ld", long(p->nMinWidth), long(p->nMinHeight), long(p->nMaxWidth), long(p->nMaxHeight));
                    style->override_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }
    } /* namespace tk */
} /* namespace lsp */


