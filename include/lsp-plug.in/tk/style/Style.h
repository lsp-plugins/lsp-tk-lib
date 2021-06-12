/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 окт. 2019 г.
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

#ifndef LSP_PLUG_IN_TK_STYLE_STYLE_H_
#define LSP_PLUG_IN_TK_STYLE_STYLE_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/runtime/LSPString.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/lltl/darray.h>

namespace lsp
{
    namespace tk
    {
        class Schema;
        class Property;

        /**
         * Some widget style. Allows nesting
         */
        class Style
        {
            private:
                Style & operator = (const Style &);
                Style(const Style &);

                friend class IStyleFactory;
                friend class Schema;
                friend class Property;

            protected:
                enum flags_t
                {
                    F_OVERRIDDEN        = 1 << 0,   // Property has been locally overridden by client
                    F_NTF_LISTENERS     = 1 << 1,   // Property requires notification of listeners
                    F_NTF_CHILDREN      = 1 << 2,   // Property requires notification of children
                };

                enum style_flags_t
                {
                    S_DELAYED           = 1 << 0,   // Delayed notification
                    S_OVERRIDE          = 1 << 1,   // Force overrides
                    S_CONFIGURED        = 1 << 2,   // The changes to style have been configured
                };

                typedef struct property_t
                {
                    atom_t              id;         // Unique identifier of property
                    property_type_t     type;       // Type of property
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
                    atom_t              nId;        // Property identifier
                    bool                bNotify;    // Delayed notify flag
                    IStyleListener     *pListener;  // Listener
                } listener_t;

                typedef struct client_t
                {
                    atom_t              nId;
                    Property           *pClient;
                } client_t;

            private:
                lltl::parray<Style>             vParents;
                lltl::parray<Style>             vChildren;
                lltl::darray<property_t>        vProperties;
                lltl::darray<listener_t>        vListeners;
                lltl::parray<IStyleListener>    vLocks;
                mutable Schema                 *pSchema;
                size_t                          nFlags;

            public:
                explicit Style(Schema *schema);
                virtual ~Style();

                virtual status_t    init();
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
                status_t            update_default_value(property_t *p, const property_t *src);

                inline const property_t   *get_property(atom_t id) const { return const_cast<Style *>(this)->get_property(id); };
                inline const property_t   *get_property_recursive(atom_t id) const { return const_cast<Style *>(this)->get_property_recursive(id); };

                void                synchronize();
                void                notify_change(property_t *prop);
                void                notify_children(property_t *prop);
                size_t              notify_children_delayed(property_t *prop);
                void                notify_listeners(property_t *prop);
                size_t              notify_listeners_delayed(property_t *prop);
                void                deref_property(property_t *prop);

                bool                set_configured(bool set);
                inline bool         configured() const      { return nFlags & S_CONFIGURED; }

            public:
                /** Set override mode for the style
                 *
                 * @param set overide mode (true/false)
                 * @return previous mode
                 */
                bool                    set_override(bool set);

                /**
                 * Get associated schema
                 * @return schema
                 */
                inline Schema          *schema()            { return pSchema;           }

                /**
                 * Get associated schema
                 * @return schema
                 */
                inline Schema          *schema() const      { return pSchema;           }

                /**
                 * Get number of parent styles
                 * @return number of parent styles
                 */
                inline size_t           parents() const     { return vParents.size();   }

                /**
                 * Get parent style
                 * @param idx sequential number of parent style starting with 0
                 * @return parent style or NULL if does not exist
                 */
                inline Style           *parent(size_t idx)  { return vParents.get(idx); }

                /**
                 * Check sync mode
                 * @return true if sync mode
                 */
                bool                    config_mode() const;

                /**
                 * Check sync mode
                 * @return true if sync mode
                 */
                bool                    override_mode() const;

                /**
                 * Set parent style
                 * @param parent parent style
                 * @param idx parent index, negative value means last
                 * @return status of operation
                 */
                status_t                add_parent(Style *parent, ssize_t idx = -1);

                /**
                 * Inject parent style as having most priority against others
                 * @param parent parent style to inject
                 * @return status of operation
                 */
                inline status_t         inject_parent(Style *parent)    { return add_parent(parent, -1);    }

                /**
                 * Remove the parent style
                 * @param parent parent style to remove
                 * @return status of operation
                 */
                status_t                remove_parent(Style *parent);

                /**
                 * Remove all parents
                 * @return status of operation
                 */
                status_t                remove_all_parents();

                /**
                 * Check whether style has a parent
                 * @param parent parent style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a parent
                 */
                bool                    has_parent(Style *parent, bool recursive = false);

                /**
                 * Get number of child styles
                 * @return number of child styles
                 */
                inline size_t           children() const    { return vChildren.size();   }

