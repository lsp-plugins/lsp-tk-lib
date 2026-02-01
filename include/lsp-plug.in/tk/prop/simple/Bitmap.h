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

#ifndef LSP_PLUG_IN_TK_PROP_SIMPLE_BITMAP_H_
#define LSP_PLUG_IN_TK_PROP_SIMPLE_BITMAP_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/mm/Bitmap.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Bitmap: in-memory contents of bitmap image stored in PRGBA8888 format
         */
        class Bitmap: public Property
        {
            protected:
                mm::Bitmap                      sBitmap;

            protected:
                explicit Bitmap(prop::Listener *listener = NULL);
                Bitmap(Bitmap &) = delete;
                Bitmap(Bitmap &&) = delete;
                virtual ~Bitmap() override;

                Bitmap & operator = (const Bitmap &) = delete;
                Bitmap & operator = (Bitmap &&) = delete;

            public:
                const mm::Bitmap *bitmap() const                        { return &sBitmap;                  }

                inline mm::pixel_format_t format() const noexcept       { return sBitmap.format();          }
                inline size_t rows() const noexcept                     { return sBitmap.rows();            }
                inline size_t width() const noexcept                    { return sBitmap.width();           }
                inline size_t columns() const noexcept                  { return sBitmap.columns();         }
                inline size_t height() const noexcept                   { return sBitmap.height();          }
                inline size_t stride() const noexcept                   { return sBitmap.stride();          }
                inline size_t bytes_per_row() const noexcept            { return sBitmap.bytes_per_row();   }
                inline uint8_t *row(size_t index) noexcept              { return sBitmap.row(index);        }
                inline const uint8_t *row(size_t index) const noexcept  { return sBitmap.row(index);        }
                inline uint8_t *data() noexcept                         { return sBitmap.data();            }
                inline const uint8_t *data() const noexcept             { return sBitmap.data();            }
                inline bool is_empty() const noexcept                   { return sBitmap.is_empty();        }

            public: // mm::Bitmap interface
                status_t set(const mm::Bitmap & src);
                status_t set(const mm::Bitmap *src);
                status_t set(const Bitmap & src);
                status_t set(const Bitmap *src);

                status_t init(mm::pixel_format_t format, size_t rows, size_t cols);
                void reset();
                status_t resize(size_t rows, size_t cols);

                status_t load(const char *path, mm::IColorMap *map = NULL);
                status_t load(const LSPString *path, mm::IColorMap *map = NULL);
                status_t load(const LSPString & path, mm::IColorMap *map = NULL);
                status_t load(const io::Path *path, mm::IColorMap *map = NULL);
                status_t load(const void *data, size_t size, mm::IColorMap *map = NULL);
                status_t load(const io::Path & path, mm::IColorMap *map = NULL);
                status_t load(io::IInStream *is, mm::IColorMap *map = NULL);

                void swap(mm::Bitmap *src);
                inline void swap(mm::Bitmap &src)                       { swap(&src);                       }
                void swap(Bitmap *src);
                inline void swap(Bitmap &src)                           { swap(&src);                       }
        };

        namespace prop
        {
            class Bitmap: public tk::Bitmap
            {
                public:
                    explicit inline Bitmap(prop::Listener *listener = NULL): tk::Bitmap(listener) {};
                    Bitmap(const Bitmap &) = delete;
                    Bitmap(Bitmap &&) = delete;

                    Bitmap & operator = (const Bitmap &) = delete;
                    Bitmap & operator = (Bitmap &&) = delete;

                public:
                    inline status_t     bind(atom_t property, Style *style)             { return STATUS_OK; }
                    inline status_t     bind(const char *property, Style *style)        { return STATUS_OK; }
                    inline status_t     bind(const LSPString *property, Style *style)   { return STATUS_OK; }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */




#endif /* LSP_PLUG_IN_TK_PROP_SIMPLE_BITMAP_H_ */
