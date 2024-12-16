/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 13 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/tk/sys/Slot.h>

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
                enum property_t
                {
                    P_VALUE,    // Value: Ctrl+Alt+Space
                    P_MOD,      // Modifiers: Ctrl, Alt
                    P_KEY,      // Key: Space

                    P_COUNT
                };

            protected:
                static const prop::desc_t   DESC[];
                static const prop::enum_t   MODLIST[];

            protected:
                atom_t              vAtoms[P_COUNT];    // Atom bindings
                size_t              nMod;               // Modifiers
                ws::code_t          nKey;               // Key
                Slot                sSlot;              // Event handling

            protected:
                static status_t     append_modifier(LSPString *s, size_t mod, size_t index);
                static status_t     append_key(LSPString *s, ws::code_t key);

                static status_t     format_value(LSPString *s, ws::code_t key, size_t mod);
                static status_t     format_modifiers(LSPString *s, size_t mod);
                static status_t     format_key(LSPString *s, ws::code_t key);

                static bool         check_modifier(size_t mod, size_t check);
                static bool         check_modifiers(size_t mod, size_t check);

            protected:
                virtual void        push();
                virtual void        commit(atom_t property);

                void                parse_value(const LSPString *s);
                static ws::code_t   parse_key(const LSPString *s);
                static size_t       parse_modifiers(const LSPString *s);
                static size_t       parse_modifier(const LSPString *s);

            protected:
                explicit Shortcut(prop::Listener *listener = NULL);
                Shortcut(const Shortcut &) = delete;
                Shortcut(Shortcut &&) = delete;

                Shortcut & operator = (const Shortcut &) = delete;
                Shortcut & operator = (Shortcut &&) = delete;

            public:
                virtual ~Shortcut();

            public:
                inline bool         modifier(key_modifier_t v) const    { return (nMod & v) == v;               }
                inline size_t       modifiers() const                   { return nMod;                          }

                inline bool         lctrl() const                       { return modifier(KM_LCTRL);            }
                inline bool         rctrl() const                       { return modifier(KM_RCTRL);            }
                inline bool         ctrl() const                        { return modifier(KM_CTRL);             }

                inline bool         lalt() const                        { return modifier(KM_LALT);             }
                inline bool         ralt() const                        { return modifier(KM_RALT);             }
                inline bool         alt() const                         { return modifier(KM_ALT);              }

                inline bool         lshift() const                      { return modifier(KM_LSHIFT);           }
                inline bool         rshift() const                      { return modifier(KM_RSHIFT);           }
                inline bool         shift() const                       { return modifier(KM_SHIFT);            }

                inline bool         lmeta() const                       { return modifier(KM_LMETA);            }
                inline bool         rmeta() const                       { return modifier(KM_RMETA);            }
                inline bool         meta() const                        { return modifier(KM_META);             }

                inline bool         lsuper() const                      { return modifier(KM_LSUPER);           }
                inline bool         rsuper() const                      { return modifier(KM_RSUPER);           }
                inline bool         super() const                       { return modifier(KM_SUPER);            }

                inline bool         lhyper() const                      { return modifier(KM_LHYPER);           }
                inline bool         rhyper() const                      { return modifier(KM_RHYPER);           }
                inline bool         hyper() const                       { return modifier(KM_HYPER);            }

                inline ws::code_t   key() const                         { return nKey;                          }

                inline bool         valid() const                       { return nKey != ws::WSK_UNKNOWN;       }

                ws::code_t          set(ws::code_t key);
                void                set(ws::code_t key, size_t mod);
                inline ws::code_t   set_key(ws::code_t key)             { return set(key);                      }
                size_t              set_modifiers(size_t mod);
                inline size_t       add_modifiers(size_t mod)           { return set_modifiers(nMod | mod);     }
                inline size_t       remove_modifiers(size_t mod)        { return set_modifiers(nMod & (~mod));  }
                inline size_t       toggle_modifiers(size_t mod)        { return set_modifiers(nMod ^ mod);     }

                inline void         clear()                             { set(ws::WSK_UNKNOWN, KM_NONE);        }

                inline Slot        *slot()                              { return &sSlot; }

                bool                equals(const Shortcut *scut) const;
                bool                equals(const Shortcut &scut) const;

                /**
                 * Set key and modifiers to be equal to the passed shortcut
                 * @param scut passed shortcut
                 */
                void                set(const Shortcut *scut);

                /**
                 * Set key and modifiers to be equal to the passed shortcut
                 * @param scut passed shortcut
                 */
                void                set(const Shortcut &scut);

                /**
                 * Check that shortcut must trigger
                 * @param key key to check
                 * @param mod modifier
                 * @return true if shortcut must trigger
                 */
                bool                match(ws::code_t key, size_t mod) const;

                /**
                 * Check if shortcut matches another shortcut
                 * @param scut shotcut to check
                 * @return true if shortcut matches the passed shortcut
                 */
                bool                match(const Shortcut *scut) const;
                bool                match(const Shortcut &scut) const;

                /**
                 * Format state to the string presentation
                 * @param s string to perform format
                 * @return status of operation
                 */
                status_t            format(LSPString *s);
        };

        namespace prop
        {
            class Shortcut: public tk::Shortcut
            {
                public:
                    inline Shortcut(prop::Listener *listener = NULL): tk::Shortcut(listener) {}
                    Shortcut(const Shortcut &) = delete;
                    Shortcut(Shortcut &&) = delete;

                    Shortcut & operator = (const Shortcut &) = delete;
                    Shortcut & operator = (Shortcut &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::Shortcut::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::Shortcut::bind(property, style, vAtoms, DESC, &sListener); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::Shortcut::bind(property, style, vAtoms, DESC, &sListener); }
            };
        }
    } /* namespace tk */
} /* namespace lsp */


#endif /* LSP_PLUG_IN_TK_PROP_MULTI_SHORTCUT_H_ */
