/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHMESHDATA_H_
#define LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHMESHDATA_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphMeshData: public MultiProperty
        {
            private:
                GraphMeshData &operator = (const GraphMeshData &);
                GraphMeshData(const GraphMeshData &);

            protected:
                static const prop::desc_t   DESC[];

                enum property_t
                {
                    P_SIZE,
                    P_STROBE,

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        GraphMeshData  *pValue;

                    public:
                        inline Listener(GraphMeshData *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                float          *vData;
                size_t          nSize;
                size_t          nStride;
                bool            bStrobe;
                uint8_t        *pPtr;

                atom_t          vAtoms[P_COUNT];    // Atoms
                Listener        sListener;          // Listener

            protected:
                void            copy_data(float *dst, const float *src, size_t n);
                void            sync();
                void            commit(atom_t property);
                bool            resize_buffer(size_t size, bool strobe);

            public:
                explicit GraphMeshData(prop::Listener *listener);
                virtual ~GraphMeshData();

            public:
                inline size_t       size() const                { return nSize;                                 }
                inline size_t       capacity() const            { return nStride*(2 + bStrobe);                 }
                inline bool         valid() const               { return vData != NULL;                         }
                inline bool         strobe() const              { return bStrobe;                               }
                inline const float *x() const                   { return vData;                                 }
                inline const float *y() const                   { return &vData[nStride];                       }
                inline const float *s() const                   { return (bStrobe) ? &vData[nStride*2] : NULL;  }

                bool                set_size(size_t size, bool strobe);
                bool                set_size(size_t size);
                bool                set_strobe(bool strobe);
                bool                set_x(const float *v, size_t size);
                inline bool         set_x(const float *v)       { return set_x(v, nSize);       }
                bool                set_y(const float *v, size_t size);
                inline bool         set_y(const float *v)       { return set_y(v, nSize);       }
                bool                set_s(const float *v, size_t size);
                inline bool         set_s(const float *v)       { return set_s(v, nSize);       }
                bool                set(const float *x, const float *y, size_t size);
        };

        namespace prop
        {
            class GraphMeshData: public tk::GraphMeshData
            {
                private:
                    GraphMeshData &operator = (const GraphMeshData &);
                    GraphMeshData(const GraphMeshData &);

                public:
                    explicit inline GraphMeshData(prop::Listener *listener = NULL): tk::GraphMeshData(listener) {}

                public:
                    inline status_t     bind(atom_t property, Style *style)             { return tk::GraphMeshData::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::GraphMeshData::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::GraphMeshData::bind(property, style, vAtoms, DESC, &sListener); }
            };
        };
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHMESHDATA_H_ */
