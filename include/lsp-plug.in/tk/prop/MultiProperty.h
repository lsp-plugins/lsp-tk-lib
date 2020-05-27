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
                MultiProperty & operator = (const MultiProperty &);

            protected:
                status_t        unbind(atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(atom_t id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(const char *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);
                status_t        bind(const LSPString *id, Style *style, atom_t *atoms, const prop::desc_t *desc, IStyleListener *listener);

                void            set_default(atom_t *atoms, const prop::desc_t *desc);

            protected:
                inline MultiProperty(prop::Listener *listener = NULL): Property(listener) {};
        };

    }
}

#endif /* LSP_PLUG_IN_TK_PROP_MULTIPROPERTY_H_ */
