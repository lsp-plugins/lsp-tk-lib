/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 4 нояб. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_ISTYLEFACTORY_H_
#define LSP_PLUG_IN_TK_STYLE_ISTYLEFACTORY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif


namespace lsp
{
    namespace tk
    {
        class Schema;
        class Style;

        /**
         * Style factory, allows to create and initialize the style
         */
        class IStyleFactory
        {
            private:
                IStyleFactory & operator = (const IStyleFactory &);

            protected:
                const char         *sName;

            protected:
                static Style       *init(Style *s);

            public:
                explicit            IStyleFactory(const char *name);
                virtual            ~IStyleFactory();

            public:
                /**
                 * Return the name of produced style
                 * @return name of produced style
                 */
                inline const char  *name() const        { return sName;     }

            public:
                /** Create and initialize style
                 *
                 * @return created and initialized style
                 */
                virtual Style      *create(Schema *schema);
        };

        /**
         * Generalized factory
         * @tparam IStyle Style class to instantiate
         */
        template <class IStyle>
            class StyleFactory: public IStyleFactory
            {
                private:
                    StyleFactory & operator = (const StyleFactory &);

                public:
                    explicit StyleFactory(const char *name) : IStyleFactory(name) {}

                public:
                    virtual Style      *create(Schema *schema) { return init(new IStyle(schema));   }
            };
    }
}

#endif /* LSP_PLUG_IN_TK_STYLE_ISTYLEFACTORY_H_ */
