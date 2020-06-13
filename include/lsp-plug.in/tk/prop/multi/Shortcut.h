/*
 * Shortcut.h
 *
 *  Created on: 13 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_

namespace lsp
{
    namespace tk
    {
        /**
         * Shortcut property.
         * Allows to define and trigger keyboard shortcuts
         */
        class Shortcut: public MultiProperty
        {
            protected:
                Shortcut & operator = (const Shortcut &);

            protected:
                enum property_t
                {
                    P_VALUE,    // Value: Ctrl+Alt+Space
                    P_MOD,      // Modifiers: Ctrl, Alt
                    P_KEY,      // Key: Space

                    P_COUNT
                };

                class Listener: public IStyleListener
                {
                    private:
                        Shortcut        *pValue;

                    public:
                        inline Listener(Shortcut *color)   { pValue = color; };

                    public:
                        virtual void    notify(atom_t property);
                };

            protected:
                static const prop::desc_t   DESC[];
                static const prop::enum_t   MODLIST[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                size_t              nMod;               // Modifiers
                ws::code_t          nKey;               // Key
                Listener            sListener;          // Listener

            protected:
                static status_t     append_modifier(LSPString *s, size_t mod, size_t index);
                static status_t     append_key(LSPString *s, ws::code_t key);

                static status_t     format_value(LSPString *s, ws::code_t key, size_t mod);
                static status_t     format_modifiers(LSPString *s, size_t mod);
                static status_t     format_key(LSPString *s, ws::code_t key);

            protected:
                void                sync();
                void                commit(atom_t property);
                void                parse_value(const LSPString *s);
                void                parse_key(const LSPString *s);
                void                parse_modifiers(const LSPString *s);

            protected:
                explicit Shortcut(prop::Listener *listener = NULL);
                ~Shortcut();

            public:
                inline bool         modifier(key_modifier_t v) const    { return (nMod & v) == v;       }

                inline bool         lctrl() const                       { return modifier(KM_LCTRL);    }
                inline bool         rctrl() const                       { return modifier(KM_RCTRL);    }
                inline bool         ctrl() const                        { return modifier(KM_CTRL);     }

                inline bool         lalt() const                        { return modifier(KM_LALT);     }
                inline bool         ralt() const                        { return modifier(KM_RALT);     }
                inline bool         alt() const                         { return modifier(KM_ALT);      }

                inline bool         lshift() const                      { return modifier(KM_LSHIFT);   }
                inline bool         rshift() const                      { return modifier(KM_RSHIFT);   }
                inline bool         shift() const                       { return modifier(KM_SHIFT);    }

                inline bool         lmeta() const                       { return modifier(KM_LMETA);    }
                inline bool         rmeta() const                       { return modifier(KM_RMETA);    }
                inline bool         meta() const                        { return modifier(KM_META);     }

                inline bool         lsuper() const                      { return modifier(KM_LSUPER);   }
                inline bool         rsuper() const                      { return modifier(KM_RSUPER);   }
                inline bool         super() const                       { return modifier(KM_SUPER);    }

                inline bool         lhyper() const                      { return modifier(KM_LHYPER);   }
                inline bool         rhyper() const                      { return modifier(KM_RHYPER);   }
                inline bool         hyper() const                       { return modifier(KM_HYPER);    }

                inline ws::code_t   key() const                         { return nKey;                  }

                ws::code_t          set(ws::code_t key);
                void                set(ws::code_t key, size_t mod);
                inline ws::code_t   set_key(ws::code_t key)             { return set(key);              }
                size_t              set_modifiers(size_t mod);
                inline size_t       add_modifiers(size_t mod)           { return set_modifiers(nMod | mod);     }
                inline size_t       remove_modifiers(size_t mod)        { return set_modifiers(nMod & (~mod));  }
                inline size_t       toggle_modifiers(size_t mod)        { return set_modifiers(nMod ^ mod);     }

                /**
                 * Check that shortcut must trigger
                 * @param key key to check
                 * @param mod modifier
                 * @return true if shortcut must trigger
                 */
                bool                check(ws::code_t key, size_t mod);

                /**
                 * Format state to the string presentation
                 * @param s string to perform format
                 * @return status of operation
                 */
                status_t            format(LSPString *s);
        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_ */
