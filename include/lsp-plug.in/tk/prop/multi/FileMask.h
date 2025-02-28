/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_MULTI_FILEMASK_H_
#define LSP_PLUG_IN_TK_PROP_MULTI_FILEMASK_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * File mask property that holds font parameters
         */
        class FileMask: public Property
        {
            protected:
                class PListener: public prop::Listener
                {
                    protected:
                        ssize_t     nLocks;
                        FileMask   *pMask;

                    public:
                        explicit PListener(FileMask *prop);

                    public:
                        virtual void    notify(Property *prop) override;
                        inline ssize_t  lock()      { return ++nLocks; }
                        inline ssize_t  unlock()    { return --nLocks; }
                };

            protected:
                PListener           sPListener;
                prop::String        sTitle;
                prop::String        sExtensions;
                prop::PathPattern   sPattern;

            protected:
                status_t                    bind(atom_t property, Style *style, i18n::IDictionary *dict);
                status_t                    bind(const char *property, Style *style, i18n::IDictionary *dict);
                status_t                    bind(const LSPString *property, Style *style, i18n::IDictionary *dict);
                status_t                    unbind();

            protected:
                explicit FileMask(prop::Listener *listener = NULL);
                FileMask(const FileMask &) = delete;
                FileMask(FileMask &&) = delete;
                virtual ~FileMask() override;

                FileMask & operator = (const FileMask &) = delete;
                FileMask & operator = (FileMask &&) = delete;

            public:
                LSP_TK_PROPERTY(String,             title,                  &sTitle);
                LSP_TK_PROPERTY(String,             extensions,             &sExtensions);
                LSP_TK_PROPERTY(PathPattern,        pattern,                &sPattern);

            public:
                inline bool                 test(const LSPString *path) const       { return sPattern.test(path);       }
                inline bool                 test(const io::Path *path) const        { return sPattern.test(path);       }
                inline bool                 test(const char *path) const            { return sPattern.test(path);       }

                status_t                    append_extension(LSPString *str);
        };

        namespace prop
        {
            class FileMask: public tk::FileMask
            {
                public:
                    explicit FileMask(prop::Listener *listener = NULL): tk::FileMask(listener) {}
                    FileMask(const FileMask &) = delete;
                    FileMask(FileMask &&) = delete;
                    FileMask & operator = (const FileMask &) = delete;
                    FileMask & operator = (FileMask &&) = delete;

                public:
                    /**
                     * Bind property with specified name to the style of linked widget
                     */
                    inline status_t     bind(atom_t id, Style *style, i18n::IDictionary *dict)              { return tk::FileMask::bind(id, style, dict); }
                    inline status_t     bind(const LSPString *id, Style *style, i18n::IDictionary *dict)    { return tk::FileMask::bind(id, style, dict); }
                    inline status_t     bind(const char *id, Style *style, i18n::IDictionary *dict)         { return tk::FileMask::bind(id, style, dict); }
                    inline status_t     bind(Style *style, i18n::IDictionary *dict)                         { return tk::FileMask::bind(LSP_TK_PROP_LANGUAGE, style, dict); }

                    /**
                     * Unbind property
                     */
                    inline status_t     unbind()                            { return tk::FileMask::unbind();            }

                    inline void         listener(prop::Listener *listener)  { pListener = listener;                     }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_MULTI_FILEMASK_H_ */
