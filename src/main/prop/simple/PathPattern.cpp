/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 окт. 2020 г.
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
        PathPattern::PathPattern(prop::Listener *listener):
            Property(listener)
        {
        }

        PathPattern::~PathPattern()
        {
        }

        status_t PathPattern::set(const io::PathPattern *pattern, size_t flags)
        {
            status_t res = sPattern.set(pattern->get(), flags);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set(const LSPString *pattern, size_t flags)
        {
            status_t res = sPattern.set(pattern, flags);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set(const io::Path *pattern, size_t flags)
        {
            status_t res = sPattern.set(pattern, flags);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set(const char *pattern, size_t flags)
        {
            status_t res = sPattern.set(pattern, flags);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set_pattern(const io::PathPattern *pattern)
        {
            status_t res = sPattern.set_pattern(pattern);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set_pattern(const LSPString *pattern)
        {
            status_t res = sPattern.set_pattern(pattern);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set_pattern(const io::Path *pattern)
        {
            status_t res = sPattern.set_pattern(pattern);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t PathPattern::set_pattern(const char *pattern)
        {
            status_t res = sPattern.set_pattern(pattern);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        size_t PathPattern::set_flags(size_t flags)
        {
            size_t old = sPattern.set_flags(flags);
            if (old != flags)
                sync();
            return old;
        }

        void PathPattern::swap(PathPattern *src)
        {
            sPattern.swap(&src->sPattern);
            src->sync();
            sync();
        }

        void PathPattern::sync()
        {
            if (pListener != NULL)
                pListener->notify(this);
        }
    }
}

