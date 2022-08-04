/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 авг. 2022 г.
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

#include <lsp-plug.in/tk/helpers/arrange.h>

namespace lsp
{
    namespace tk
    {
        /** Just choose the right value between minimum, preferred, maximum
         *
         * @param min minimum possible value, negative if not present
         * @param pre preferred value, negative if not present
         * @param max maximum possible value, negative if not present
         * @return actual value, positive
         */
        ssize_t get_axis_size(ssize_t min, ssize_t pre, ssize_t max)
        {
            min     = lsp_max(0, min);
            if (max < 0)
                return lsp_max(min, pre);
            return lsp_max(min, max);
        }

        ssize_t limit_size(ssize_t value, ssize_t min, ssize_t max)
        {
            value = ((max >= 0) && (value > max)) ? max : value;
            return ((min >= 0) && (value < min)) ? min : value;
        }

        ssize_t make_fit_range(ssize_t v, ssize_t w, ssize_t vmin, ssize_t vmax)
        {
            if (v < vmin)
                return v;
            ssize_t ve = v + w;
            return (ve < vmax) ? v : v + vmax - ve;
        }

        void apply_stretching(
            ws::rectangle_t *dst,
            const ws::rectangle_t *src,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *sc,
            wssize_t flags)
        {
            if (flags & TF_HSTRETCH)
            {
                ssize_t l   = lsp_min(src->nLeft, trg->nLeft);
                ssize_t r   = lsp_max(src->nLeft + src->nWidth, trg->nLeft + trg->nWidth);
                ssize_t w   = limit_size(r - l, sc->nMinWidth, sc->nMaxWidth);

                dst->nLeft  = (flags & TF_RIGHT) ? r - w : l;
                dst->nWidth = w;
            }
            else
            {
                dst->nLeft  = src->nLeft;
                dst->nWidth = src->nWidth;
            }

            if (flags & TF_VSTRETCH)
            {
                ssize_t t   = lsp_min(src->nTop, trg->nTop);
                ssize_t b   = lsp_max(src->nHeight + src->nHeight, trg->nTop + trg->nHeight);
                ssize_t h   = limit_size(b - t, sc->nMinWidth, sc->nMaxWidth);

                dst->nTop   = (flags & TF_BOTTOM) ? b - t : t;
                dst->nHeight= h;
            }
            else
            {
                dst->nTop   = src->nTop;
                dst->nHeight= src->nHeight;
            }
        }

        /**
         * Do the direct arrangement
         * @param dst destination rectangle
         * @param trg trigger rectangle
         * @param src source rectangle sizing parameters
         * @return status of operation
         */
        status_t arrange_direct(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src)
        {
            // Choose position being same to the original rectangle
            dst->nLeft      = trg->nLeft;
            dst->nTop       = trg->nTop;
            dst->nWidth     = get_axis_size(src->nMinWidth,  src->nPreWidth,  src->nMaxWidth );
            dst->nHeight    = get_axis_size(src->nMinHeight, src->nPreHeight, src->nMaxHeight);

            return STATUS_OK;
        }

