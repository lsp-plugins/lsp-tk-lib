/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 17 мая 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_FILEDIALOGMODE_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_FILEDIALOGMODE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class FileDialogMode: public Enum
        {
            private:
                FileDialogMode & operator = (const FileDialogMode &);

            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit FileDialogMode(prop::Listener *listener = NULL): Enum(ENUM, listener) {};

            public:
                inline file_dialog_mode_t   get() const                 { return file_dialog_mode_t(nValue);            }
                inline file_dialog_mode_t   set(file_dialog_mode_t v)   { return file_dialog_mode_t(Enum::set(v));      }

                inline bool                 open_file() const           { return nValue == FDM_OPEN_FILE;               }
                inline bool                 save_file() const           { return nValue == FDM_SAVE_FILE;               }

                inline file_dialog_mode_t   set_open_file()             { return set(FDM_OPEN_FILE);                    }
                inline file_dialog_mode_t   set_save_file()             { return set(FDM_SAVE_FILE);                    }

        };
    
        namespace prop
        {
            class FileDialogMode: public tk::FileDialogMode
            {
                private:
                    FileDialogMode & operator = (const FileDialogMode &);

                public:
                    explicit FileDialogMode(prop::Listener *listener = NULL): tk::FileDialogMode(listener) {};

                protected:
                    using tk::Enum::init;
                    using tk::Enum::override;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t property, Style *style)             { return tk::FileDialogMode::bind(property, style); }
                    inline status_t     bind(const char *property, Style *style)        { return tk::FileDialogMode::bind(property, style); }
                    inline status_t     bind(const LSPString *property, Style *style)   { return tk::FileDialogMode::bind(property, style); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                                        { return tk::FileDialogMode::unbind(); };

                    /**
                     * Initialize default value
                     * @param style style
                     * @param o default orientation
                     * @return status of operation
                     */
                    inline status_t     init(Style *style, file_dialog_mode_t o)        { return tk::FileDialogMode::init(style, o);      };

                    static status_t     init(const char *name, Style *style, file_dialog_mode_t value);
                    static status_t     override(const char *name, Style *style, file_dialog_mode_t value);

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };
        }
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_FILEDIALOGMODE_H_ */
