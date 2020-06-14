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

        typedef struct key_desc_t
        {
            ws::code_t code;
            const char *desc;
        } key_desc_t;

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

        static const key_desc_t KEY_DESC[] =
        {
            { ws::WSK_BACKSPACE, "Backspace" },
            { ws::WSK_TAB, "Tab" },
            { ws::WSK_LINEFEED, "LF" },
            { ws::WSK_CLEAR, "Clear" },
            { ws::WSK_RETURN, "Enter" },
            { ws::WSK_PAUSE, "Pause" },
            { ws::WSK_SCROLL_LOCK, "ScrollLock" },
            { ws::WSK_SYS_REQ, "SysReq" },
            { ws::WSK_ESCAPE, "Escape" },
            { ws::WSK_DELETE, "Delete" },
            { ws::WSK_HOME, "Home" },
            { ws::WSK_END, "End" },
            { ws::WSK_LEFT, "Left" },
            { ws::WSK_RIGHT, "Right" },
            { ws::WSK_UP, "Up" },
            { ws::WSK_DOWN, "Down" },
            { ws::WSK_PAGE_UP, "PageUp" },
            { ws::WSK_PAGE_DOWN, "PageDown" },
            { ws::WSK_BEGIN, "Begin" },
            { ws::WSK_SELECT, "Select" },
            { ws::WSK_PRINT, "Print" },
            { ws::WSK_EXECUTE, "Execute" },
            { ws::WSK_INSERT, "Insert" },
            { ws::WSK_UNDO, "Undo" },
            { ws::WSK_REDO, "Redo" },
            { ws::WSK_MENU, "Menu" },
            { ws::WSK_FIND, "Find" },
            { ws::WSK_CANCEL, "Cancel" },
            { ws::WSK_HELP, "Help" },
            { ws::WSK_BREAK, "Break" },
            { ws::WSK_MODE_SWITCH, "ModeSwitch" },
            { ws::WSK_NUM_LOCK, "NumLock" },
            { ws::WSK_KEYPAD_SPACE, "Numpad Space" },
            { ws::WSK_KEYPAD_TAB, "Numpad Tab" },
            { ws::WSK_KEYPAD_ENTER, "Numpad Enter" },
            { ws::WSK_KEYPAD_F1, "Numpad F1" },
            { ws::WSK_KEYPAD_F2, "Numpad F2" },
            { ws::WSK_KEYPAD_F3, "Numpad F3" },
            { ws::WSK_KEYPAD_F4, "Numpad F4" },
            { ws::WSK_KEYPAD_HOME, "Numpad Home" },
            { ws::WSK_KEYPAD_LEFT, "Numpad Left" },
            { ws::WSK_KEYPAD_UP, "Numpad Up" },
            { ws::WSK_KEYPAD_RIGHT, "Numpad Right" },
            { ws::WSK_KEYPAD_DOWN, "Numpad Down" },
            { ws::WSK_KEYPAD_PAGE_UP, "Numpad PageUp" },
            { ws::WSK_KEYPAD_PAGE_DOWN, "Numpad PageDown" },
            { ws::WSK_KEYPAD_END, "Numpad End" },
            { ws::WSK_KEYPAD_BEGIN, "Numpad Begin" },
            { ws::WSK_KEYPAD_INSERT, "Numpad Insert" },
            { ws::WSK_KEYPAD_DELETE, "Numpad Delete" },
            { ws::WSK_KEYPAD_EQUAL, "Numpad =" },
            { ws::WSK_KEYPAD_MULTIPLY, "Numpad *" },
            { ws::WSK_KEYPAD_ADD, "Numpad +" },
            { ws::WSK_KEYPAD_SEPARATOR, "Numpad Separator" },
            { ws::WSK_KEYPAD_SUBTRACT, "Numpad -" },
            { ws::WSK_KEYPAD_DECIMAL, "Numpad ." },
            { ws::WSK_KEYPAD_DIVIDE, "Numpad /" },
            { ws::WSK_KEYPAD_0, "Numpad 0" },
            { ws::WSK_KEYPAD_1, "Numpad 1" },
            { ws::WSK_KEYPAD_2, "Numpad 2" },
            { ws::WSK_KEYPAD_3, "Numpad 3" },
            { ws::WSK_KEYPAD_4, "Numpad 4" },
            { ws::WSK_KEYPAD_5, "Numpad 5" },
            { ws::WSK_KEYPAD_6, "Numpad 6" },
            { ws::WSK_KEYPAD_7, "Numpad 7" },
            { ws::WSK_KEYPAD_8, "Numpad 8" },
            { ws::WSK_KEYPAD_9, "Numpad 9" },
            { ws::WSK_F1, "F1" },
            { ws::WSK_F2, "F2" },
            { ws::WSK_F3, "F3" },
            { ws::WSK_F4, "F4" },
            { ws::WSK_F5, "F5" },
            { ws::WSK_F6, "F6" },
            { ws::WSK_F7, "F7" },
            { ws::WSK_F8, "F8" },
            { ws::WSK_F9, "F9" },
            { ws::WSK_F10, "F10" },
            { ws::WSK_F11, "F11" },
            { ws::WSK_F12, "F12" },
            { ws::WSK_F13, "F13" },
            { ws::WSK_F14, "F14" },
            { ws::WSK_F15, "F15" },
            { ws::WSK_F16, "F16" },
            { ws::WSK_F17, "F17" },
            { ws::WSK_F18, "F18" },
            { ws::WSK_F19, "F19" },
            { ws::WSK_F20, "F20" },
            { ws::WSK_F21, "F21" },
            { ws::WSK_F22, "F22" },
            { ws::WSK_F23, "F23" },
            { ws::WSK_F24, "F24" },
            { ws::WSK_F25, "F25" },
            { ws::WSK_F26, "F26" },
            { ws::WSK_F27, "F27" },
            { ws::WSK_F28, "F28" },
            { ws::WSK_F29, "F29" },
            { ws::WSK_F30, "F30" },
            { ws::WSK_F31, "F31" },
            { ws::WSK_F32, "F32" },
            { ws::WSK_F33, "F33" },
            { ws::WSK_F34, "F34" },
            { ws::WSK_F35, "F35" },

            { ws::WSK_SHIFT_L, NULL },
            { ws::WSK_SHIFT_R, NULL },
            { ws::WSK_CONTROL_L, NULL },
            { ws::WSK_CONTROL_R, NULL },
            { ws::WSK_CAPS_LOCK, NULL },
            { ws::WSK_SHIFT_LOCK, NULL },

            { ws::WSK_META_L, NULL },
            { ws::WSK_META_R, NULL },
            { ws::WSK_ALT_L, NULL },
            { ws::WSK_ALT_R, NULL },
            { ws::WSK_SUPER_L, NULL },
            { ws::WSK_SUPER_R, NULL },
            { ws::WSK_HYPER_L, NULL },
            { ws::WSK_HYPER_R, NULL },

            { ws::WSK_UNKNOWN, NULL }
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
                nMod = parse_modifiers(&s);
            if ((property == vAtoms[P_KEY]) && (pStyle->get_string(vAtoms[P_KEY], &s) == STATUS_OK))
                nKey = parse_key(&s);

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
            LSPString tmp;
            ssize_t last = 0, split = -1;

            size_t mod, xmod;

            while ((split = s->index_of(last, '+')) > last)
            {
                if (!tmp.set(s, last, split))
                    return;
                if (!(xmod = parse_modifier(&tmp)))
                    break;

                mod    |= xmod;
                last    = split + 1;
            }

            if (!tmp.set(s, last, split))
                return;

            nKey    = parse_key(&tmp);
            nMod    = mod;
        }

        ws::code_t Shortcut::parse_key(const LSPString *s)
        {
            for (const key_desc_t *k=KEY_DESC; k->code != ws::WSK_UNKNOWN; ++k)
            {
                if (k->desc == NULL)
                    continue;

                if (s->equals_utf8_nocase(k->desc))
                    return k->code;
            }

            return (s->length() == 1) ? s->char_at(0) : ws::WSK_UNKNOWN;
        }

        size_t Shortcut::parse_modifier(const LSPString *s)
        {
            for (size_t i=0; i<(sizeof(MOD_DESC) / sizeof(mod_desc_t)); ++i)
            {
                const mod_desc_t *desc = &MOD_DESC[i];

                if (s->equals_ascii_nocase(desc->left))
                    return 1 << (i << 1);
                else if (s->equals_ascii_nocase(desc->right))
                    return 2 << (i << 1);
                else if (s->equals_ascii_nocase(desc->both))
                    return 3 << (i << 1);
            }
            return 0;
        }

        size_t Shortcut::parse_modifiers(const LSPString *s)
        {
            size_t mod = 0;
            Property::parse_bit_enums(&mod, s, MODLIST);
            return mod;
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

            for (size_t i=0; i<(sizeof(MOD_DESC) / sizeof(mod_desc_t)); ++i)
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
            if (key >= ws::WSK_FIRST)
            {
                for (const key_desc_t *k=KEY_DESC; k->code != ws::WSK_UNKNOWN; ++k)
                {
                    if (k->code == key)
                    {
                        if (k->desc == NULL)
                            return STATUS_OK;
                        return (s->append_utf8(k->desc)) ? STATUS_OK : STATUS_NO_MEM;
                    }
                }
            }

            return (s->append(lsp_wchar_t(key))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Shortcut::format_modifiers(LSPString *s, size_t mod)
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
                if (!tmp.append(','))
                    return STATUS_NO_MEM;
            }

            tmp.toupper();
            tmp.swap(s);
            return STATUS_OK;
        }

        status_t Shortcut::format_key(LSPString *s, ws::code_t key)
        {
            LSPString tmp;
            status_t res = append_key(&tmp, key);
            if (res == STATUS_OK)
                tmp.swap(s);
            return res;
        }


    } /* namespace tk */
} /* namespace lsp */


