/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 3 сент. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_GRAPHFRAMEFUNCTION_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_GRAPHFRAMEFUNCTION_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class GraphFrameFunction: public Enum
        {
            private:
                GraphFrameFunction & operator = (const GraphFrameFunction &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit GraphFrameFunction(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline graph_frame_function_t   get() const                     { return graph_frame_function_t(nValue);        }
                inline graph_frame_function_t   set(graph_frame_function_t v)   { return graph_frame_function_t(Enum::set(v));  }

                inline bool                     is_default() const              { return nValue == GFF_DEFAULT;                 }
                inline bool                     rainbow() const                 { return nValue == GFF_RAINBOW;                 }
                inline bool                     fog_color() const               { return nValue == GFF_FOG;                     }
                inline bool                     color() const                   { return nValue == GFF_COLOR;                   }
                inline bool                     lightness() const               { return nValue == GFF_LIGHTNESS;               }
                inline bool                     lightness2() const              { return nValue == GFF_LIGHTNESS2;              }

                inline graph_frame_function_t   set_default()                   { return set(GFF_DEFAULT);                      }
                inline graph_frame_function_t   set_rainbow()                   { return set(GFF_RAINBOW);                      }
                inline graph_frame_function_t   set_fog_color()                 { return set(GFF_FOG);                          }
                inline graph_frame_function_t   set_color()                     { return set(GFF_COLOR);                        }
                inline graph_frame_function_t   set_lightness()                 { return set(GFF_LIGHTNESS);                    }
                inline graph_frame_function_t   set_lightness2()                { return set(GFF_LIGHTNESS2);                   }
        };

        namespace prop
        {
            class GraphFrameFunction: public tk::GraphFrameFunction
            {
                private:
                    GraphFrameFunction & operator = (const GraphFrameFunction &);

                public:
                    explicit GraphFrameFunction(prop::Listener *listener = NULL): tk::GraphFrameFunction(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::GraphFrameFunction::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::GraphFrameFunction::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::GraphFrameFunction::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::GraphFrameFunction::unbind(); };

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_GRAPHFRAMEFUNCTION_H_ */
