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

namespace lsp
{
    namespace tk
    {
        class GraphMeshData: public Property
        {
            private:
                GraphMeshData &operator = (const GraphMeshData &);

            protected:
                float          *vData;
                size_t          nSize;
                size_t          nStride;
                uint8_t        *pPtr;

            protected:
                void            copy_data(float *dst, const float *src, size_t n);

            public:
                explicit GraphMeshData(prop::Listener *listener);
                ~GraphMeshData();

            public:
                inline size_t       size() const                { return nSize;                 }
                inline size_t       capacity() const            { return nStride*2;             }
                inline bool         valid() const               { return vData != NULL;         }
                inline const float *x() const                   { return vData;                 }
                inline const float *y() const                   { return &vData[nStride];       }

                bool                set_size(size_t size);
                bool                set_x(const float *v, size_t size);
                inline bool         set_x(const float *v)       { return set_x(v, nSize);       }
                bool                set_y(const float *v, size_t size);
                inline bool         set_y(const float *v)       { return set_y(v, nSize);       }
        };

        namespace prop
        {
            class GraphMeshData: public tk::GraphMeshData
            {
                private:
                    GraphMeshData &operator = (const GraphMeshData &);

                public:
                    explicit inline GraphMeshData(prop::Listener *listener): tk::GraphMeshData(listener) {}
            };
        };
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_SPECIFIC_GRAPHMESHDATA_H_ */
