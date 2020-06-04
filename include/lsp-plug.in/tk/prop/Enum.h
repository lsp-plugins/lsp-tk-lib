/*
 * Enum.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Integering property interface
         */
        class Enum: public SimpleProperty
        {
            private:
                Enum & operator = (const Enum &);

            protected:
                class Listener: public IStyleListener
                {
                    protected:
                        Enum       *pValue;

                    public:
                        inline Listener(Enum *ptr)             { pValue = ptr;     }

                    public:
                        virtual void notify(atom_t property);
                };

            protected:
                ssize_t             nValue;
                Listener            sListener;
                const prop::enum_t *pEnum;

            protected:
                status_t            unbind();
                status_t            bind(atom_t property, Style *style);
                status_t            bind(const char *property, Style *style);
                status_t            bind(const LSPString *property, Style *style);
                void                commit();
                ssize_t             set(ssize_t v);
                ssize_t             init(Style *style, ssize_t v);
                ssize_t             override(Style *style, ssize_t v);

            protected:
                explicit Enum(const prop::enum_t *xenum, prop::Listener *listener = NULL);
                ~Enum();

        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_H_ */
