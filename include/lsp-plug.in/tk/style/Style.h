/*
 * Style.h
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_STYLE_STYLE_H_
#define LSP_PLUG_IN_TK_STYLE_STYLE_H_

#include <lsp-plug.in/tk/version.h>
#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/tk/prop/types.h>
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
                    F_CREATED           = 1 << 0,   // Property has been explicitly created by client
                    F_OVERRIDDEN        = 1 << 1,   // Property has been locally overridden by client
                    F_NTF_LISTENERS     = 1 << 2,   // Property requires notification of listeners
                    F_NTF_CHILDREN      = 1 << 3,   // Property requires notification of children
                };

                typedef struct property_t
                {
                    atom_t              id;         // Unique identifier of property
                    ssize_t             type;       // Type of property
                    ssize_t             refs;       // Number of references
                    size_t              changes;    // Number of changes
                    size_t              flags;      // Flags
                    Style              *owner;      // Style that is owning a property

                    union
                    {
                        ssize_t             iValue;
                        float               fValue;
                        bool                bValue;
                        char               *sValue;
                    } v;                            // Actual property value

                    union
                    {
                        ssize_t             iValue;
                        float               fValue;
                        bool                bValue;
                        char               *sValue;
                    } dv;                           // Property local default value
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
                property_t         *create_property(atom_t id, const property_t *src, size_t flags);
                property_t         *create_property(atom_t id, property_type_t type, size_t flags);
                status_t            set_property_default(property_t *dst);
                status_t            copy_property(property_t *dst, const property_t *src);
                status_t            create_local_property(atom_t id, const property_t *src);

                inline const property_t   *get_property(atom_t id) const { return const_cast<Style *>(this)->get_property(id); };
                inline const property_t   *get_property_recursive(atom_t id) const { return const_cast<Style *>(this)->get_property_recursive(id); };

                void                sync();
                void                notify_change(property_t *prop);
                void                notify_children(property_t *prop);
                void                notify_listeners(property_t *prop);
                void                deref_property(property_t *prop);

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

                /**
                 * Bind listener to integer property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t     bind_int(atom_t id, IStyleListener *listener)        { return bind(id, PT_INT, listener); };

                /**
                 * Bind listener to floating-point property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t     bind_float(atom_t id, IStyleListener *listener)      { return bind(id, PT_FLOAT, listener); };

                /**
                 * Bind listener to boolean property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t     bind_bool(atom_t id, IStyleListener *listener)       { return bind(id, PT_BOOL, listener); };

                /**
                 * Bind listener to string property
                 * @param id property identifier
                 * @return status of operation
                 */
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

                /**
                 * Get integer property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t            get_int(atom_t id, ssize_t *dst) const;

                /**
                 * Get floating-point property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t            get_float(atom_t id, float *dst) const;

                /**
                 * Get boolean property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t            get_bool(atom_t id, bool *dst) const;

                /**
                 * Get string property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t            get_string(atom_t id, LSPString *dst) const;

                /**
                 * Get string property
                 * @param id property identifier
                 * @param dst pointer to store UTF-8 encoded result
                 * @return status of operation
                 */
                status_t            get_string(atom_t id, const char **dst) const;

                /**
                 * Check whether property exists in the whole style tree
                 * @param id property identifier
                 * @return true if this style or parent styles contain such property
                 */
                bool                exists(atom_t id) const;

                /**
                 * Check whether property is local for this style (explicitly created)
                 * @param id property identifier
                 * @return true if property has been explicitly created
                 */
                bool                is_local(atom_t id) const;

                /**
                 * Check whether property has default value or derives actual value
                 * from parent style
                 * @param id property identifier
                 * @return true if property has default value
                 */
                bool                is_default(atom_t id) const;

                /**
                 * Check whether property has been locally overridden
                 * @param id property identifier
                 * @return true if property has been locally overridden
                 */
                bool                is_overridden(atom_t id) const;

                /**
                 * Get property type for the whole style tree
                 * @param id property identifier
                 * @return actual property type or PT_UNKNOWN if property has not been found
                 */
                property_type_t     get_type(atom_t id) const;

                /**
                 * Assign value to integer property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t            set_int(atom_t id, ssize_t value);

                /**
                 * Assign value to floating-point property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t            set_float(atom_t id, float value);

                /**
                 * Assign value to boolean property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t            set_bool(atom_t id, bool value);

                /**
                 * Assign value to string property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t            set_string(atom_t id, const LSPString *value);

                /**
                 * Assign value to string property
                 * @param id property identifier
                 * @param value the UTF-8 encoded value to assign
                 * @return status of operation
                 */
                status_t            set_string(atom_t id, const char *value);

                /**
                 * Reset property to it's default value.
                 * If property is overridden by parent, it's value is taken.
                 * If property has no overrides, default value passed on create()
                 * is taken. Otherwise 0/0.0f/false/empty string will be assigned.
                 *
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t            set_default(atom_t id);

                /**
                 * Create local integer property
                 * @param id property identifier
                 * @param value default value
                 * @return status of operation
                 */
                status_t            create_int(atom_t id, ssize_t value);

                /**
                 * Create local floating-point property
                 * @param id property identifier
                 * @param value default value
                 * @return status of operation
                 */
                status_t            create_float(atom_t id, float value);

                /**
                 * Create local boolean property
                 * @param id property identifier
                 * @param value default value
                 * @return status of operation
                 */
                status_t            create_bool(atom_t id, bool value);

                /**
                 * Create local string property
                 * @param id property identifier
                 * @param value default value
                 * @return status of operation
                 */
                status_t            create_string(atom_t id, const LSPString *value);

                /**
                 * Create local string property
                 * @param id property identifier
                 * @param value default value (UTF-8 encoded)
                 * @return status of operation
                 */
                status_t            create_string(atom_t id, const char *value);

                /**
                 * Remove locally-created property
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t            remove(atom_t id);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_STYLE_H_ */