                /**
                 * Get child style
                 * @param idx sequential number of child style starting with 0
                 * @return child style or NULL if does not exist
                 */
                inline Style           *child(size_t idx)  { return vChildren.get(idx); };

                /**
                 * Add child style
                 * @param child child style
                 * @param idx child index, negative value means last
                 * @return status of operation
                 */
                status_t                add_child(Style *child, ssize_t idx = -1);

                /** Remove child style
                 *
                 * @param child child style to remove
                 * @return status of operation
                 */
                status_t                remove_child(Style *child);

                /**
                 * Remove all parents
                 * @return status of operation
                 */
                status_t                remove_all_children();

                /**
                 * Check whether style has a child
                 * @param child child style
                 * @param recursive flag that indicates that a recursive search should be performed
                 * @return true if style has a child
                 */
                bool                    has_child(Style *child, bool recursive = false);

            public:
                /**
                 * Bind listener to property
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t                bind(atom_t id, property_type_t type, IStyleListener *listener);
                status_t                bind(const char *id, property_type_t type, IStyleListener *listener);
                status_t                bind(const LSPString *id, property_type_t type, IStyleListener *listener);

                /**
                 * Bind listener to integer property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t         bind_int(atom_t id, IStyleListener *listener)               { return bind(id, PT_INT, listener);    }
                inline status_t         bind_int(const char *id, IStyleListener *listener)          { return bind(id, PT_INT, listener);    }
                inline status_t         bind_int(const LSPString *id, IStyleListener *listener)     { return bind(id, PT_INT, listener);    }

                /**
                 * Bind listener to floating-point property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t         bind_float(atom_t id, IStyleListener *listener)             { return bind(id, PT_FLOAT, listener);  }
                inline status_t         bind_float(const char *id, IStyleListener *listener)        { return bind(id, PT_FLOAT, listener);  }
                inline status_t         bind_float(const LSPString *id, IStyleListener *listener)   { return bind(id, PT_FLOAT, listener);  }

                /**
                 * Bind listener to boolean property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t         bind_bool(atom_t id, IStyleListener *listener)              { return bind(id, PT_BOOL, listener);   }
                inline status_t         bind_bool(const char *id, IStyleListener *listener)         { return bind(id, PT_BOOL, listener);   }
                inline status_t         bind_bool(const LSPString *id, IStyleListener *listener)    { return bind(id, PT_BOOL, listener);   }

                /**
                 * Bind listener to string property
                 * @param id property identifier
                 * @return status of operation
                 */
                inline status_t         bind_string(atom_t id, IStyleListener *listener)            { return bind(id, PT_STRING, listener); }
                inline status_t         bind_string(const char *id, IStyleListener *listener)       { return bind(id, PT_STRING, listener); }
                inline status_t         bind_string(const LSPString *id, IStyleListener *listener)  { return bind(id, PT_STRING, listener); }

                /**
                 * Check that listener is already bound to the property
                 * @param id property identifier
                 * @param listener listener
                 * @return true if listener is bound
                 */
                bool                    is_bound(atom_t id, IStyleListener *listener) const;
                bool                    is_bound(const char *id, IStyleListener *listener) const;
                bool                    is_bound(const LSPString *id, IStyleListener *listener) const;

                /**
                 * Unbind listener from a property
                 * @param id property identifier
                 * @param listener property listener
                 * @return status of operation
                 */
                status_t                unbind(atom_t id, IStyleListener *listener);
                status_t                unbind(const char *id, IStyleListener *listener);
                status_t                unbind(const LSPString *id, IStyleListener *listener);

            public:
                /**
                 * Return overall number of local properties
                 * @return overall number of local properties
                 */
                inline size_t           properties() const  { return vProperties.size(); }

                /**
                 * Return overall number of property listener bindings
                 * @return overall number of listener bindings
                 */
                inline size_t           listeners() const   { return vListeners.size(); }

            public:
                /**
                 * Start transactional update of properties.
                 * All listeners and children will be notified
                 * only after transaction becomes completed
                 * @return status of operation
                 */
                status_t                begin();

                /**
                 * Start transactional update of properties.
                 * All listeners and children will be notified
                 * only after transaction becomes completed
                 * @param listener the listener excluded from notification
                 * @return status of operation
                 */
                status_t                begin(IStyleListener *listener);

                /**
                 * End transactional update of properties.
                 * All listeners and children will be notified
                 * @return status of operation
                 */
                status_t                end();

