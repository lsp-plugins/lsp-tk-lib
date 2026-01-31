/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 февр. 2026 г.
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
        Bitmap::Bitmap(prop::Listener *listener):
            Property(listener)
        {
        }

        Bitmap::~Bitmap()
        {
        }

        status_t Bitmap::set(const mm::Bitmap & src)
        {
            status_t res = sBitmap.convert_from(src, mm::PIXFMT_PBGRA8888);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::set(const mm::Bitmap *src)
        {
            status_t res = sBitmap.convert_from(src, mm::PIXFMT_PBGRA8888);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::set(const Bitmap & src)
        {
            status_t res = sBitmap.convert_from(src.sBitmap, mm::PIXFMT_PBGRA8888);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::set(const Bitmap *src)
        {
            status_t res = (src != NULL) ? sBitmap.set(src->sBitmap) : STATUS_BAD_ARGUMENTS;
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::init(mm::pixel_format_t format, size_t rows, size_t cols)
        {
            status_t res = sBitmap.init(format, rows, cols);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        void Bitmap::reset()
        {
            sBitmap.reset();
            sync();
        }

        status_t Bitmap::resize(size_t rows, size_t cols)
        {
            status_t res = sBitmap.resize(rows, cols);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const char *path, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(path, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const LSPString *path, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(path, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const LSPString & path, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(path, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const io::Path *path, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(path, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const void *data, size_t size, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(data, size, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(const io::Path & path, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(path, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        status_t Bitmap::load(io::IInStream *is, mm::IColorMap *map)
        {
            status_t res = sBitmap.load(is, mm::PIXFMT_PBGRA8888, map);
            if (res == STATUS_OK)
                sync();
            return res;
        }

        void Bitmap::swap(mm::Bitmap *src)
        {
            sBitmap.swap(src);
            sync();
        }

        void Bitmap::swap(Bitmap *src)
        {
            sBitmap.swap(src->sBitmap);
            sync();
        }
    } /* namespace tk */
} /* namespace lsp */



