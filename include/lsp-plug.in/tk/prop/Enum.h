/*
 * Enum.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_ENUM_H_
#define LSP_PLUG_IN_TK_PROP_ENUM_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>

#include <lsp-plug.in/tk/style.h>
#include <lsp-plug.in/tk/prop.h>

namespace lsp
{
    namespace tk
    {
        class Display;
        class Widget;

        /**
         * Integering property interface
         */
        class Enum: public Property
        {
            private:
                Enum & operator = (const Enum *);

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
                atom_t              nAtom;
                ssize_t             nValue;
                Listener            sListener;
                const prop::enum_t *pEnum;

            protected:
                status_t            unbind();
                status_t            bind(atom_t property, Style *style);
                void                commit();
                ssize_t             set(ssize_t v);

            protected:
                explicit Enum(const prop::enum_t *xenum, prop::Listener *listener = NULL);
                ~Enum();

        };
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_ENUM_H_ */
