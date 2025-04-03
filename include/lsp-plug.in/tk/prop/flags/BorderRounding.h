/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 2 апр. 2025 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_FLAGS_BORDERROUNDING_H_
#define LSP_PLUG_IN_TK_PROP_FLAGS_BORDERROUNDING_H_

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;

        class BorderRounding: public BitEnum
        {
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit BorderRounding(prop::Listener *listener = NULL): BitEnum(ENUM, listener) {};
                BorderRounding(const BorderRounding &) = delete;
                BorderRounding(BorderRounding &&) = delete;

                BorderRounding & operator = (const BorderRounding &) = delete;
                BorderRounding & operator = (BorderRounding &&) = delete;

            public:
                inline bool     left_top() const                        { return nValue & ws::CORNER_LEFT_TOP;      }
                inline bool     right_top() const                       { return nValue & ws::CORNER_RIGHT_TOP;     }
                inline bool     left_bottom() const                     { return nValue & ws::CORNER_LEFT_BOTTOM;   }
                inline bool     right_bottom() const                    { return nValue & ws::CORNER_RIGHT_BOTTOM;  }
                inline bool     left() const                            { return (nValue & ws::CORNERS_LEFT) == ws::CORNERS_LEFT;       }
                inline bool     right() const                           { return (nValue & ws::CORNERS_RIGHT) == ws::CORNERS_RIGHT;     }
                inline bool     top() const                             { return (nValue & ws::CORNERS_TOP) == ws::CORNERS_TOP;         }
                inline bool     bottom() const                          { return (nValue & ws::CORNERS_BOTTOM) == ws::CORNERS_BOTTOM;   }
                inline bool     all() const                             { return nValue == ws::CORNERS_ALL;         }
                inline size_t   corners() const                         { return nValue;                            }

            public:
                inline void     set_corners(size_t flags)               { xset_all(flags & ws::CORNERS_ALL);                }
                inline void     toggle_corners(size_t flags)            { xset_all((nValue ^ flags) & ws::CORNERS_ALL);     }
                inline void     add_corners(size_t flags)               { xset_all((nValue | flags) & ws::CORNERS_ALL);     }
                inline void     remove_corners(size_t flags)            { xset_all((nValue & (~flags)) & ws::CORNERS_ALL);  }

                inline void     set(ws::corner_t c)                     { xset(c);                                  }
                inline void     toggle(ws::corner_t c)                  { xtoggle(c);                               }
                inline void     add(ws::corner_t c)                     { xset(c);                                  }
                inline void     remove(ws::corner_t c)                  { xunset(c);                                }

                inline void     set_left_top()                          { xset(ws::CORNER_LEFT_TOP);                }
                inline void     set_left_top(bool value)                { xset(ws::CORNER_LEFT_TOP, value);         }
                inline void     set_right_top()                         { xset(ws::CORNER_RIGHT_TOP);               }
                inline void     set_right_top(bool value)               { xset(ws::CORNER_RIGHT_TOP, value);        }
                inline void     set_left_bottom()                       { xset(ws::CORNER_LEFT_BOTTOM);             }
                inline void     set_left_bottom(bool value)             { xset(ws::CORNER_LEFT_BOTTOM, value);      }
                inline void     set_right_bottom()                      { xset(ws::CORNER_RIGHT_BOTTOM);            }
                inline void     set_right_bottom(bool value)            { xset(ws::CORNER_RIGHT_BOTTOM, value);     }

                inline void     set_all()                               { xset_all(ws::CORNERS_ALL);                }
                inline void     remove_all()                            { xset_all(ws::CORNERS_NONE);               }
                inline void     set_none()                              { xset_all(ws::CORNERS_NONE);               }
                inline void     toggle_all()                            { xset_all(nValue ^ ws::CORNERS_ALL);       }
        };

        namespace prop
        {
            class BorderRounding: public tk::BorderRounding
            {
                public:
                    explicit BorderRounding(prop::Listener *listener = NULL): tk::BorderRounding(listener) {};
                    BorderRounding(const BorderRounding &) = delete;
                    BorderRounding(BorderRounding &&) = delete;

                    BorderRounding & operator = (const BorderRounding &) = delete;
                    BorderRounding & operator = (BorderRounding &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::BorderRounding::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::BorderRounding::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::BorderRounding::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::BorderRounding::unbind(); };
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_FLAGS_BORDERROUNDING_H_ */
