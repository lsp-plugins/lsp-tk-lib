/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            protected:
                static const prop::enum_t ENUM[];

            protected:
                explicit FileDialogMode(prop::Listener *listener = NULL): Enum(ENUM, FDM_OPEN_FILE, listener) {};
                FileDialogMode(const FileDialogMode &) = delete;
                FileDialogMode(FileDialogMode &&) = delete;
                FileDialogMode & operator = (const FileDialogMode &) = delete;
                FileDialogMode & operator = (FileDialogMode &&) = delete;

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
                public:
                    explicit FileDialogMode(prop::Listener *listener = NULL): tk::FileDialogMode(listener) {};
                    FileDialogMode(const FileDialogMode &) = delete;
                    FileDialogMode(FileDialogMode &&) = delete;
                    FileDialogMode & operator = (const FileDialogMode &) = delete;
                    FileDialogMode & operator = (FileDialogMode &&) = delete;

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

                    inline void         listener(prop::Listener *listener)              { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_ENUM_FILEDIALOGMODE_H_ */