        bool arrange_optimistic(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src,
            const tether_t *rule,
            const ws::rectangle_t *range,
            bool allow_sizing,
            bool allow_crossing,
            bool allow_directions)
        {
            // Update the parameters of triggered rectangle
            ws::rectangle_t xtrg, t;
            if (allow_crossing)
            {
                if (!Size::intersection(&xtrg, trg, range))
                    return false;
                trg     = &xtrg;
            }
            else if (!Size::inside(range, trg)) // The triggered rectangle should be strictly placed inside of defined range
                return false;

            // Compute right and bottom boundaries
            ssize_t right   = trg->nLeft + trg->nWidth;
            ssize_t bottom  = trg->nTop  + trg->nHeight;

            // Compute tether coordinages
            ssize_t tx      = (rule->nFlags & TF_RIGHT)  ? right  : trg->nLeft;
            ssize_t ty      = (rule->nFlags & TF_BOTTOM) ? bottom : trg->nTop;

            // Now get the size for both axes
            t.nWidth        = get_axis_size(src->nMinWidth,  src->nPreWidth,  src->nMaxWidth);
            t.nHeight       = get_axis_size(src->nMinHeight, src->nPreHeight, src->nMaxHeight);
            t.nLeft         = tx + (rule->fHAlign * 0.5f - 0.5f) * t.nWidth;
            t.nTop          = ty + (rule->fVAlign * 0.5f - 0.5f) * t.nHeight;

            // Maximize if necessary
            if (rule->nFlags & TF_HMAXIMIZE)
                t.nLeft         = make_fit_range(t.nLeft, t.nWidth,  range->nLeft, range->nLeft + range->nWidth);
            if (rule->nFlags & TF_VMAXIMIZE)
                t.nTop          = make_fit_range(t.nTop,  t.nHeight, range->nTop,  range->nTop  + range->nHeight);

            // Check the pririty
            if (allow_directions)
            {
                // Check the pririty of each direction and shift the range
                if (rule->nFlags & TF_VERTICAL)
                    t.nLeft         = make_fit_range(t.nLeft, t.nWidth, range->nLeft, range->nLeft + range->nWidth);
                else
                    t.nTop          = make_fit_range(t.nTop, t.nHeight, range->nTop, range->nTop + range->nHeight);
            }

            if (!Size::inside(range, &t))
            {
                // Need to do extra job related to the trimming of the rectangle
                if (!allow_sizing)
                    return false;

                // Try to reduce size and check that size constraints match
                if (!Size::intersection(&t, range))
                    return false;
                if (!SizeConstraints::match(&t, src))
                    return false;
            }

            // Apply stretching regarding to size constraints
            apply_stretching(dst, &t, trg, src, rule->nFlags);
            return true;
        }

        bool arrange_full_area(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src,
            const ws::rectangle_t *range,
            bool match_rect,
            bool force)
        {
            ws::rectangle_t t;

            // Check that we hit the rectangle
            if ((match_rect) && (!Size::overlap(trg, range)))
                return false;

            // Now get the size for both axes
            t.nLeft         = trg->nLeft;
            t.nTop          = trg->nTop;
            t.nWidth        = get_axis_size(src->nMinWidth,  src->nPreWidth,  src->nMaxWidth);
            t.nHeight       = get_axis_size(src->nMinHeight, src->nPreHeight, src->nMaxHeight);

            // Place the window to fit the range
            t.nLeft         = make_fit_range(t.nLeft, t.nWidth, range->nLeft, range->nLeft + range->nWidth);
            t.nTop          = make_fit_range(t.nTop, t.nHeight, range->nTop, range->nTop + range->nHeight);
            if (Size::inside(range, &t))
            {
                *dst            = t;
                return true;
            }
            else if (!force)
                return false;

            Size::intersection(dst, &t, range);
            return true;
        }

        status_t arrange_rectangle(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src,
            const ws::rectangle_t *ranges,
            size_t num_ranges,
            const tether_t *rules,
            size_t num_rules)
        {
            // Validate arguments
            if ((dst == NULL) || (trg == NULL) || (src == NULL))
                return STATUS_BAD_ARGUMENTS;
            // Check the simpliest case, when there are no rules
            if ((ranges == NULL) || (num_ranges == 0))
                return arrange_direct(dst, trg, src);
            if ((rules == NULL) || (num_rules == 0))
                return arrange_direct(dst, trg, src);

            // Try to perform optimistic arrange
            for (size_t flags=0; flags <= 7; ++flags)
                for (size_t i=0; i<num_rules; ++i)
                    for (size_t j=0; j<num_ranges; ++j)
                        if (arrange_optimistic(dst, trg, src, &rules[i], &ranges[j], flags & 1, flags & 2, flags & 4))
                            return STATUS_OK;

            for (size_t flags=0; flags <= 3; ++flags)
                for (size_t j=0; j<num_ranges; ++j)
                    if (arrange_full_area(dst, trg, src, &ranges[j], !(flags & 1), !(flags & 2)) == STATUS_OK)
                        return STATUS_OK;

            // All our tries were senseless
            return arrange_direct(dst, trg, src);
        }

    } /* namespace tk */
} /* namespace lsp */


