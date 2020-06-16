/*
 * WindowActions.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_FLAGS_WINDOWACTIONS_H_
#define LSP_PLUG_IN_TK_PROP_FLAGS_WINDOWACTIONS_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;

        class WindowActions: public BitEnum
        {
            private:
                WindowActions & operator = (const WindowActions &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit WindowActions(prop::Listener *listener = NULL): BitEnum(ENUM, listener) {};

            public:
                inline bool     allowed(ws::window_action_t wa) const   { return nValue & wa;                   }
                inline bool     denied(ws::window_action_t wa) const    { return !(nValue & wa);                }
                inline bool     get(ws::window_action_t wa) const       { return nValue & wa;                   }
                inline size_t   actions() const                         { return nValue;                        }
                inline size_t   get_all() const                         { return nValue;                        }
                inline bool     movable() const                         { return nValue & ws::WA_MOVE;          }
                inline bool     resizable() const                       { return nValue & ws::WA_RESIZE;        }
                inline bool     minimizable() const                     { return nValue & ws::WA_MINIMIZE;      }
                inline bool     maximizable() const                     { return nValue & ws::WA_MAXIMIZE;      }
                inline bool     closeable() const                       { return nValue & ws::WA_CLOSE;         }
                inline bool     stickable() const                       { return nValue & ws::WA_STICK;         }
                inline bool     shadeable() const                       { return nValue & ws::WA_SHADE;         }
                inline bool     fullscreenable() const                  { return nValue & ws::WA_FULLSCREEN;    }
                inline bool     transportable() const                   { return nValue & ws::WA_CHANGE_DESK;   }

            public:
                inline void     set_actions(size_t flags)               { xset_all(flags);                      }
                inline void     toggle_actions(size_t flags)            { xset_all(nValue ^ flags);             }
                inline void     allow_actions(size_t flags)             { xset_all(nValue | flags);             }
                inline void     deny_actions(size_t flags)              { xset_all(nValue & (~flags));          }

                inline void     set(ws::window_action_t wa)             { xset(wa);                             }
                inline void     toggle(ws::window_action_t wa)          { xtoggle(wa);                          }
                inline void     allow(ws::window_action_t wa)           { xset(wa);                             }
                inline void     deny(ws::window_action_t wa)            { xunset(wa);                           }

                inline void     set_movable()                           { xset(ws::WA_MOVE);                    }
                inline void     set_movable(bool value)                 { xset(ws::WA_MOVE, value);             }

                inline void     set_resizable()                         { xset(ws::WA_RESIZE);                  }
                inline void     set_resizable(bool value)               { xset(ws::WA_RESIZE, value);           }

                inline void     set_minimizable()                       { xset(ws::WA_MINIMIZE);                }
                inline void     set_minimizable(bool value)             { xset(ws::WA_MINIMIZE, value);         }

                inline void     set_maximizable()                       { xset(ws::WA_MAXIMIZE);                }
                inline void     set_maximizable(bool value)             { xset(ws::WA_MAXIMIZE, value);         }

                inline void     set_closeable()                         { xset(ws::WA_CLOSE);                   }
                inline void     set_closeable(bool value)               { xset(ws::WA_CLOSE, value);            }

                inline void     set_stickable()                         { xset(ws::WA_STICK);                   }
                inline void     set_stickable(bool value)               { xset(ws::WA_STICK, value);            }

                inline void     set_shadeable()                         { xset(ws::WA_SHADE);                   }
                inline void     set_shadeable(bool value)               { xset(ws::WA_SHADE, value);            }

                inline void     set_fullscreenable()                    { xset(ws::WA_FULLSCREEN);              }
                inline void     set_fullscreenable(bool value)          { xset(ws::WA_FULLSCREEN, value);       }

                inline void     set_transportable()                     { xset(ws::WA_CHANGE_DESK);             }
                inline void     set_transportable(bool value)           { xset(ws::WA_CHANGE_DESK, value);      }

                inline void     allow_all()                             { xset_all(ws::WA_ALL);                 }
                inline void     deny_all()                              { xset_all(ws::WA_NONE);                }
                inline void     toggle_all()                            { xset_all(nValue ^ ws::WA_ALL);        }
        };

        namespace prop
        {
            class WindowActions: public tk::WindowActions
            {
                private:
                    WindowActions & operator = (const WindowActions &);

                public:
                    explicit WindowActions(prop::Listener *listener = NULL): tk::WindowActions(listener) {};

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::WindowActions::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::WindowActions::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::WindowActions::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::WindowActions::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param actions set of actions
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, size_t actions)              { return tk::WindowActions::init(style, actions);       }
                    inline status_t     override(Style *style, size_t actions)          { return tk::WindowActions::override(style, actions);   }
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_FLAGS_WINDOWACTIONS_H_ */
