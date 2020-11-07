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

#include <lsp-plug.in/tk/tk.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        Style::Style(Schema *schema)
        {
            pSchema     = schema;
            nFlags      = 0;
        }
        
        Style::~Style()
        {
            do_destroy();
        }

        status_t Style::init()
        {
            return STATUS_OK;
        }

        void Style::destroy()
        {
            do_destroy();
        }

        void Style::do_destroy()
        {
            // Unlock all pending transactions
            vLocks.flush();
            delayed_notify();

            // Unlink from parents and remove all children
            for (size_t i=0, n=vParents.size(); i<n; ++i)
            {
                Style *parent = vParents.uget(i);
                if (parent != NULL)
                    parent->vChildren.premove(this);
            }

            // Unlink from children and remove all parents
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                Style *child = vChildren.uget(i);
                if (child != NULL)
                {
                    child->vParents.premove(this);
                    child->synchronize();
                }
            }
            vChildren.flush();

            // Synchronize state with listeners and remove them
            synchronize();
            vListeners.flush();

            // Destroy stored properties
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
                undef_property(vProperties.uget(i));
            vProperties.flush();
        }

        void Style::undef_property(property_t *property)
        {
            if (property == NULL)
                return;

            switch (property->type)
            {
                case PT_STRING:
                    if (property->v.sValue != NULL)
                    {
                        ::free(property->v.sValue);
                        property->v.sValue      = NULL;
                    }
                    if (property->dv.sValue != NULL)
                    {
                        ::free(property->dv.sValue);
                        property->dv.sValue     = NULL;
                    }
                    break;
                default:
                    break;
            }

            property->type = PT_UNKNOWN;
        }

        bool Style::config_mode() const
        {
            return (pSchema != NULL) ? pSchema->config_mode() : false;
        }

        bool Style::set_override(bool set)
        {
            bool res = nFlags & S_OVERRIDE;
            nFlags = lsp_setflag(nFlags, S_OVERRIDE, set);
            return res;
        }

        bool Style::override_mode() const
        {
            return (nFlags & S_OVERRIDE) ? true : !config_mode();
        }

        status_t Style::copy_property(property_t *dst, const property_t *src)
        {
            // Check type of property
            if (src->type != dst->type)
                return STATUS_OK;

            // Update contents
            switch (src->type)
            {
                case PT_INT:
                    if (dst->v.iValue != src->v.iValue)
                    {
                        ++dst->changes;
                        dst->v.iValue   = src->v.iValue;
                    }

                    // Copy default value in INIT mode
                    if ((config_mode()) && (dst->dv.iValue != src->dv.iValue))
                    {
                        ++dst->changes;
                        dst->dv.iValue  = src->dv.iValue;
                    }
                    break;
                case PT_FLOAT:
                    if (dst->v.fValue != src->v.fValue)
                    {
                        ++dst->changes;
                        dst->v.fValue   = src->v.fValue;
                    }

                    // Copy default value in INIT mode
                    if ((config_mode()) && (dst->dv.fValue != src->dv.fValue))
                    {
                        ++dst->changes;
                        dst->dv.fValue  = src->dv.fValue;
                    }
                    break;
                case PT_BOOL:
                    if (dst->v.bValue != src->v.bValue)
                    {
                        ++dst->changes;
                        dst->v.bValue   = src->v.bValue;
                    }

                    // Copy default value in INIT mode
                    if ((config_mode()) && (dst->dv.bValue != src->dv.bValue))
                    {
                        ++dst->changes;
                        dst->dv.bValue  = src->dv.bValue;
                    }
                    break;
                case PT_STRING:
                {
                    // Update value if it has changed
                    if (::strcmp(dst->v.sValue, src->v.sValue) != 0)
                    {
                        char *tmp = ::strdup(src->v.sValue);
                        if (tmp == NULL)
                            return STATUS_NO_MEM;
                        ::free(dst->v.sValue);
                        dst->v.sValue   = tmp;
                        ++dst->changes;
                    }

                    // Copy default value in INIT mode
                    if ((config_mode()) && (::strcmp(dst->dv.sValue, src->dv.sValue) != 0))
                    {
                        char *tmp = ::strdup(src->dv.sValue);
                        if (tmp == NULL)
                            return STATUS_NO_MEM;
                        ::free(dst->dv.sValue);
                        dst->dv.sValue  = tmp;
                        ++dst->changes;
                    }
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            return STATUS_OK;
        }

        Style::property_t *Style::create_property(atom_t id, const property_t *src, size_t flags)
        {
            // Allocate property
            property_t *dst = vProperties.add();
            if (dst == NULL)
                return NULL;

            // Init contents
            switch (src->type)
            {
                case PT_INT:
                    dst->v.iValue   = src->v.iValue;
                    dst->dv.iValue  = src->dv.iValue;
                    break;
                case PT_FLOAT:
                    dst->v.fValue   = src->v.fValue;
                    dst->dv.fValue  = src->dv.fValue;
                    break;
                case PT_BOOL:
                    dst->v.bValue   = src->v.bValue;
                    dst->dv.bValue  = src->dv.bValue;
                    break;
                case PT_STRING:
                {
                    // Update value
                    if ((dst->v.sValue = ::strdup(src->v.sValue)) == NULL)
                    {
                        vProperties.premove(dst);
                        return NULL;
                    }
                    // Update default value
                    if ((dst->dv.sValue = ::strdup(src->dv.sValue)) == NULL)
                    {
                        ::free(dst->v.sValue);
                        dst->v.sValue   = NULL;
                        vProperties.premove(dst);
                        return NULL;
                    }
                    break;
                }
                default:
                    return NULL;
            }

            dst->id         = id;
            dst->refs       = (flags & F_CREATED) ? 1 : 0;
            dst->type       = src->type;
            dst->changes    = 0;
            dst->flags      = flags;
            dst->owner      = this;

            return dst;
        }

        Style::property_t *Style::create_property(atom_t id, property_type_t type, size_t flags)
        {
            // Allocate property
            property_t *dst = vProperties.add();
            if (dst == NULL)
                return NULL;

            // Init contents
            switch (type)
            {
                case PT_INT:
                    dst->v.iValue   = 0;
                    dst->dv.iValue  = 0;
                    break;
                case PT_FLOAT:
                    dst->v.fValue   = 0.0;
                    dst->dv.fValue  = 0.0;
                    break;
                case PT_BOOL:
                    dst->v.bValue   = false;
                    dst->dv.bValue  = false;
                    break;
                case PT_STRING:
                    if ((dst->v.sValue = ::strdup("")) == NULL)
                    {
                        vProperties.premove(dst);
                        return NULL;
                    }
                    if ((dst->dv.sValue = ::strdup("")) == NULL)
                    {
                        ::free(dst->v.sValue);
                        dst->v.sValue   = NULL;
                        vProperties.premove(dst);
                        return NULL;
                    }
                    break;
                default:
                    return NULL;
            }

            dst->id         = id;
            dst->refs       = (flags & F_CREATED) ? 1 : 0;
            dst->type       = type;
            dst->changes    = 0;
            dst->flags      = flags;
            dst->owner      = this;

            return dst;
        }

        status_t Style::sync_property(property_t *p)
        {
            lsp_trace("name = %s, flags=0x%x", atom_name(p->id), p->flags);

            // Local-overridden properties can not be changed by parent ones
            if (p->flags & F_OVERRIDDEN)
                return STATUS_OK;

            status_t res;
            size_t changes      = p->changes;

            // Lookup for a parent property, copy default value
            property_t *parent  = get_parent_property(p->id);
            res = (parent != NULL) ? copy_property(p, parent) : set_property_default(p);

            // Deploy changes if there are any
            if ((res == STATUS_OK) && (changes != p->changes))
            {
                notify_listeners(p);
                notify_children(p);
            }
            return res;
        }

        status_t Style::set_property_default(property_t *p)
        {
            switch (p->type)
            {
                case PT_INT:
                    if (p->v.iValue == p->dv.iValue)
                        return STATUS_OK;
                    p->v.iValue = p->dv.iValue;
                    break;
                case PT_FLOAT:
                    if (p->v.fValue == p->dv.fValue)
                        return STATUS_OK;
                    p->v.fValue = p->dv.fValue;
                    break;
                case PT_BOOL:
                    if (p->v.bValue == p->dv.bValue)
                        return STATUS_OK;
                    p->v.bValue = p->dv.bValue;
                    break;
                case PT_STRING:
                {
                    if (::strcmp(p->v.sValue, p->dv.sValue) == 0)
                        return STATUS_OK;
                    char *tmp = ::strdup(p->dv.sValue);
                    if (tmp == NULL)
                        return STATUS_NO_MEM;
                    ::free(p->v.sValue);
                    p->v.sValue = tmp;
                    break;
                }
                default:
                    return STATUS_BAD_TYPE;
            }

            ++p->changes;
            return STATUS_OK;
        }

        void Style::synchronize()
        {
            // For each property: copy value from parent and notify children and listeners for changes
            property_t *vp = vProperties.array();
            for (size_t i=0, n=vProperties.size(); i < n; ++i)
                sync_property(&vp[i]);

            // Call all children for synchronize()
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                Style *child = vChildren.uget(i);
                if (child != NULL)
                    child->synchronize();
            }
        }

        void Style::delayed_notify()
        {
            size_t notified;

            if (nFlags & S_DELAYED)
                return;

            nFlags |= S_DELAYED; // Disallow delayed notify because it is already active
            do
            {
                notified = 0;
                for (size_t i=0, n=vProperties.size(); i < n; ++i)
                {
                    // Notify if notification is pending
                    property_t *prop = vProperties.uget(i);
                    if (prop != NULL)
                    {
                        notified       += notify_listeners_delayed(prop);
                        notified       += notify_children_delayed(prop);
                    }
                }
            } while (notified > 0);
            nFlags &= ~S_DELAYED;
        }

        void Style::notify_change(property_t *prop)
        {
            // Find the matching property (if present)
            property_t *p = get_property(prop->id);

            // Property not found?
            if ((p == NULL) || (p->refs <= 0))
            {
                notify_children(prop); // Just bypass event to children
                return;
            }
            else if (p->flags & F_OVERRIDDEN) // Locally overridden property? Ignore the event
                return;

            // Try to copy value from parent property first
            property_t *parent  = get_parent_property(prop->id);
            size_t change       = p->changes;
            status_t res        = copy_property(p, (parent != NULL) ? parent : prop);

            // Notify children and listeners about property change if all is successful
            if ((res == STATUS_OK) && (change != p->changes))
            {
                notify_listeners(p);
                notify_children(p);
            }
        }

        void Style::notify_children(property_t *prop)
        {
            // In transaction, just set notification flag instead of issuing notification procedure
            if ((vLocks.size() > 0) && (prop->owner == this))
            {
                prop->flags    |= F_NTF_CHILDREN;
                return;
            }

            // Notify all children about property change
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                Style *child = vChildren.uget(i);
                if (child != NULL)
                    child->notify_change(prop);
            }
        }

        size_t Style::notify_children_delayed(property_t *prop)
        {
            size_t count = 0;

            if (prop->flags & F_NTF_CHILDREN)
            {
                prop->flags &= ~F_NTF_CHILDREN;

                // Notify all children about property change
                for (size_t i=0, n=vChildren.size(); i<n; ++i)
                {
                    Style *child = vChildren.uget(i);
                    if (child != NULL)
                    {
                        child->notify_change(prop);
                        ++count;
                    }
                }
            }

            return count;
        }

        void Style::notify_listeners(property_t *prop)
        {
            atom_t id = prop->id;

            // Check whether we are in transactional state
            if ((vLocks.size() > 0) && (prop->owner == this))
            {
                size_t count = 0;

                // Mark all listeners for pending property change event except listeners in transaction
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    listener_t *lst = vListeners.uget(i);
                    if ((lst != NULL) && (lst->nId == id))
                    {
                        // Check that listener is not excluded from notifications
                        if (vLocks.index_of(lst->pListener) < 0)
                        {
                            lst->bNotify    = true;
                            ++count;
                        }
                    }
                }

                // Are there any listeners pending?
                if (count > 0)
                    prop->flags    |= F_NTF_LISTENERS;
            }
            else
            {
                // Notify all listeners about property change
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    listener_t *lst = vListeners.uget(i);
                    if ((lst != NULL) && (lst->nId == id))
                        lst->pListener->notify(id);
                }
            }
        }

        size_t Style::notify_listeners_delayed(property_t *prop)
        {
            size_t count = 0;
            if (prop->flags & F_NTF_LISTENERS)
            {
                // Reset notification flag
                prop->flags &= ~F_NTF_LISTENERS;

                // Notify all allowed listeners about property change
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    listener_t *lst = vListeners.uget(i);
                    if ((lst != NULL) && (lst->nId == prop->id) && (lst->bNotify))
                    {
                        lst->bNotify    = false;
                        lst->pListener->notify(prop->id);
                        ++count;
                    }
                }
            }
            return count;
        }

        status_t Style::add_child(Style *child, ssize_t idx)
        {
            // Check arguments
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (idx < 0)
                idx = vChildren.size();
            else if (size_t(idx) > vChildren.size())
                return STATUS_INVALID_VALUE;

            // Check
            if (vChildren.index_of(child) >= 0)
                return STATUS_ALREADY_EXISTS;
            if ((child == this) || (child->has_child(this, true)))
                return STATUS_BAD_HIERARCHY;

            // Make bindings
            if (!vChildren.insert(idx, child))
                return STATUS_NO_MEM;
            if (!child->vParents.add(this))
            {
                vChildren.premove(child);
                return STATUS_NO_MEM;
            }

            // Synchronize state
            child->synchronize();

            return STATUS_OK;
        }

        status_t Style::add_parent(Style *parent, ssize_t idx)
        {
            // Check arguments
            if (parent == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (idx < 0)
                idx = vParents.size();
            else if (size_t(idx) > vParents.size())
                return STATUS_INVALID_VALUE;

            // Check
            if (vParents.index_of(parent) >= 0)
                return STATUS_ALREADY_EXISTS;
            if ((parent == this) || (this->has_child(parent, true)))
                return STATUS_BAD_HIERARCHY;

            // Make bindings
            if (!vParents.insert(idx, parent))
                return STATUS_NO_MEM;
            if (!parent->vChildren.add(this))
            {
                vParents.premove(parent);
                return STATUS_NO_MEM;
            }

            // Synchronize state
            synchronize();

            return STATUS_OK;
        }

        status_t Style::remove_child(Style *child)
        {
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vChildren.premove(child))
                return STATUS_NOT_FOUND;

            child->vParents.premove(this);
            child->synchronize();

            return STATUS_OK;
        }

        status_t Style::remove_all_children()
        {
            if (vChildren.is_empty())
                return STATUS_OK;

            // Remove all children
            lltl::parray<Style> children;
            children.swap(vChildren);

            // Remove self from parent list of each child
            for (size_t i=0, n=children.size(); i < n; ++i)
            {
                Style *child = children.uget(i);
                if (child != NULL)
                    child->vParents.premove(this);
            }

            // Synchronize children
            for (size_t i=0, n=children.size(); i < n; ++i)
            {
                Style *child = children.uget(i);
                if (child != NULL)
                    child->synchronize();
            }

            return STATUS_OK;
        }

        status_t Style::remove_parent(Style *parent)
        {
            if (parent == NULL)
                return STATUS_BAD_ARGUMENTS;

            if (!vParents.premove(parent))
                return STATUS_NOT_FOUND;

            parent->vChildren.premove(this);
            synchronize();

            return STATUS_OK;
        }

        status_t Style::remove_all_parents()
        {
            if (vParents.is_empty())
                return STATUS_OK;

            // Remove all parents
            lltl::parray<Style> parents;
            parents.swap(vParents);

            // Remove self from parent lists
            for (size_t i=0, n=parents.size(); i < n; ++i)
            {
                Style *parent = parents.uget(i);
                if (parent != NULL)
                    parent->vChildren.premove(this);
            }

            // Synchronize state
            synchronize();

            return STATUS_OK;
        }

        bool Style::has_child(Style *child, bool recursive)
        {
            if ((child == NULL) || (child == this))
                return false;

            // First, lookup self children
            if (vChildren.index_of(child) >= 0)
                return true;
            else if (!recursive)
                return false;

            // Second, lookup recursive
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                Style *s = vChildren.uget(i);
                if ((s != NULL) && (s->has_child(child, recursive)))
                    return true;
            }

            return false;
        }

        bool Style::has_parent(Style *parent, bool recursive)
        {
            if ((parent == NULL) || (parent == this))
                return false;

            // First, lookup self children
            if (vParents.index_of(parent) >= 0)
                return true;
            else if (!recursive)
                return false;

            // Second, lookup recursively parents
            for (size_t i=0, n=vParents.size(); i<n; ++i)
            {
                Style *s = vParents.uget(i);
                if ((s != NULL) && (s->has_parent(parent, recursive)))
                    return true;
            }

            return false;
        }

        bool Style::is_bound(atom_t id, IStyleListener *listener) const
        {
            const listener_t *pv = vListeners.array();
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                const listener_t *p = &pv[i];
                if ((p->nId == id) && (p->pListener == listener))
                    return true;
            }
            return false;
        }

        bool Style::is_bound(const char *id, IStyleListener *listener) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_bound(atom, listener) : false;
        }

        bool Style::is_bound(const LSPString *id, IStyleListener *listener) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_bound(atom, listener) : false;
        }

        status_t Style::bind(atom_t id, property_type_t type, IStyleListener *listener)
        {
            // Listener can not be NULL
            if (listener == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t *p   = get_property(id);
            listener_t *lst = NULL;

            // Property has been found?
            if (p == NULL)
            {
                // Lookup parent property
                property_t *parent = get_parent_property(id);

                // Create property
                p = (parent != NULL) ? create_property(id, parent, 0) : create_property(id, type, 0);
                if (p == NULL)
                    return STATUS_NO_MEM;

                // Allocate listener binding
                lst = vListeners.add();
                if (lst == NULL)
                {
                    undef_property(p);
                    vProperties.premove(p);
                    return STATUS_NO_MEM;
                }
            }
            else
            {
                // Check that not already bound
                if (is_bound(id, listener))
                    return STATUS_ALREADY_BOUND;

                // Just allocate listener binding
                lst = vListeners.add();
                if (lst == NULL)
                    return STATUS_NO_MEM;
            }

            // Save listener to allocated binding
            lst->nId        = p->id;
            lst->bNotify    = vLocks.index_of(listener) < 0;
            lst->pListener  = listener;
            ++p->refs;

            if (lst->bNotify)
            {
                p->flags       |= F_NTF_LISTENERS;
                if ((vLocks.is_empty()) || (p->owner != this))
                    notify_listeners_delayed(p);
            }
            notify_children(p);

            return STATUS_OK;
        }

        status_t Style::bind(const char *id, property_type_t type, IStyleListener *listener)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? bind(atom, type, listener) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::bind(const LSPString *id, property_type_t type, IStyleListener *listener)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? bind(atom, type, listener) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::unbind(atom_t id, IStyleListener *listener)
        {
            // Find listener binding
            listener_t *lst = NULL;
            listener_t *pv = vListeners.array();
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                listener_t *p = &pv[i];
                if ((p->nId == id) && (p->pListener == listener))
                {
                    lst = p;
                    break;
                }
            }

            if (lst == NULL)
                return STATUS_NOT_BOUND;

            // Get property
            property_t *p = get_property(id);
            if (p == NULL)
                return STATUS_CORRUPTED; // This actually should not ever happen

            // Remove listener binding and dereference property
            vListeners.premove(lst);
            deref_property(p);

            return STATUS_OK;
        }

        status_t Style::unbind(const char *id, IStyleListener *listener)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? unbind(atom, listener) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::unbind(const LSPString *id, IStyleListener *listener)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? unbind(atom, listener) : STATUS_UNKNOWN_ERR;
        }

        void Style::deref_property(property_t *p)
        {
            // Decrement number of references
            if ((--p->refs) > 0)
                return;

            // Destroy property if there are no more references
            // Since number of references is 0, property is not visible to children
            p->flags &= ~F_OVERRIDDEN;
            undef_property(p);
            property_t *parent = get_parent_property(p->id);
            notify_children((parent != NULL) ? parent : p);
            vProperties.premove(p);
        }

        Style::property_t *Style::get_property(atom_t id)
        {
            for (size_t i=0, n=vProperties.size(); i<n; ++i)
            {
                property_t *p   = vProperties.uget(i);
                if ((p != NULL) && (p->id == id))
                    return p;
            }
            return NULL;
        }

        Style::property_t *Style::get_parent_property(atom_t id)
        {
            // Lookup parents in reverse order
            for (ssize_t i=vParents.size() - 1; i >= 0; --i)
            {
                Style *curr = vParents.uget(i);
                if (curr == NULL)
                    continue;

                // Try to fetch property first
                property_t *p = curr->get_property(id);
                if (p == NULL) // Property not found?
                    p = curr->get_parent_property(id); // Search parents recursively
                if (p != NULL)
                    return p;
            }

            return NULL;
        }

        status_t Style::begin()
        {
            return (vLocks.push(static_cast<IStyleListener *>(NULL))) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Style::begin(IStyleListener *listener)
        {
            return (vLocks.push(listener)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Style::end()
        {
            ssize_t n = vLocks.size();
            if ((n--) <= 0)
                return STATUS_BAD_STATE;

            vLocks.pop();
            if (n <= 0)
                delayed_notify();
            return STATUS_OK;
        }

        Style::property_t *Style::get_property_recursive(atom_t id)
        {
            property_t *p = get_property(id);
            return (p != NULL) ? p : get_parent_property(id);
        }

        status_t Style::get_int(atom_t id, ssize_t *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = 0;
                return STATUS_OK;
            }
            else if (prop->type != PT_INT)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.iValue;
            return STATUS_OK;
        }

        status_t Style::get_int(const char *id, ssize_t *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_int(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_int(const LSPString *id, ssize_t *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_int(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_float(atom_t id, float *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = 0.0f;
                return STATUS_OK;
            }
            else if (prop->type != PT_FLOAT)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.fValue;
            return STATUS_OK;
        }

        status_t Style::get_float(const char *id, float *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_float(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_float(const LSPString *id, float *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_float(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_bool(atom_t id, bool *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                *dst = false;
                return STATUS_OK;
            }
            else if (prop->type != PT_BOOL)
                return STATUS_BAD_TYPE;
            if (dst != NULL)
                *dst = prop->v.bValue;
            return STATUS_OK;
        }

        status_t Style::get_bool(const char *id, bool *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_bool(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_bool(const LSPString *id, bool *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_bool(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_string(atom_t id, LSPString *dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                if (dst != NULL)
                    dst->truncate();
                return STATUS_OK;
            }
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            if (dst == NULL)
                return STATUS_OK;
            return (dst->set_utf8(prop->v.sValue)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t Style::get_string(const char *id, LSPString *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_string(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_string(const LSPString *id, LSPString *dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_string(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_string(atom_t id, const char **dst) const
        {
            const property_t *prop = get_property_recursive(id);
            if (prop == NULL)
            {
                if (dst != NULL)
                    *dst = "";
                return STATUS_OK;
            }
            else if (prop->type != PT_STRING)
                return STATUS_BAD_TYPE;

            if (dst != NULL)
                *dst = prop->v.sValue;
            return STATUS_OK;
        }

        status_t Style::get_string(const char *id, const char **dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_string(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::get_string(const LSPString *id, const char **dst) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_string(atom, dst) : STATUS_UNKNOWN_ERR;
        }

        bool Style::is_default(atom_t id) const
        {
            const property_t *prop = get_property(id);
            return (prop != NULL) ? !(prop->flags & F_OVERRIDDEN) : true;
        }

        bool Style::is_default(const char *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_default(atom) : false;
        }

        bool Style::is_default(const LSPString *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_default(atom) : false;
        }

        bool Style::is_overridden(atom_t id) const
        {
            const property_t *prop = get_property(id);
            return (prop != NULL) ? (prop->flags & F_OVERRIDDEN) : false;
        }

        bool Style::is_overridden(const char *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_overridden(atom) : false;
        }

        bool Style::is_overridden(const LSPString *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_overridden(atom) : false;
        }

        bool Style::exists(atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL);
        }

        bool Style::exists(const char *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? exists(atom) : false;
        }

        bool Style::exists(const LSPString *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? exists(atom) : false;
        }

        bool Style::is_local(atom_t id) const
        {
            const property_t *prop = get_property(id);
            return (prop != NULL) && (prop->flags & F_CREATED);
        }

        bool Style::is_local(const char *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_local(atom) : false;
        }

        bool Style::is_local(const LSPString *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? is_local(atom) : false;
        }

        property_type_t Style::get_type(atom_t id) const
        {
            const property_t *prop = get_property_recursive(id);
            return (prop != NULL) ? prop->type : PT_UNKNOWN;
        }

        property_type_t Style::get_type(const char *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_type(atom) : PT_UNKNOWN;
        }

        property_type_t Style::get_type(const LSPString *id) const
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? get_type(atom) : PT_UNKNOWN;
        }

        status_t Style::set_property(atom_t id, property_t *src)
        {
            status_t res = STATUS_OK;
            property_t *p = get_property(id);

            if (p == NULL)
            {
                p = create_property(id, src, (override_mode()) ? F_OVERRIDDEN : 0);
                if (p != NULL)
                {
                    notify_listeners(p);
                    notify_children(p);
                }
                else
                    res         = STATUS_NO_MEM;
            }
            else
            {
                // Notify only if value has changed
                size_t change = p->changes;
                res           = copy_property(p, src);

                if (res == STATUS_OK)
                {
                    if (override_mode())
                        p->flags   |= F_OVERRIDDEN;

                    if (change != p->changes)
                    {
                        notify_listeners(p);
                        notify_children(p);
                    }
                }
            }

            return res;
        }

        status_t Style::set_int(atom_t id, ssize_t value)
        {
            property_t tmp;
            tmp.type        = PT_INT;
            tmp.v.iValue    = value;
            tmp.dv.iValue   = value;
            return set_property(id, &tmp);
        }

        status_t Style::set_int(const char *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_int(const LSPString *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_float(atom_t id, float value)
        {
            property_t tmp;
            tmp.type        = PT_FLOAT;
            tmp.v.fValue    = value;
            tmp.dv.fValue   = value;
            return set_property(id, &tmp);
        }

        status_t Style::set_float(const char *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_float(const LSPString *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_bool(atom_t id, bool value)
        {
            property_t tmp;
            tmp.type        = PT_BOOL;
            tmp.v.bValue    = value;
            tmp.dv.bValue   = value;
            return set_property(id, &tmp);
        }

        status_t Style::set_bool(const char *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_bool(const LSPString *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_string(atom_t id, const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value->get_utf8());
            tmp.dv.sValue   = tmp.v.sValue;
            return set_property(id, &tmp);
        }

        status_t Style::set_string(const char *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_string(const LSPString *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_string(atom_t id, const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value);
            tmp.dv.sValue   = tmp.v.sValue;
            return set_property(id, &tmp);
        }

        status_t Style::set_string(const char *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_string(const LSPString *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::set_default(atom_t id)
        {
            property_t *p = get_property(id);
            if (p == NULL)
                return STATUS_NOT_FOUND;
            else if (!(p->flags & F_OVERRIDDEN))
                return STATUS_OK;

            // Initialize property with default value
            p->flags   &= ~F_OVERRIDDEN;
            return sync_property(p);
        }

        status_t Style::set_default(const char *id)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? set_default(atom) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::update_default_value(property_t *p, const property_t *src)
        {
            // Check that types of properties match
            if (p->type != src->type)
                return STATUS_BAD_TYPE;

            // Override values
            size_t changes  = p->changes;
            switch (p->type)
            {
                case PT_INT:
                    if ((!(p->flags & F_OVERRIDDEN)) &&
                        (p->v.iValue != src->v.iValue))
                    {

                        p->v.iValue     = src->v.iValue;
                        ++p->changes;
                    }
                    p->dv.iValue    = src->dv.iValue;
                    break;
                case PT_FLOAT:
                    if ((!(p->flags & F_OVERRIDDEN)) &&
                        (p->v.fValue != src->v.fValue))
                    {

                        p->v.fValue     = src->v.fValue;
                        ++p->changes;
                    }
                    p->dv.fValue    = src->dv.fValue;
                    break;
                case PT_BOOL:
                    if ((!(p->flags & F_OVERRIDDEN)) &&
                        (p->v.bValue != src->v.bValue))
                    {

                        p->v.bValue     = src->v.bValue;
                        ++p->changes;
                    }
                    p->dv.bValue    = src->dv.bValue;
                    break;
                case PT_STRING:
                {
                    // Allocate strings
                    char *ds = ::strdup(src->dv.sValue);
                    if (ds == NULL)
                        return STATUS_NO_MEM;

                    // Need to override values?
                    if ((!(p->flags & F_OVERRIDDEN)) &&
                        (::strcmp(p->v.sValue, src->v.sValue) != 0))
                    {
                        char *s         = ::strdup(src->v.sValue);
                        if (s == NULL)
                        {
                            ::free(ds);
                            return STATUS_NO_MEM;
                        }

                        ::free(p->v.sValue);
                        p->v.sValue     = s;
                        ++p->changes;
                    }

                    ::free(p->dv.sValue);
                    p->dv.sValue    = ds;
                    break;
                }
                default:
                    return STATUS_UNKNOWN_ERR;
            }
            p->flags       |= F_CREATED;    // Mark as explicitly created
            ++p->refs;                      // Increment number of references

            if (changes != p->changes)
            {
                notify_listeners(p);
                notify_children(p);
            }

            return STATUS_OK;
        }

        status_t Style::create_local_property(atom_t id, const property_t *src)
        {
            // Lookup for property first
            property_t *p = get_property(id);
            if (p == NULL)
            {
                // Just create property and mark as explicitly created
                p = create_property(id, src, F_CREATED);
                if (p == NULL)
                    return STATUS_NO_MEM;

                notify_listeners(p);
                notify_children(p);

                return STATUS_OK;
            }
            else if (p->flags & F_CREATED) // Check that property has already been explicitly created previously
                return STATUS_ALREADY_EXISTS;

            // Update default value of the property
            return update_default_value(p, src);
        }

        status_t Style::override_local_property(atom_t id, const property_t *src)
        {
            // Lookup for property first
            property_t *p = get_property(id);
            if ((p == NULL) || (!(p->flags & F_CREATED)))
                return STATUS_NOT_FOUND;

            // Update default value of the property
            return update_default_value(p, src);
        }

        status_t Style::create_int(atom_t id, ssize_t value)
        {
            property_t tmp;
            tmp.type        = PT_INT;
            tmp.v.iValue    = value;
            tmp.dv.iValue   = value;
            return create_local_property(id, &tmp);
        }

        status_t Style::create_int(const char *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_int(const LSPString *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_float(atom_t id, float value)
        {
            property_t tmp;
            tmp.type        = PT_FLOAT;
            tmp.v.fValue    = value;
            tmp.dv.fValue   = value;
            return create_local_property(id, &tmp);
        }

        status_t Style::create_float(const char *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_float(const LSPString *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_bool(atom_t id, bool value)
        {
            property_t tmp;
            tmp.type        = PT_BOOL;
            tmp.v.bValue    = value;
            tmp.dv.bValue   = value;
            return create_local_property(id, &tmp);
        }

        status_t Style::create_bool(const char *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_bool(const LSPString *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_string(atom_t id, const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value->get_utf8());
            tmp.dv.sValue   = const_cast<char *>(value->get_utf8());
            return create_local_property(id, &tmp);
        }

        status_t Style::create_string(const char *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_string(const LSPString *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_string(atom_t id, const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value);
            tmp.dv.sValue   = const_cast<char *>(value);
            return create_local_property(id, &tmp);
        }

        status_t Style::create_string(const char *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::create_string(const LSPString *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? create_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_int(atom_t id, ssize_t value)
        {
            property_t tmp;
            tmp.type        = PT_INT;
            tmp.v.iValue    = value;
            tmp.dv.iValue   = value;
            return override_local_property(id, &tmp);
        }

        status_t Style::override_int(const char *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_int(const LSPString *id, ssize_t value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_int(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_float(atom_t id, float value)
        {
            property_t tmp;
            tmp.type        = PT_FLOAT;
            tmp.v.fValue    = value;
            tmp.dv.fValue   = value;
            return override_local_property(id, &tmp);
        }

        status_t Style::override_float(const char *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_float(const LSPString *id, float value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_float(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_bool(atom_t id, bool value)
        {
            property_t tmp;
            tmp.type        = PT_BOOL;
            tmp.v.bValue    = value;
            tmp.dv.bValue   = value;
            return override_local_property(id, &tmp);
        }

        status_t Style::override_bool(const char *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_bool(const LSPString *id, bool value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_bool(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_string(atom_t id, const LSPString *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value->get_utf8());
            tmp.dv.sValue   = const_cast<char *>(value->get_utf8());
            return override_local_property(id, &tmp);
        }

        status_t Style::override_string(const char *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_string(const LSPString *id, const LSPString *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_string(atom_t id, const char *value)
        {
            if (value == NULL)
                return STATUS_BAD_ARGUMENTS;

            property_t tmp;
            tmp.type        = PT_STRING;
            tmp.v.sValue    = const_cast<char *>(value);
            tmp.dv.sValue   = const_cast<char *>(value);
            return override_local_property(id, &tmp);
        }

        status_t Style::override_string(const char *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::override_string(const LSPString *id, const char *value)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? override_string(atom, value) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::remove(atom_t id)
        {
            property_t *p   = get_property(id);
            if (p == NULL)
                return STATUS_NOT_FOUND;
            else if (!(p->flags & F_CREATED))
                return STATUS_PERMISSION_DENIED;

            p->flags &= ~F_CREATED;
            deref_property(p);

            return STATUS_OK;
        }

        status_t Style::remove(const char *id)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? remove(atom) : STATUS_UNKNOWN_ERR;
        }

        status_t Style::remove(const LSPString *id)
        {
            atom_t atom = pSchema->atom_id(id);
            return (atom >= 0) ? remove(atom) : STATUS_UNKNOWN_ERR;
        }

        atom_t Style::atom_id(const char *name) const
        {
            return pSchema->atom_id(name);
        }

        atom_t Style::atom_id(const LSPString *name) const
        {
            return pSchema->atom_id(name);
        }

        const char *Style::atom_name(atom_t id) const
        {
            return pSchema->atom_name(id);
        }

    } /* namespace tk */
} /* namespace lsp */
