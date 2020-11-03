/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 31 окт. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_STYLEINITIALIZER_H_
#define LSP_PLUG_IN_TK_STYLE_STYLEINITIALIZER_H_

namespace lsp
{
    namespace tk
    {
        class Style;

        /** Style factory class, allows to build all style propertied for
         * the concrete style
         */
        class StyleInitializer
        {
            protected:
                const char          *pClass;

            public:
                explicit StyleInitializer(const char *style);
                virtual ~StyleInitializer();

            public:
                inline const char  *name() const        { return pClass;    }
                virtual status_t    init(Style *style);
        };

        #define STYLE_INITIALIZER_DEF(widget, parent)   \
            class widget ## StyleInitializer: public parent ## StyleInitializer \
            { \
                private: \
                    widget ## StyleInitializer & operator = (const widget ## StyleInitializer &); \
                \
                public: \
                    explicit widget ## StyleInitializer(const char *style = NULL); \
                \
                public: \
                    virtual status_t    init(Style *style); \
            };

        #define STYLE_INITIALIZER_BEGIN(widget, parent)   \
            widget ## StyleInitializer::widget ## StyleInitializer(const char *style): \
                parent ## StyleInitializer((style != NULL) ? style : #widget) \
            { \
            } \
            \
            status_t widget ## StyleInitializer::init(Style *style) \
            { \
                status_t __res = parent ## StyleInitializer::init(style); \
                if (__res != STATUS_OK) \
                    return __res; \

        #define STYLE_INITIALIZER_CLASS(widget)         widget ## StyleInitializer
        #define STYLE_INITIALIZER_INSTANCE(widget)      widget ## StyleInitializerInstance

        #define STYLE_INITIALIZER_END(widget, name) \
                return STATUS_OK; \
            } \
            \
            static STYLE_INITIALIZER_CLASS(widget)   STYLE_INITIALIZER_INSTANCE(widget)(name);

    }
}

#endif /* LSP_PLUG_IN_TK_STYLE_STYLEINITIALIZER_H_ */
