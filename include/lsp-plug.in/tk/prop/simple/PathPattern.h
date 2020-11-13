/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_SIMPLE_PATHPATTERN_H_
#define LSP_PLUG_IN_TK_PROP_SIMPLE_PATHPATTERN_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/io/PathPattern.h>

namespace lsp
{
    namespace tk
    {
        /**
         * File pattern: specifies the file pattern to match file names
         */
        class PathPattern: public Property
        {
            private:
                PathPattern & operator = (const PathPattern &);

            protected:
                io::PathPattern                 sPattern;

            protected:
                explicit PathPattern(prop::Listener *listener = NULL);
                virtual ~PathPattern();

            public:
                const io::PathPattern          *pattern() const     { return &sPattern;             }
                const LSPString                *get() const         { return sPattern.get();        }
                const char                     *get_utf8() const    { return sPattern.get_utf8();   }
                inline size_t                   flags() const       { return sPattern.flags();      }

                inline bool                     test(const char *path) const        { return sPattern.test(path);       }
                inline bool                     test(const LSPString *path) const   { return sPattern.test(path);       }
                inline bool                     test(const io::Path *path) const    { return sPattern.test(path);       }

                status_t                        set(const io::PathPattern *pattern, size_t flags = 0);
                status_t                        set(const LSPString *pattern, size_t flags = 0);
                status_t                        set(const io::Path *pattern, size_t flags = 0);
                status_t                        set(const char *pattern, size_t flags = 0);

                status_t                        set_pattern(const io::PathPattern *pattern);
                status_t                        set_pattern(const LSPString *pattern);
                status_t                        set_pattern(const io::Path *pattern);
                status_t                        set_pattern(const char *pattern);

                size_t                          set_flags(size_t flags);

                void                            swap(PathPattern *src);
                inline void                     swap(PathPattern &src)              { swap(&src);                       }
        };

        namespace prop
        {
            class PathPattern: public tk::PathPattern
            {
                private:
                    PathPattern & operator = (const PathPattern &);

                public:
                    explicit PathPattern(prop::Listener *listener = NULL): tk::PathPattern(listener) {};
            };
        }
    }
}

#endif /* LSP_PLUG_IN_TK_PROP_SIMPLE_PATHPATTERN_H_ */