                /**
                 * Get integer property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t                get_int(atom_t id, ssize_t *dst) const;
                status_t                get_int(const char *id, ssize_t *dst) const;
                status_t                get_int(const LSPString *id, ssize_t *dst) const;

                /**
                 * Get floating-point property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t                get_float(atom_t id, float *dst) const;
                status_t                get_float(const char *id, float *dst) const;
                status_t                get_float(const LSPString *id, float *dst) const;

                /**
                 * Get boolean property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t                get_bool(atom_t id, bool *dst) const;
                status_t                get_bool(const char *id, bool *dst) const;
                status_t                get_bool(const LSPString *id, bool *dst) const;

                /**
                 * Get string property
                 * @param id property identifier
                 * @param dst pointer to store result
                 * @return status of operation
                 */
                status_t                get_string(atom_t id, LSPString *dst) const;
                status_t                get_string(const char *id, LSPString *dst) const;
                status_t                get_string(const LSPString *id, LSPString *dst) const;

                /**
                 * Get string property
                 * @param id property identifier
                 * @param dst pointer to store UTF-8 encoded result
                 * @return status of operation
                 */
                status_t                get_string(atom_t id, const char **dst) const;
                status_t                get_string(const char *id, const char **dst) const;
                status_t                get_string(const LSPString *id, const char **dst) const;

                /**
                 * Check whether property exists in the whole style tree
                 * @param id property identifier
                 * @return true if this style or parent styles contain such property
                 */
                bool                    exists(atom_t id) const;
                bool                    exists(const char *id) const;
                bool                    exists(const LSPString *id) const;

                /**
                 * Check whether property has been locally overridden
                 * @param id property identifier
                 * @return true if property has been locally overridden
                 */
                bool                    is_overridden(atom_t id) const;
                bool                    is_overridden(const char *id) const;
                bool                    is_overridden(const LSPString *id) const;

                /**
                 * Get property type for the whole style tree
                 * @param id property identifier
                 * @return actual property type or PT_UNKNOWN if property has not been found
                 */
                property_type_t         get_type(atom_t id) const;
                property_type_t         get_type(const char *id) const;
                property_type_t         get_type(const LSPString *id) const;

                /**
                 * Assign value to integer property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t                set_int(atom_t id, ssize_t value);
                status_t                set_int(const char *id, ssize_t value);
                status_t                set_int(const LSPString *id, ssize_t value);

                /**
                 * Assign value to floating-point property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t                set_float(atom_t id, float value);
                status_t                set_float(const char *id, float value);
                status_t                set_float(const LSPString *id, float value);

                /**
                 * Assign value to boolean property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t                set_bool(atom_t id, bool value);
                status_t                set_bool(const char *id, bool value);
                status_t                set_bool(const LSPString *id, bool value);

                /**
                 * Assign value to string property
                 * @param id property identifier
                 * @param value the value to assign
                 * @return status of operation
                 */
                status_t                set_string(atom_t id, const LSPString *value);
                status_t                set_string(const char *id, const LSPString *value);
                status_t                set_string(const LSPString *id, const LSPString *value);

                /**
                 * Assign value to string property
                 * @param id property identifier
                 * @param value the UTF-8 encoded value to assign
                 * @return status of operation
                 */
                status_t                set_string(atom_t id, const char *value);
                status_t                set_string(const char *id, const char *value);
                status_t                set_string(const LSPString *id, const char *value);

                /**
                 * Reset property to it's default value.
                 * If property is overridden by parent, it's value is taken.
                 * If property has no overrides, default value passed on create()
                 * is taken. Otherwise 0/0.0f/false/empty string will be assigned.
                 *
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t                set_default(atom_t id);
                status_t                set_default(const char *id);
                status_t                set_default(const LSPString *id);

                /**
                 * Remove locally-created property
                 * @param id property identifier
                 * @return status of operation
                 */
                status_t                remove(atom_t id);
                status_t                remove(const char *id);
                status_t                remove(const LSPString *id);

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t                  atom_id(const char *name) const;

                /**
                 * Get atom identifier by name
                 * @param name atom name
                 * @return atom identifier or negative error code
                 */
                atom_t                  atom_id(const LSPString *name) const;

                /**
                 * Get atom name by identifier
                 * @param name atom name or NULL
                 * @return atom identifier
                 */
                const char             *atom_name(atom_t id) const;
        };

        #define LSP_TK_STYLE_DEF_BEGIN(Name, Parent) \
            class Name : public Parent \
            { \
                private: \
                    Name & operator = (Name &); \
                    Name(const Name &); \
                \
                public: \
                    explicit Name(::lsp::tk::Schema *schema); \
                \
                public: \
                    virtual ::lsp::status_t     init(); \
                \
                protected:

        #define LSP_TK_STYLE_DEF_END \
            }; \

        #define LSP_TK_STYLE_IMPL_BEGIN(Name, Parent) \
            Name::Name(::lsp::tk::Schema *schema): Parent(schema) {} \
            \
            status_t Name::init() \
            { \
                ::lsp::status_t res = Parent::init(); \
                if (res != ::lsp::STATUS_OK) \
                    return res;

        #define LSP_TK_STYLE_IMPL_END \
                return res; \
            } \

    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_STYLE_STYLE_H_ */
