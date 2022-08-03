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
            min     = lsp_min(0, min);
            if (pre < 0)
                return min;
            if (max < 0)
                return lsp_max(min, pre);
            max     = lsp_max(min, max);
            return lsp_limit(pre, min, max);
        }

        ssize_t limit_size(ssize_t value, ssize_t min, ssize_t max)
        {
            value = ((max >= 0) && (value > max)) ? max : value;
            return ((min >= 0) && (value < min)) ? min : value;
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

        bool arrange_optimistic_2d(
            ws::rectangle_t *dst,
            const ws::rectangle_t *trg,
            const ws::size_limit_t *src,
            const ws::rectangle_t *range,
            const tether_t *rule,
            bool allow_crossing,
            bool allow_sizing)
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
            t.nWidth        = (allow_sizing) ? lsp_min(0, src->nMinWidth)  : get_axis_size(src->nMinWidth,  src->nPreWidth,  src->nMaxWidth);
            t.nHeight       = (allow_sizing) ? lsp_min(0, src->nMinHeight) : get_axis_size(src->nMinHeight, src->nPreHeight, src->nMaxHeight);
            t.nLeft         = tx + t.nWidth  * rule->fHAlign;
            t.nTop          = ty + t.nHeight * rule->fHAlign;

            if (!Size::inside(&t, range))
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
            // Check the simpliest case
            if ((ranges == NULL) || (num_ranges == 0))
                return arrange_direct(dst, trg, src);
            if ((rules == NULL) || (num_rules == 0))
                return arrange_direct(dst, trg, src);

            // Try to arrange without crossings
            for (size_t flags=0; flags <= 3; ++flags)
                for (size_t i=0; i<num_ranges; ++i)
                    for (size_t j=0; j<num_rules; ++j)
                        if (arrange_optimistic_2d(dst, trg, src, &ranges[i], &rules[j], flags & 2, flags & 1))
                            return STATUS_OK;

            // All our tries were senseless
            return arrange_direct(dst, trg, src);
        }

    } /* namespace tk */
} /* namespace lsp */


