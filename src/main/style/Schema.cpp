/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>
#include <lsp-plug.in/common/debug.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_IMPL_BEGIN(Root, Style)
                // Bind
                sScaling.bind("size.scaling", this);

                // Configure
                sScaling.set(1.0f);
            LSP_TK_STYLE_IMPL_END

            LSP_SYMBOL_HIDDEN
            StyleFactory<Root> RootFactory(NULL);
        }

        Schema::Schema(Atoms *atoms)
        {
            pAtoms          = atoms;
            nFlags          = 0;
            pRoot           = NULL;
        }
    
        Schema::~Schema()
        {
            // Manually unbind all properties before destroying context
            sScaling.unbind();

            // Destroy named styles
            lltl::parray<Style> vs;
            vStyles.values(&vs);
            vStyles.flush();
            for (size_t i=0, n=vs.size(); i<n; ++i)
            {
                Style *s = vs.uget(i);
                if (s != NULL)
                    delete s;
            }

            // Destroy root style
            if (pRoot != NULL)
            {
                delete pRoot;
                pRoot = NULL;
            }

            // Destroy colors
            destroy_colors();
        }

        void Schema::destroy_colors()
        {
            if (vColors.is_empty())
                return;

            lltl::parray<lsp::Color> vc;
            vColors.values(&vc);
            vColors.flush();

            for (size_t i=0, n=vc.size(); i<n; ++i)
            {
                lsp::Color *c = vc.get(i);
                if (c != NULL)
                    delete c;
            }
        }

        status_t Schema::init(lltl::parray<IStyleFactory> *list)
        {
            return init(list->array(), list->size());
        }

        status_t Schema::init(lltl::parray<IStyleFactory> &list)
        {
            return init(list.array(), list.size());
        }

        status_t Schema::init(IStyleFactory **list, size_t n)
        {
            // Check for initialize state
            if (nFlags & S_INITIALIZED)
                return STATUS_BAD_STATE;
            nFlags = S_INITIALIZED | S_CONFIGURING;

            // Create root style
            if (pRoot == NULL)
            {
                pRoot = style::RootFactory.create(this);
                if (pRoot == NULL)
                    return STATUS_NO_MEM;
            }

            // Initialize root style with properties
            bind(pRoot);

            // Create all necessary styles
            for (size_t i=0; i<n; ++i)
            {
                LSP_STATUS_ASSERT(create_style(list[i]));
            }

            // Unset 'configuring' mode
            nFlags &= ~S_CONFIGURING;

            return STATUS_OK;
        }

        status_t Schema::apply(StyleSheet *sheet)
        {
            if (sheet == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Apply settings in configuration mode
            nFlags |= S_CONFIGURING;
            status_t res = apply_internal(sheet);
            nFlags &= ~S_CONFIGURING;

            return res;
        }

        status_t Schema::apply_internal(StyleSheet *sheet)
        {
            // Destroy colors
            destroy_colors();

            // Copy colors from sheet
            lltl::parray<LSPString> vk;
            sheet->vColors.keys(&vk);
            for (size_t i=0, n=vk.size(); i<n; ++i)
            {
                LSPString *key      = vk.uget(i);
                lsp::Color *color   = sheet->vColors.get(key);
                if ((key == NULL) || (color == NULL))
                    return STATUS_BAD_STATE;

                lsp::Color *xc      = new lsp::Color(color);
                if (xc == NULL)
                    return STATUS_NO_MEM;

                if (!vColors.create(key, xc))
                {
                    delete xc;
                    return STATUS_NO_MEM;
                }
            }

            // Destroy all relations between styles
            status_t res;
            lltl::parray<Style> vs;
            if (!vStyles.values(&vs))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vs.size(); i<n; ++i)
            {
                Style *s = vs.uget(i);
                s->remove_all_parents();
                s->remove_all_children();
            }

            // Initialize each style
            lsp_trace("Applying stylesheet to root style");
            if (sheet->pRoot != NULL)
            {
                res = apply_settings(pRoot, sheet->pRoot);
                if (res == STATUS_OK)
                    res = apply_relations(pRoot, sheet->pRoot);
                if (res != STATUS_OK)
                    return res;
            }

            // Iterate over named styles
            lltl::parray<LSPString>  vss;
            if (!vStyles.keys(&vss))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vss.size(); i<n; ++i)
            {
                LSPString *name         = vss.uget(i);
                Style *s                = vStyles.get(name);
                if (s == NULL)
                    continue;

                StyleSheet::style_t *xs = sheet->vStyles.get(name);
                if (xs != NULL)
                {
                    lsp_trace("Applying stylesheet to style '%s'", name->get_utf8());
                    res = apply_settings(s, xs);

                    if (res == STATUS_OK)
                        res = apply_relations(s, xs);
                }
                else
                    res = s->add_parent(pRoot);

                if (res != STATUS_OK)
                    return res;
            }

            return STATUS_OK;
        }

        status_t Schema::apply_settings(Style *s, StyleSheet::style_t *xs)
        {
            lltl::parray<LSPString> pnames;
            if (!xs->properties.keys(&pnames))
                return STATUS_NO_MEM;

            property_value_t v;
            status_t res;

            for (size_t i=0, n=pnames.size(); i<n; ++i)
            {
                LSPString *name         = pnames.uget(i);
                LSPString *value        = xs->properties.get(name);
                property_type_t type    = s->get_type(name);

                lsp_trace("  %s = %s", name->get_utf8(), value->get_utf8());

                if (parse_property_value(&v, value, type) == STATUS_OK)
                {
                    bool over = s->set_override(true);
                    switch (v.type)
                    {
                        case PT_BOOL:   res = s->set_bool(name, v.bvalue);      break;
                        case PT_INT:    res = s->set_int(name, v.ivalue);       break;
                        case PT_FLOAT:  res = s->set_float(name, v.fvalue);     break;
                        case PT_STRING: res = s->set_string(name, &v.svalue);   break;
                        default:        res = STATUS_OK;
                    }
                    s->set_override(over);

                    if (res != STATUS_OK)
                        return res;
                }
            }

            return STATUS_OK;
        }

        status_t Schema::apply_relations(Style *s, StyleSheet::style_t *xs)
        {
            status_t res;

            for (size_t i=0, n=xs->parents.size(); i<n; ++i)
            {
                LSPString *parent = xs->parents.get(i);
                Style *ps = (parent->equals_ascii("root")) ? pRoot : vStyles.get(parent);
                if (ps == NULL)
                    continue;

                lsp_trace("  parent: %s", parent->get_utf8());
                res = s->add_parent(ps);
                if (res != STATUS_OK)
                    return res;
            }

            return STATUS_OK;
        }

        status_t Schema::create_style(IStyleFactory *init)
        {
            LSPString name;

            // Style should have name
            if (!name.set_utf8(init->name()))
                return STATUS_NO_MEM;

            // Duplicates are disallowed
            if (vStyles.contains(&name))
            {
                lsp_warn("Duplicate style name: %s", init->name());
                return STATUS_ALREADY_EXISTS;
            }

            // Create style
            lsp_trace("Creating style '%s'...", init->name());
            Style *style    = init->create(this);
            if (style == NULL)
                return STATUS_NO_MEM;

            // Initialize style and bind to Root by default
            status_t res    = style->add_parent(pRoot);
            if (res != STATUS_OK)
            {
                delete style;
                return res;
            }

            // Register style in the list
            if (!vStyles.create(&name, style))
            {
                delete style;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        void Schema::bind(Style *root)
        {
            // Bind properties
            sScaling.bind("size.scaling", root);
        }

        status_t Schema::parse_property_value(property_value_t *v, const LSPString *text, property_type_t pt)
        {
            io::InStringSequence is(text);
            expr::Tokenizer tok(&is);
            expr::token_t t;

            switch (pt)
            {
                case PT_BOOL:
                    t = tok.get_token(expr::TF_GET);
                    if (t == expr::TT_TRUE)
                        v->bvalue       = true;
                    else if (t == expr::TT_FALSE)
                        v->bvalue       = false;
                    else
                        return STATUS_BAD_FORMAT;
                    v->type         = PT_BOOL;
                    break;

                case PT_INT:
                    t = tok.get_token(expr::TF_GET);
                    if (t != expr::TT_IVALUE)
                        return STATUS_BAD_FORMAT;
                    v->type         = PT_INT;
                    v->ivalue       = tok.int_value();
                    break;

                case PT_FLOAT:
                    t = tok.get_token(expr::TF_GET);
                    if (t == expr::TT_FVALUE)
                        v->fvalue       = tok.float_value();
                    else if (t == expr::TT_IVALUE)
                        v->fvalue       = tok.int_value();
                    else
                        return STATUS_BAD_FORMAT;
                    v->type         = PT_FLOAT;
                    break;

                case PT_STRING:
                    if (!v->svalue.set(text))
                        return STATUS_NO_MEM;
                    v->type     = PT_STRING;
                    return STATUS_OK;

                default:
                    t = tok.get_token(expr::TF_GET);
                    if ((t == expr::TT_TRUE) || (t == expr::TT_FALSE))
                    {
                        v->bvalue       = (t == expr::TT_TRUE);
                        v->type         = PT_BOOL;
                    }
                    else if (t == expr::TT_IVALUE)
                    {
                        v->ivalue       = tok.int_value();
                        v->type         = PT_INT;
                    }
                    else if (t == expr::TT_FVALUE)
                    {
                        v->fvalue       = tok.float_value();
                        v->type         = PT_FLOAT;
                    }
                    else
                    {
                        if (!v->svalue.set(text))
                            return STATUS_NO_MEM;
                        v->type         = PT_STRING;
                        return STATUS_OK;
                    }
                    break;
            }

            return (tok.get_token(expr::TF_GET) == expr::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }
    
        Style *Schema::get(const char *id, StyleInitializer *init)
        {
            LSPString tmp;
            if (!tmp.set_utf8(id))
                return NULL;
            return get(&tmp, init);
        }

        Style *Schema::get(const LSPString *id, StyleInitializer *init)
        {
            // Check that style exists
            Style *s  = vStyles.get(id);
            if (s != NULL)
                return s;

            // Create style
            s = new Style(this);
            if (s == NULL)
                return NULL;

            // Initialize style
            if (init != NULL)
            {
                if (init->init(s) != STATUS_OK)
                {
                    delete s;
                    return NULL;
                }
            }

            // Bind root style as parent automatically
            if (pRoot != NULL)
            {
                if (s->add_parent(pRoot) != STATUS_OK)
                {
                    delete s;
                    return NULL;
                }
            }

            // Register style
            if (!vStyles.create(id, s))
            {
                delete s;
                return NULL;
            }

            // Return style
            return s;
        }

        lsp::Color *Schema::color(const char *id)
        {
            LSPString tmp;
            if (!tmp.set_utf8(id))
                return NULL;
            return color(&tmp);
        }

        lsp::Color *Schema::color(const LSPString *id)
        {
            return vColors.get(id);
        }

        atom_t Schema::atom_id(const char *name) const
        {
            return pAtoms->atom_id(name);
        }

        atom_t Schema::atom_id(const LSPString *name) const
        {
            return pAtoms->atom_id(name);
        }

        const char *Schema::atom_name(atom_t id) const
        {
            return pAtoms->atom_name(id);
        }
    } /* namespace tk */
} /* namespace lsp */
