/*
 * SimpleProperty.h
 *
 *  Created on: 14 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_SIMPLEPROPERTY_H_
#define LSP_PLUG_IN_TK_PROP_SIMPLEPROPERTY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class SimpleProperty: public Property
        {
            private:
                SimpleProperty & operator = (const SimpleProperty *);

            protected:
                atom_t              nAtom;

            protected:
                status_t            bind(atom_t property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            bind(const char *property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            bind(const LSPString *property, Style *style, property_type_t type, IStyleListener *listener);
                status_t            unbind(IStyleListener *listener);

            protected:
                inline SimpleProperty(prop::Listener *listener = NULL): Property(listener) { nAtom = -1; };
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_SIMPLEPROPERTY_H_ */
