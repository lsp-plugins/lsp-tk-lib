/*
 * MultiProperty.h
 *
 *  Created on: 8 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Style;
        class Display;

        /**
         * Floating-point property interface
         */
        class MultiProperty: public Property
        {
            private:
                Property & operator = (const Property *);

            protected:
                status_t        unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(
                        atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener,
                        const char *property, Style *style, Atoms *xatoms
                );

                void            set_default(atom_t *atoms, const prop::desc_t *desc);

            protected:
                inline MultiProperty(prop::Listener *listener = NULL): Property(listener) {};
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_ */
