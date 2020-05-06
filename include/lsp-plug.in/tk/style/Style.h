/*
 * Style.h
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_STYLE_H_
#define LSP_PLUG_IN_TK_STYLE_STYLE_H_

#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/style/IStyleListener.h>
#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace tk
    {
        /**
         * Some widget style. Allows nesting
         */
        class Style
        {
            protected:
                enum flags_t
                {
                    F_DEFAULT           = 1 << 0,
                    F_NTF_LISTENERS     = 1 << 1,
                    F_NTF_CHILDREN      = 1 << 2
                };

                typedef struct property_t
                {
                    atom_t              id;         // Unique identifier of property
                    ssize_t             type;       // Type of property
                    size_t              refs;       // Number of references
                    size_t              changes;    // Number of changes
                    size_t              flags;      // Flags
                    Style              *owner;      // Style that is owning a property
                    union
                    {
                        ssize_t     iValue;
                        float       fValue;
                        bool        bValue;
                        char       *sValue;
                    } v;
                } property_t;

                typedef struct listener_t
                {
                    atom_t           nId;        // Property identifier
                    IStyleListener     *pListener;  // Listener
                } listener_t;

            private:
                lltl::parray<Style>         vParents;
                lltl::parray<Style>         vChildren;
                lltl::darray<property_t>    vProperties;
                lltl::darray<listener_t>    vListeners;
                ssize_t                     nLock;
                bool                        bDelayed;

            public:
                explicit Style();
                virtual ~Style();

                status_t            init();
                void                destroy();

            protected:
                void                undef_property(property_t *property);
                void                do_destroy();
                void                delayed_notify();
                property_t         *get_property_recursive(atom_t id);
                property_t         *get_parent_property(atom_t id);
                property_t         *get_property(atom_t id);
                status_t            set_property(atom_t id, property_t *src);
                status_t            sync_property(property_t *p);
                property_t         *create_property(atom_t id, const property_t *src);
                property_t         *create_property(atom_t id, property_type_t type);
                status_t            set_property_default(property_t *dst);
                status_t            copy_property(property_t *dst, const property_t *src);

                inline const property_t   *get_property(atom_t id) const { return const_cast<Style *>(this)->get_property(id); };
                inline const property_t   *get_property_recursive(atom_t id) const { return const_cast<Style *>(this)->get_property_recursive(id); };

                void                sync();
                void                notify_change(property_t *prop);
                void                notify_children(property_t *prop);
                void                notify_listeners(property_t *prop);

            public:
                /**
                 * Get number of parent styles
                 * @return number of parent styles
                 */
                inline size_t       parents() const     { return vParents.size();   }

                /**
                 * Get parent style
                 * @param idx sequential number of parent style starting with 0
                 * @return parent style or NULL if does not exist
                 */
                inline Style       *parent(size_t idx)  { return vParents.get(idx); };

                /**
                 * Set parent style
                 * @param parent parent style
                 * @param idx parent index, negative value means last
                 * @return status of operation
                 */
                status_t            add_parent(Style *parent, ssize_t idx = -1);

                /**
                 * Remove the parent style
                 * @param parent parent style to remove
                 * @return status of operation
                 */
                status_t            remove_parent(Style *parent);

                /**
                 * Check whether style has a parent
                 * @param parent parent style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a parent
                 */
                bool                has_parent(Style *parent, bool recursive = false);

                /**
                 * Get number of child styles
                 * @return number of child styles
                 */
                inline size_t       children() const    { return vChildren.size();   }

                /**
                 * Get child style
                 * @param idx sequential number of child style starting with 0
                 * @return child style or NULL if does not exist
                 */
                inline Style       *child(size_t idx)  { return vChildren.get(idx); };

                /**
                 * Add child style
                 * @param child child style
                 * @param idx child index, negative value means last
                 * @return status of operation
                 */
                status_t            add_child(Style *child, ssize_t idx = -1);

                /** Remove child style
                 *
                 * @param child child style to remove
                 * @return status of operation
                 */
                status_t            remove_child(Style *child);

                /**
                 * Check whether style has a child
                 * @param child child style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a child
                 */
                bool                has_child(Style *child, bool recursive = false);

            public:
                /**
                 * Bind listener to property
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t            bind(atom_t id, property_type_t type, IStyleListener *listener);

                inline status_t     bind_int(atom_t id, IStyleListener *listener)        { return bind(id, PT_INT, listener); };
                inline status_t     bind_float(atom_t id, IStyleListener *listener)      { return bind(id, PT_FLOAT, listener); };
                inline status_t     bind_bool(atom_t id, IStyleListener *listener)       { return bind(id, PT_BOOL, listener); };
                inline status_t     bind_string(atom_t id, IStyleListener *listener)     { return bind(id, PT_STRING, listener); };

                /**
                 * Check that listener is already bound to the property
                 * @param id property identifier
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                is_bound(atom_t id, IStyleListener *listener) const;

                /**
                 * Unbind listener from a property
                 * @param id property identifier
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t            unbind(atom_t id, IStyleListener *listener);

            public:
                /**
                 * Return overall number of properties
                 * @return overall number of properties
                 */
                inline size_t       properties() const  { return vProperties.size(); }

                /**
                 * Return overall number of listeners
                 * @return overall number of listeners
                 */
                inline size_t       listeners() const   { return vListeners.size(); }

            public:
                /**
                 * Start transactional update of properties.
                 * All listeners and children will be notified
                 * only after transaction becomes completed
                 */
                void                begin();

                /**
                 * End transactional update of properties.
                 * All listeners and children will be notified
                 */
                void                end();

                status_t            get_int(atom_t id, ssize_t *dst) const;
                status_t            get_float(atom_t id, float *dst) const;
                status_t            get_bool(atom_t id, bool *dst) const;
                status_t            get_string(atom_t id, LSPString *dst) const;
                status_t            get_string(atom_t id, const char **dst) const;
                bool                exists(atom_t id) const;
                bool                is_default(atom_t id) const;
                ssize_t             get_type(atom_t id) const;

                status_t            set_int(atom_t id, ssize_t value);
                status_t            set_float(atom_t id, float value);
                status_t            set_bool(atom_t id, bool value);
                status_t            set_string(atom_t id, const LSPString *value);
                status_t            set_string(atom_t id, const char *value);
                status_t            set_default(atom_t id);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_STYLE_H_ */
