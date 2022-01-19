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
                sFontScaling.bind("font.scaling", this);
                sFont.bind("font", this);
                sDrawMode.bind("draw.mode", this);
                // Configure
                sFont.set_antialiasing(ws::FA_DEFAULT);
                sFont.set_size(12.0f);
                sFontScaling.set(1.0f);
                sScaling.set(1.0f);
                sDrawMode.set(DM_CLASSIC);
            LSP_TK_STYLE_IMPL_END

            LSP_SYMBOL_HIDDEN
            StyleFactory<Root> RootFactory(NULL, NULL);
        }

        Schema::Schema(Atoms *atoms, Display *dpy)
        {
            pAtoms          = atoms;
            pDisplay        = dpy;
            nFlags          = 0;
            pRoot           = NULL;
        }
    
        Schema::~Schema()
        {
            // Manually unbind all properties before destroying context
            sScaling.unbind();
            sFontScaling.unbind();
            sFont.unbind();
            sDrawMode.unbind();

            // Destroy named styles
            vBuiltin.flush();
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

        status_t Schema::init_colors_from_sheet(const StyleSheet *sheet)
        {
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

            return STATUS_OK;
        }

        status_t Schema::load_fonts_from_sheet(const StyleSheet *sheet, resource::ILoader *loader)
        {
            status_t res;
            lltl::parray<LSPString> vk;
            sheet->enum_fonts(&vk);

            ws::IDisplay *dpy = pDisplay->display();
            if (dpy == NULL)
                return STATUS_BAD_STATE;

            for (size_t i=0, n=vk.size(); i<n; ++i)
            {
                LSPString *key              = vk.uget(i);
                StyleSheet::font_t *font    = sheet->vFonts.get(key);
                if ((key == NULL) || (font == NULL))
                    return STATUS_BAD_STATE;

                if (font->alias)
                {
                    if ((res = dpy->add_font_alias(font->name.get_utf8(), font->path.get_utf8())) != STATUS_OK)
                    {
                        lsp_error("Could not create font alias \"%s\" -> \"%s\"",
                            font->name.get_utf8(),
                            font->path.get_utf8()
                        );
                        return res;
                    }
                }
                else if ((loader != NULL) || (pDisplay->pResourceLoader != NULL))
                {
                    // Patch the loader (if not specified)
                    if (loader == NULL)
                        loader  = pDisplay->pResourceLoader;

                    // Use resource resolver for loading fonts
                    io::IInStream *is = loader->read_stream(&font->path);
                    if (is == NULL)
                    {
                        lsp_error("Could not resolve font data \"%s\" located at \"%s\", error code %d",
                            font->name.get_utf8(),
                            font->path.get_utf8(),
                            int(loader->last_error())
                        );
                        return loader->last_error();
                    }

                    if ((res = dpy->add_font(font->name.get_utf8(), is)) != STATUS_OK)
                    {
                        lsp_error("Could not load font data \"%s\" resolved at \"%s\", error code %d",
                            font->name.get_utf8(),
                            font->path.get_utf8(),
                            int(loader->last_error())
                        );
                        is->close();
                        delete is;
                        return res;
                    }

                    is->close();
                    delete is;
                }
                else
                {
                    // Just load font from file
                    if ((res = dpy->add_font(font->name.get_utf8(), font->path.get_utf8())) != STATUS_OK)
                    {
                        lsp_error("Could not load font \"%s\" located at \"%s\", error code %d",
                            font->name.get_utf8(),
                            font->path.get_utf8(),
                            int(res)
                        );
                        return res;
                    }
                }
            }

            return STATUS_OK;
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
                LSP_STATUS_ASSERT(create_builtin_style(list[i]));
            }

            // Unset 'configuring' mode
            nFlags &= ~S_CONFIGURING;

            return STATUS_OK;
        }

        status_t Schema::add(lltl::parray<IStyleFactory> *list)
        {
            return add(list->array(), list->size());
        }

        status_t Schema::add(lltl::parray<IStyleFactory> &list)
        {
            return add(list.array(), list.size());
        }

        status_t Schema::add(IStyleFactory **list, size_t n)
        {
            size_t old  = nFlags;
            nFlags     |= S_CONFIGURING;

            // Create all necessary styles
            for (size_t i=0; i<n; ++i)
            {
                LSP_STATUS_ASSERT(create_builtin_style(list[i]));
            }

            // Unset 'configuring' mode
            nFlags      = old;

            return STATUS_OK;
        }

        status_t Schema::add(IStyleFactory *factory)
        {
            size_t old  = nFlags;
            nFlags     |= S_CONFIGURING;

            // Create necessary style
            LSP_STATUS_ASSERT(create_builtin_style(factory));

            // Unset 'configuring' mode
            nFlags      = old;

            return STATUS_OK;
        }

        status_t Schema::apply(const StyleSheet *sheet, resource::ILoader *loader)
        {
            if (sheet == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Apply settings in configuration mode
            nFlags |= S_CONFIGURING;
            status_t res = apply_internal(sheet, loader);
            nFlags &= ~S_CONFIGURING;

            return res;
        }

        status_t Schema::create_missing_styles(const StyleSheet *sheet)
        {
            // List all possible styles sheet names
            status_t res;
            lltl::parray<LSPString>  vss;
            if (!sheet->vStyles.keys(&vss))
                return STATUS_NO_MEM;

            // Create missing styles and reset configured flag for all styles
            for (size_t i=0, n=vss.size(); i<n; ++i)
            {
                LSPString *name         = vss.uget(i);
                Style *s                = vStyles.get(name);
                if (s != NULL)
                    continue;

                // Create new unbound style
                if ((res = create_style(name)) != STATUS_OK)
                    return res;

            }

            return STATUS_OK;
        }

        status_t Schema::link_styles(const StyleSheet *sheet)
        {
            // List all possible styles sheet names
            status_t res;
            lltl::parray<LSPString>  vss;
            if (!vStyles.keys(&vss))
                return STATUS_NO_MEM;

            // For each style: link it to parents and reset the 'configured' flag
            for (size_t i=0, n=vss.size(); i<n; ++i)
            {
                LSPString *name         = vss.uget(i);
                Style *s                = vStyles.get(name);
                if (s == NULL)
                    continue;

                // Reset the 'configured' flag for styles
                s->set_configured(false);

                // Link to parents
                StyleSheet::style_t *xs = sheet->vStyles.get(name);
                if (xs != NULL)
                {
                    //lsp_trace("Linking style '%s'", name->get_utf8());
                    res = apply_relations(s, &xs->parents);
                }
                else
                {
                    const char *default_parents = s->default_parents();
                    if (default_parents == NULL)
                        default_parents = "root";
                    //lsp_trace("Linking style '%s' to default parents: '%s'", name->get_utf8(), default_parents);
                    res = apply_relations(s, default_parents);
                }

                if (res != STATUS_OK)
                    return res;
            }

            return STATUS_OK;
        }

        bool Schema::check_parents_configured(Style *s)
        {
            for (size_t i=0, n=s->parents(); i<n; ++i)
            {
                Style *sp = s->parent(i);
                if ((sp != NULL) && (!sp->configured()))
                    return false;
            }

            return true;
        }

        status_t Schema::configure_styles(const StyleSheet *sheet)
        {
            // List all possible styles sheet names
            status_t res;
            lltl::parray<LSPString> vss;
            if (!vStyles.keys(&vss))
                return STATUS_NO_MEM;

            // Initialize all styles in the order of their inheritance
            for (size_t i=0; vss.size() > 0; )
            {
                i                      %= vss.size();
                LSPString *name         = vss.uget(i);
                Style *s                = vStyles.get(name);

                // Style does not exists or is already configured?
                if ((s == NULL) || (s->configured()))
                {
                    vss.remove(i); // Remove the key
                    continue;
                }

                // Is there stylesheet present for this style?
                StyleSheet::style_t *xs = sheet->vStyles.get(name);
                if (xs == NULL)
                {
                    s->set_configured(true);
                    vss.remove(i); // Remove the key
                    continue;
                }

                // Check that parents of this style already have been configured
                if (check_parents_configured(s))
                {
                    //lsp_trace("Configuring style '%s'", name->get_utf8());
                    if ((res = apply_settings(s, xs)) != STATUS_OK)
                        return res;

                    s->set_configured(true);
                    vss.remove(i); // Remove the key
                    continue;
                }

                // Increment the position
                ++i;
            }

            return STATUS_OK;
        }

        status_t Schema::unlink_styles()
        {
            lltl::parray<Style> vs;
            if (!vStyles.values(&vs))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vs.size(); i<n; ++i)
            {
                Style *s = vs.uget(i);
                if (s != NULL)
                    s->remove_all_parents();
            }
            return STATUS_OK;
        }

        status_t Schema::apply_internal(const StyleSheet *sheet, resource::ILoader *loader)
        {
            status_t res;

            // Destroy all previously loaded and used fonts and apply new
            if (pDisplay != NULL)
            {
                pDisplay->display()->remove_all_fonts();
                load_fonts_from_sheet(sheet, loader);
            }

            // Destroy colors and copy colors from sheed
            destroy_colors();
            if ((res = init_colors_from_sheet(sheet)) != STATUS_OK)
                return res;

            // Destroy all relations between styles
            if ((res = unlink_styles()) != STATUS_OK)
                return res;

            // Create missing styles
            if ((res = create_missing_styles(sheet)) != STATUS_OK)
                return res;

            // Link root style and other styles
            //lsp_trace("Linking root style");
            if (sheet->pRoot != NULL)
            {
                if ((res = apply_relations(pRoot, &sheet->pRoot->parents)) != STATUS_OK)
                    return res;
            }
            if ((res = link_styles(sheet)) != STATUS_OK)
                return res;

            // Configure root style and others
            //lsp_trace("Configuring root style");
            if (sheet->pRoot != NULL)
            {
                if ((res = apply_settings(pRoot, sheet->pRoot)) != STATUS_OK)
                    return res;
                pRoot->set_configured(true);
            }
            if ((res = configure_styles(sheet)) != STATUS_OK)
                return res;

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

//                lsp_trace("  %s = %s [%d]",
//                    name->get_utf8(),
//                    value->get_utf8(),
//                    int(pAtoms->atom_id(name))
//                );

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

        status_t Schema::apply_relations(Style *s, const lltl::parray<LSPString> *parents)
        {
            status_t res;

            for (size_t i=0, n=parents->size(); i<n; ++i)
            {
                const LSPString *parent = parents->uget(i);
                Style *ps = (parent->equals_ascii("root")) ? pRoot : vStyles.get(parent);
                if (ps != NULL)
                {
//                    lsp_trace("  parent: %s", parent->get_utf8());
                    if ((res = s->add_parent(ps)) != STATUS_OK)
                        return res;
                }
            }

            return STATUS_OK;
        }

        status_t Schema::apply_relations(Style *s, const char *list)
        {
            status_t res;
            LSPString parent, text;
            if (!text.set_utf8(list))
                return STATUS_NO_MEM;

            // Parse list of
            ssize_t first = 0, last = -1, len = text.length();

            while (true)
            {
                last = text.index_of(first, ',');
                if (last < 0)
                {
                    last = len;
                    break;
                }

                if (!parent.set(&text, first, last))
                    return false;

                Style *ps = (parent.equals_ascii("root")) ? pRoot : vStyles.get(&parent);
                if (ps != NULL)
                {
//                    lsp_trace("  parent: %s", parent.get_utf8());
                    if ((res = s->add_parent(ps)) != STATUS_OK)
                        return res;
                }

                first = last + 1;
            }

            // Last token pending?
            if (last > first)
            {
                if (!parent.set(&text, first, last))
                    return false;

                Style *ps = (parent.equals_ascii("root")) ? pRoot : vStyles.get(&parent);
                if (ps != NULL)
                {
//                    lsp_trace("  parent: %s", parent.get_utf8());
                    if ((res = s->add_parent(ps)) != STATUS_OK)
                        return res;
                }
            }

            return STATUS_OK;
        }

        status_t Schema::create_builtin_style(IStyleFactory *init)
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
//            lsp_trace("Creating style '%s' with default parents '%s'...", init->name(), init->default_parents());
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

            // Register style in the list
            if (!vBuiltin.create(&name, style))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }

        status_t Schema::create_style(const LSPString *name)
        {
            // Duplicates are disallowed
            if (vStyles.contains(name))
            {
                lsp_warn("Duplicate style name: %s", name->get_native());
                return STATUS_ALREADY_EXISTS;
            }

            // Create style
//            lsp_trace("Creating style '%s'...", name->get_native());
            Style *style    = new Style(this, name->get_utf8(), "root");
            if (style == NULL)
                return STATUS_NO_MEM;

            // Register style in the list
            if (!vStyles.create(name, style))
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
            sFontScaling.bind("font.scaling", root);
            sFont.bind("font", root);
            sDrawMode.bind("draw.mode", root);
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
    
        Style *Schema::get(const char *id)
        {
            LSPString tmp;
            if (!tmp.set_utf8(id))
                return NULL;
            return get(&tmp);
        }

        Style *Schema::get(const LSPString *id)
        {
            // Check that style exists
            Style *s  = vStyles.get(id);
            if (s != NULL)
                return s;

            // Create style
            s = new Style(this, id->get_utf8(), NULL);
            if (s == NULL)
                return NULL;

            // Initialize style
            if (s->init() != STATUS_OK)
            {
                delete s;
                return NULL;
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

        status_t Schema::get_language(LSPString *dst) const
        {
            // Check state
            if (pRoot == NULL)
                return STATUS_BAD_STATE;

            // Get atom
            atom_t atom = pAtoms->atom_id(LSP_TK_PROP_LANGUAGE);
            if (atom < 0)
                return -atom;

            // Get the string value
            return pRoot->get_string(atom, dst);
        }

        status_t Schema::set_lanugage(const LSPString *lang)
        {
            // Check for arguments
            if (lang == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Check state
            if (pRoot == NULL)
                return STATUS_BAD_STATE;

            // Get atom
            atom_t atom = pAtoms->atom_id(LSP_TK_PROP_LANGUAGE);
            if (atom < 0)
                return -atom;

            // Get the string value
            return pRoot->set_string(atom, lang);
        }

        status_t Schema::set_lanugage(const char *lang)
        {
            // Check for arguments
            if (lang == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Check state
            if (pRoot == NULL)
                return STATUS_BAD_STATE;

            // Get atom
            atom_t atom = pAtoms->atom_id(LSP_TK_PROP_LANGUAGE);
            if (atom < 0)
                return -atom;

            // Get the string value
            return pRoot->set_string(atom, lang);
        }

        status_t Schema::add_font(const char *name, const char *path)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->add_font(name, path) : STATUS_BAD_STATE;
        }

        status_t Schema::add_font(const char *name, const io::Path *path)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->add_font(name, path) : STATUS_BAD_STATE;
        }

        status_t Schema::add_font(const char *name, const LSPString *path)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->add_font(name, path) : STATUS_BAD_STATE;
        }

        status_t Schema::add_font(const char *name, io::IInStream *is)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->add_font(name, is) : STATUS_BAD_STATE;
        }

        status_t Schema::add_font_alias(const char *name, const char *alias)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->add_font_alias(name, alias) : STATUS_BAD_STATE;
        }

        status_t Schema::remove_font(const char *name)
        {
            ws::IDisplay *dpy = pDisplay->display();
            return (dpy != NULL) ? dpy->remove_font(name) : STATUS_BAD_STATE;
        }

        void Schema::remove_all_fonts()
        {
            ws::IDisplay *dpy = pDisplay->display();
            if (dpy != NULL)
                dpy->remove_all_fonts();
        }
    } /* namespace tk */
} /* namespace lsp */
