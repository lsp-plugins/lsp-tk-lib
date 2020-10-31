/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHFRAMEDATA_H_
#define LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHFRAMEDATA_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphFrameData: public MultiProperty
        {
            private:
                GraphFrameData & operator = (const GraphFrameData &);

            protected:
                static const prop::desc_t   DESC[];

                enum property_t
                {
                    P_ROWS,
                    P_COLS,
                    P_MIN,
                    P_MAX,
                    P_DFL,
                    P_SIZE,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        GraphFrameData *pValue;

                    public:
                        inline Listener(GraphFrameData *data)       { pValue = data; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                float          *vData;              // Data buffer
                size_t          nRows;              // Number of rows
                size_t          nCols;              // Number of columns
                uint32_t        nChanges;           // Number of changes
                uint32_t        nRowId;             // Current row number
                size_t          nStride;            // Stride between rows
                size_t          nCapacity;          // The real capacity, should be 2^n
                float           fMin;               // Minimum allowed value
                float           fMax;               // Maximum allowed value
                float           fDfl;               // Default value
                uint8_t        *pPtr;               // Allocated pointer

                atom_t          vAtoms[P_COUNT];    // Atoms
                Listener        sListener;          // Listener

            protected:
                void            sync();
                void            commit(atom_t property);
                bool            resize_buffer(size_t rows, size_t cols);
                ssize_t         row_index(uint32_t id, size_t range) const;
                status_t        init();
                status_t        override();

            public:
                explicit GraphFrameData(prop::Listener *listener);
                ~GraphFrameData();

            public:
                size_t          rows() const                { return nRows;                     }
                size_t          columns() const             { return nCols;                     }
                uint32_t        bottom() const              { return nRowId - nCapacity;        }
                uint32_t        first() const               { return nRowId - nRows;            }
                uint32_t        last() const                { return nRowId;                    }
                uint32_t        top() const                 { return nRowId;                    }
                uint32_t        pending() const             { return nRowId - nChanges;         }
                size_t          capacity() const            { return nCapacity;                 }
                size_t          stride() const              { return nStride;                   }
                inline bool     changed() const             { return nChanges > 0;              }
                inline size_t   changes() const             { return nChanges;                  }
                inline float    min() const                 { return fMin;                      }
                inline float    max() const                 { return fMax;                      }
                inline bool     valid() const               { return vData != NULL;             }
                float           get_default() const;
                const float    *row(uint32_t id) const;

                bool            set_size(size_t rows, size_t cols);
                float           set_min(float v);
                float           set_max(float v);
                float           set_default(float v);
                void            set_range(float min, float max, float dfl);
                inline void     set_range(float min, float max)             { set_range(min, max, fDfl);        }
                inline bool     set_rows(size_t rows)                       { return set_size(rows, nCols);     }
                inline bool     set_columns(size_t cols)                    { return set_size(nRows, cols);     }
                bool            set_row(uint32_t id, const float *data, size_t columns);
                inline bool     set_row(uint32_t id, const float *data)     { return set_row(id, data, nCols);  }
                void            clear();
                void            advance();
        };

        namespace prop
        {
            class GraphFrameData: public tk::GraphFrameData
            {
                private:
                    GraphFrameData &operator = (const GraphFrameData &);

                public:
                    explicit inline GraphFrameData(prop::Listener *listener = NULL): tk::GraphFrameData(listener) {}

                protected:
                    using               tk::GraphFrameData::init;
                    using               tk::GraphFrameData::override;

                public:
                    inline status_t     bind(atom_t property, Style *style)             { return tk::GraphFrameData::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::GraphFrameData::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::GraphFrameData::bind(property, style, vAtoms, DESC, &sListener); }

                    status_t            init(Style *style, size_t rows, size_t cols, float min, float max, float dfl);
                    inline status_t     init(Style *style, size_t rows, size_t cols) { return init(style, rows, cols, 0.0f, 1.0f, 0.0f);   }
                    inline status_t     init(Style *style, size_t rows, float min, float max, float dfl) { return init(style, 0, 0, min, max, dfl); }

                    status_t            override(Style *style, size_t rows, size_t cols, float min, float max, float dfl);
                    inline status_t     override(Style *style, float min, float max, float dfl)  { return override(style, 0, 0, min, max, dfl);     }
                    inline status_t     override(Style *style, size_t rows, size_t cols) { return override(style, rows, cols, 0.0f, 1.0f, 0.0f);    }

                    static status_t            init(const char *name, Style *style, size_t rows, size_t cols, float min, float max, float dfl);
                    static inline status_t     init(const char *name, Style *style, size_t rows, size_t cols) { return init(name, style, rows, cols, 0.0f, 1.0f, 0.0f);   }
                    static inline status_t     init(const char *name, Style *style, size_t rows, float min, float max, float dfl) { return init(name, style, 0, 0, min, max, dfl); }

                    static status_t            override(const char *name, Style *style, size_t rows, size_t cols, float min, float max, float dfl);
                    static inline status_t     override(const char *name, Style *style, float min, float max, float dfl)  { return override(name, style, 0, 0, min, max, dfl);     }
                    static inline status_t     override(const char *name, Style *style, size_t rows, size_t cols) { return override(name, style, rows, cols, 0.0f, 1.0f, 0.0f);    }
            };
        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHFRAMEDATA_H_ */
