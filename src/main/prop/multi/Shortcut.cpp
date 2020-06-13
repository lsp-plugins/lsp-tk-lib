/*
 * Shortcut.cpp
 *
 *  Created on: 14 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        typedef struct mod_desc_t
        {
            const char *left;
            const char *right;
            const char *both;
        } mod_desc_t;

        const prop::desc_t Shortcut::DESC[] =
        {
            { "",           PT_STRING   },
            { ".mod",       PT_STRING   },
            { ".key",       PT_STRING   },
            { NULL,         PT_UNKNOWN  }
        };

        const prop::enum_t Shortcut::MODLIST[] =
        {
            { "lctrl",      KM_LCTRL    },
            { "rctrl",      KM_RCTRL    },
            { "ctrl",       KM_CTRL     },
            { "lalt",       KM_LALT     },
            { "ralt",       KM_RALT     },
            { "alt",        KM_ALT      },
            { "lshift",     KM_LSHIFT   },
            { "rshift",     KM_RSHIFT   },
            { "shift",      KM_SHIFT    },
            { "lmeta",      KM_LMETA    },
            { "rmeta",      KM_RMETA    },
            { "meta",       KM_META     },
            { "lsuper",     KM_LSUPER   },
            { "rsuper",     KM_RSUPER   },
            { "super",      KM_SUPER    },
            { "lhyper",     KM_LHYPER   },
            { "rhyper",     KM_RHYPER   },
            { "hyper",      KM_HYPER    },
            { NULL,         0           }
        };

        static const mod_desc_t MOD_DESC[] =
        {
            { "LCtrl", "RCtrl", "Ctrl"      },
            { "LShift", "RShift", "Shift"   },
            { "LAlt", "RAlt", "Alt"         },
            { "LMeta", "RMeta", "Meta"      },
            { "LSuper", "RSuper", "Super"   },
            { "LHyper", "RHyper", "Hyper"   }
        };

        void Shortcut::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Shortcut::Shortcut(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nMod        = 0;
            nKey        = ws::WSK_UNKNOWN;
        }

        Shortcut::~Shortcut()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Shortcut::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    LSPString s;

                    // Simple components
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (format_value(&s, nKey, nMod) == STATUS_OK)
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                    if (vAtoms[P_MOD] >= 0)
                    {
                        if (format_modifiers(&s, nMod) == STATUS_OK)
                            pStyle->set_string(vAtoms[P_MOD], &s);
                    }
                    if (vAtoms[P_KEY] >= 0)
                    {
                        if (format_key(&s, nKey) == STATUS_OK)
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Shortcut::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
                parse_value(&s);
            if ((property == vAtoms[P_MOD]) && (pStyle->get_string(vAtoms[P_MOD], &s) == STATUS_OK))
                parse_modifiers(&s);
            if ((property == vAtoms[P_KEY]) && (pStyle->get_string(vAtoms[P_KEY], &s) == STATUS_OK))
                parse_key(&s);

            if (pListener != NULL)
                pListener->notify(this);
        }

        ws::code_t Shortcut::set(ws::code_t key)
        {
            ws::code_t old = nKey;
            if (old == key)
                return old;

            nKey    = key;
            sync();
            return old;
        }

        void Shortcut::set(ws::code_t key, size_t mod)
        {
            mod    &= KM_ALL;
            if ((key == nKey) &&
                (mod == nMod))
                return;

            nKey    = key;
            nMod    = mod;
            sync();
        }

        size_t Shortcut::set_modifiers(size_t mod)
        {
            mod    &= KM_ALL;
            ws::code_t old = nMod;
            if (old == mod)
                return old;

            nMod    = mod;
            sync();
            return old;
        }

        void Shortcut::parse_value(const LSPString *s)
        {
        }

        void Shortcut::parse_key(const LSPString *s)
        {
        }

        void Shortcut::parse_modifiers(const LSPString *s)
        {
            size_t mod = 0;
            ssize_t n = Property::parse_bit_enums(&mod, s, MODLIST);
            if (n >= 0)
                nMod    = mod;
        }

        status_t Shortcut::append_modifier(LSPString *s, size_t mod, size_t index)
        {
            mod >>= (index << 1);
            const mod_desc_t *desc = &MOD_DESC[index];
            switch (mod & 3)
            {
                case 1: return (s->append_ascii(desc->left))  ? STATUS_OK : STATUS_NO_MEM;
                case 2: return (s->append_ascii(desc->right)) ? STATUS_OK : STATUS_NO_MEM;
                case 3: return (s->append_ascii(desc->both))  ? STATUS_OK : STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t Shortcut::format_value(LSPString *s, ws::code_t key, size_t mod)
        {
            LSPString tmp;
            status_t res = STATUS_OK;

            for (size_t i=0; i<6; ++i)
            {
                size_t len = tmp.length();
                if ((res = append_modifier(&tmp, mod, i)) != STATUS_OK)
                    return res;
                if (len == tmp.length())
                    continue;
                if (!tmp.append('+'))
                    return STATUS_NO_MEM;
            }

            if ((res = append_key(&tmp, key)) != STATUS_OK)
                return res;

            tmp.swap(s);
            return STATUS_OK;
        }

        status_t Shortcut::append_key(LSPString *s, ws::code_t key)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Shortcut::format_modifiers(LSPString *s, size_t mod)
        {
            // TODO
            return STATUS_OK;
        }

        status_t Shortcut::format_key(LSPString *s, ws::code_t key)
        {
            // TODO
            return STATUS_OK;
        }


    } /* namespace tk */
} /* namespace lsp */


