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

namespace lsp
{
    namespace tk
    {
        Schema::style_t::style_t(Schema *schema):
            sStyle(schema)
        {
            bInitialized        = false;
            sStyle.init();
        }
        
        Schema::style_t::~style_t()
        {
            sStyle.destroy();
            for (size_t i=0, n=vParents.size(); i<n; ++i)
                delete vParents.uget(i);
            vParents.flush();
        }

        Schema::Schema(Atoms *atoms)
        {
            pAtoms      = atoms;
            init_context(&sCtx);
        }
        
        Schema::~Schema()
        {
            // Manually unbind all properties before destroying context
            sScaling.unbind();

            destroy_context(&sCtx);
        }

        void Schema::destroy_context(context_t *ctx)
        {
            // Destroy color aliases
            lltl::parray<lsp::Color> colors;
            if (ctx->vColors.values(&colors))
            {
                for (size_t i=0, n=colors.size(); i<n; ++i)
                    delete colors.uget(i);
                colors.flush();
            }
            ctx->vColors.flush();

            // Destroy styles
            lltl::parray<style_t> styles;
            if (ctx->vStyles.values(&styles))
            {
                for (size_t i=0, n=styles.size(); i<n; ++i)
                    delete styles.uget(i);
                styles.flush();
            }

            // Destroy root style
            if (ctx->pRoot != NULL)
            {
                delete ctx->pRoot;
                ctx->pRoot  = NULL;
            }
        }

        void Schema::init_context(context_t *s)
        {
            s->pRoot        = NULL;
        }

        void Schema::swap_context(context_t *a, context_t *b)
        {
            swap(a->pRoot, b->pRoot);
            a->vColors.swap(b->vColors);
            a->vStyles.swap(b->vStyles);
        }

        status_t Schema::parse_file(const char *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_file(const LSPString *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_file(const io::Path *path, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.open(path, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(io::IInStream *is, size_t flags, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(is, flags, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(const char *str, const char *charset)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(str, charset);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(const LSPString *str)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(str);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_data(io::IInSequence *seq, size_t flags)
        {
            xml::PullParser parser;
            status_t res = parser.wrap(seq, flags);
            if (res == STATUS_OK)
                res = parse_document(&parser);
            if (res == STATUS_OK)
                res = parser.close();
            else
                parser.close();
            return res;
        }

        status_t Schema::parse_document(xml::PullParser *p)
        {
            status_t item, res = STATUS_OK;
            bool read = false;

            context_t ctx;
            init_context(&ctx);

            while (true)
            {
                if ((item = p->read_next()) < 0)
                {
                    destroy_context(&ctx);
                    return -item;
                }

                if (item == xml::XT_END_DOCUMENT)
                    break;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                    case xml::XT_DTD:
                    case xml::XT_START_DOCUMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                        if ((read) || (p->name() == NULL) || (!p->name()->equals_ascii("schema")))
                        {
                            destroy_context(&ctx);
                            return STATUS_CORRUPTED;
                        }

                        if ((res = parse_schema(p, &ctx)) != STATUS_OK)
                        {
                            destroy_context(&ctx);
                            return res;
                        }

                        read = true;
                        break;

                    default:
                        destroy_context(&ctx);
                        return STATUS_CORRUPTED;
                }
            }

            if (!read)
                return STATUS_CORRUPTED;

            res = apply_context(&ctx);
            destroy_context(&ctx);

            return res;
        }

        status_t Schema::parse_schema(xml::PullParser *p, context_t *ctx)
        {
            status_t item, res = STATUS_OK;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                        if (p->name()->equals_ascii("colors"))
                            res = parse_colors(p, ctx);
                        else if (p->name()->equals_ascii("style"))
                            res = parse_style(p, ctx, false);
                        else if (p->name()->equals_ascii("root"))
                            res = parse_style(p, ctx, true);
                        else
                            return STATUS_CORRUPTED;
                        break;

                    case xml::XT_END_ELEMENT:
                        if (!p->name()->equals_ascii("schema"))
                            return STATUS_CORRUPTED;
                        return STATUS_OK;

                    default:
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t Schema::parse_colors(xml::PullParser *p, context_t *ctx)
        {
            status_t item, res = STATUS_OK;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        // Check that color has not been previously defined
                        if (ctx->vColors.exists(p->name()))
                            return STATUS_DUPLICATED;

                        // Create color object
                        lsp::Color *c = new lsp::Color();
                        if (c == NULL)
                            return STATUS_NO_MEM;

                        // Try to parse color
                        if ((res = parse_color(p, c)) == STATUS_OK)
                        {
                            if (!ctx->vColors.put(p->name(), c, NULL))
                                res = STATUS_NO_MEM;
                        }

                        // Drop color on error
                        if (res != STATUS_OK)
                            delete c;
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return STATUS_OK;

                    default:
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t Schema::parse_style(xml::PullParser *p, context_t *ctx, bool root)
        {
            status_t item, res = STATUS_OK;
            bool bParents = false;
            bool bClass = false;

            if ((root) && (ctx->pRoot != NULL))
                return STATUS_DUPLICATED;

            LSPString sClass;
            LSPString name;
            style_t *style = new style_t(this);
            if (style == NULL)
                return STATUS_NO_MEM;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        if (!name.set(p->name()))
                            res = STATUS_NO_MEM;
                        else
                            res = parse_property(p, style, &name);
                        break;
                    }

                    case xml::XT_ATTRIBUTE:
                        if (p->name()->equals_ascii("class"))
                        {
                            if ((root) || (bClass))
                                res = STATUS_BAD_FORMAT;
                            bClass = true;
                            res = parse_style_class(&sClass, p->value());
                        }
                        else if (p->name()->equals_ascii("parents"))
                        {
                            if ((root) || (bParents))
                                res = STATUS_BAD_FORMAT;
                            bParents = true;
                            res = parse_style_parents(style, p->value());
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case xml::XT_END_ELEMENT:
                        // Post-conditions
                        if (!root)
                        {
                            if (sClass.is_empty())
                                res = STATUS_BAD_FORMAT;
                            else if (ctx->vStyles.exists(&sClass))
                                res = STATUS_DUPLICATED;
                            else if (!ctx->vStyles.put(&sClass, style, NULL))
                                res = STATUS_NO_MEM;
                        }

                        if (res == STATUS_OK)
                        {
                            if (root)
                                ctx->pRoot = style;
                            return STATUS_OK;
                        }
                        break;

                    default:
                        res = STATUS_CORRUPTED;
                        break;
                }

                if (res != STATUS_OK)
                {
                    delete style;
                    return res;
                }
            }
        }

        status_t Schema::parse_color(xml::PullParser *p, lsp::Color *color)
        {
            status_t item, res = STATUS_OK;
            bool value = false;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_ATTRIBUTE:
                        // Value already has been set?
                        if (value)
                            return STATUS_BAD_FORMAT;
                        value   = true;

                        // Parse value
                        if (p->name()->equals_ascii("value"))
                            res = color->parse3(p->value());
                        else if (p->name()->equals_ascii("avalue"))
                            res = color->parse4(p->value());
                        else if (p->name()->equals_ascii("rgb"))
                            res = color->parse_rgb(p->value());
                        else if (p->name()->equals_ascii("rgba"))
                            res = color->parse_rgba(p->value());
                        else if (p->name()->equals_ascii("hsl"))
                            res = color->parse_hsl(p->value());
                        else if (p->name()->equals_ascii("hsla"))
                            res = color->parse_hsla(p->value());
                        else
                            return STATUS_CORRUPTED;
                        break;

                    case xml::XT_END_ELEMENT:
                        if (!value)
                            return STATUS_BAD_FORMAT;
                        return STATUS_OK;

                    default:
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t Schema::parse_property(xml::PullParser *p, style_t *style, const LSPString *name)
        {
            status_t item, res = STATUS_OK;

            bool bValue = false;
            property_type_t pt = PT_UNKNOWN;
            LSPString value;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_ATTRIBUTE:
                        if (p->name()->equals_ascii("value"))
                        {
                            if (bValue)
                                return STATUS_BAD_FORMAT;
                            bValue = true;
                            if (!value.set(p->value()))
                                return STATUS_NO_MEM;
                        }
                        else if (p->name()->equals_ascii("type"))
                        {
                            if (pt != PT_UNKNOWN)
                                return STATUS_BAD_FORMAT;
                            res = parse_property_type(&pt, p->value());
                        }
                        else
                            res = STATUS_BAD_FORMAT;
                        break;

                    case xml::XT_END_ELEMENT:
                    {
                        // Ensure that value has been set
                        if (!bValue)
                            return STATUS_BAD_FORMAT;

                        // Parse value according to the specified type
                        property_value_t v;
                        if ((res = parse_property_value(&v, &value, pt)) != STATUS_OK)
                            return res;
                        atom_t id = pAtoms->atom_id(name);
                        if (id < 0)
                            return STATUS_UNKNOWN_ERR;

                        // Deploy value to the style
                        if (v.type == PT_BOOL)
                            res = style->sStyle.set_bool(id, v.bvalue);
                        else if (v.type == PT_INT)
                            res = style->sStyle.set_int(id, v.ivalue);
                        else if (v.type == PT_FLOAT)
                            res = style->sStyle.set_float(id, v.fvalue);
                        else if (v.type == PT_STRING)
                            res = style->sStyle.set_string(id, &v.svalue);
                        else
                            res = STATUS_UNKNOWN_ERR;

                        return res;
                    }

                    default:
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t Schema::apply_context(context_t *ctx)
        {
            status_t res;

            // Check that there is root style
            if (ctx->pRoot == NULL)
                return STATUS_BAD_FORMAT;

            // Fetch the overall list of styles
            lltl::parray<LSPString> keys;
            if (!ctx->vStyles.keys(&keys))
                return STATUS_NO_MEM;

            // Perform style binding
            for (size_t i=0, n=keys.size(); i<n; ++i)
            {
                LSPString *id   = keys.uget(i);
                style_t *child  = ctx->vStyles.get(id);
                if (child == NULL)
                    return STATUS_CORRUPTED;

                // Lookup for each parent style in list and perform binding
                for (size_t j=0, m=child->vParents.size(); j<m; ++j)
                {
                    LSPString *pid = child->vParents.uget(j);
                    if (pid == NULL)
                        return STATUS_CORRUPTED;

                    // Fetch parent style
                    style_t *parent = (pid->equals_ascii("root")) ? ctx->pRoot : ctx->vStyles.get(pid);
                    if (parent == NULL)
                        return STATUS_CORRUPTED;

                    // Add parent style to the child style
                    if ((res = child->sStyle.add_parent(&parent->sStyle)) != STATUS_OK)
                        return res;
                }
            }

            // Swap the state
            swap_context(&sCtx, ctx);

            // Bind schema
            bind();

            return STATUS_OK;
        }

        void Schema::bind()
        {
            Style *root = (sCtx.pRoot != NULL) ? &sCtx.pRoot->sStyle : NULL;

            // Bind properties
            sScaling.bind("size.scaling", root);

            // Initialize default values
            sScaling.init(root, 1.0f);
        }

        status_t Schema::parse_style_class(LSPString *cname, const LSPString *text)
        {
            io::InStringSequence is(text);
            expr::Tokenizer tok(&is);

            if (tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS) != expr::TT_BAREWORD)
                return STATUS_BAD_FORMAT;
            else if (!cname->set(tok.text_value()))
                return STATUS_NO_MEM;

            return (tok.get_token(expr::TF_GET) == expr::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }

        status_t Schema::parse_style_parents(style_t *style, const LSPString *text)
        {
            io::InStringSequence is(text);
            expr::Tokenizer tok(&is);
            expr::token_t t;

            while ((t = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS)) != expr::TT_EOF)
            {
                // Require comma if there is more than one parent
                if (style->vParents.size() > 0)
                {
                    if (t != expr::TT_COMMA)
                        return STATUS_BAD_FORMAT;
                    t = tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS);
                }
                if (t != expr::TT_BAREWORD)
                    return STATUS_BAD_FORMAT;

                // Check for duplicates
                LSPString *parent = tok.text_value()->clone();
                if (parent == NULL)
                    return STATUS_NO_MEM;

                for (size_t i=0, n=style->vParents.size(); i<n; ++i)
                    if (tok.text_value()->equals(style->vParents.uget(i)))
                    {
                        delete parent;
                        return STATUS_DUPLICATED;
                    }

                // Add to list
                if (!style->vParents.add(parent))
                    return STATUS_NO_MEM;
            }

            return (style->vParents.is_empty()) ? STATUS_NO_DATA : STATUS_OK;
        }

        status_t Schema::parse_property_type(property_type_t *pt, const LSPString *text)
        {
            io::InStringSequence is(text);
            expr::Tokenizer tok(&is);

            if (tok.get_token(expr::TF_GET | expr::TF_XKEYWORDS) != expr::TT_BAREWORD)
                return STATUS_BAD_FORMAT;
            else
                text = tok.text_value();

            if ((text->equals_ascii_nocase("b")) ||
                (text->equals_ascii_nocase("bool")) ||
                (text->equals_ascii_nocase("boolean")))
                *pt = PT_BOOL;
            else if ((text->equals_ascii_nocase("i")) ||
                (text->equals_ascii_nocase("int")) ||
                (text->equals_ascii_nocase("integer")))
                *pt = PT_INT;
            else if ((text->equals_ascii_nocase("f")) ||
                (text->equals_ascii_nocase("float")))
                *pt = PT_FLOAT;
            else if ((text->equals_ascii_nocase("s")) ||
                (text->equals_ascii_nocase("t")) ||
                (text->equals_ascii_nocase("str")) ||
                (text->equals_ascii_nocase("text")) ||
                (text->equals_ascii_nocase("string")))
                *pt = PT_STRING;
            else
                return STATUS_BAD_FORMAT;

            return (tok.get_token(expr::TF_GET) == expr::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
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

        Schema::style_t *Schema::get_style(const LSPString *id)
        {
            style_t *s  = sCtx.vStyles.get(id);
            if (s == NULL)
            {
                // Get root style
                Style *r = root();
                if (r == NULL)
                    return NULL;

                // Create child style
                s = new style_t(this);
                if (s == NULL)
                    return NULL;

                // Add to style map
                if (!sCtx.vStyles.put(id, s, NULL))
                {
                    delete s;
                    return NULL;
                }

                // Bind to root
                s->sStyle.add_parent(r);
            }
            return s;
        }
    
        Style *Schema::root()
        {
            style_t *s = sCtx.pRoot;
            if (s == NULL)
            {
                s = new style_t(this);
                if (s == NULL)
                    return NULL;
                sCtx.pRoot  = s;

                // Bind schema
                bind();
            }

            return &s->sStyle;
        }

        Style *Schema::get(const char *id, style_init_t init, void *args)
        {
            LSPString tmp;
            if (!tmp.set_utf8(id))
                return NULL;
            return get(&tmp, init, args);
        }

        Style *Schema::get(const char *id, StyleInitializer *init)
        {
            LSPString tmp;
            if (!tmp.set_utf8(id))
                return NULL;
            return get(&tmp, init);
        }

        Style *Schema::get(const LSPString *id, style_init_t init, void *args)
        {
            style_t *s = get_style(id);

            // Check that style is initialized
            if ((s != NULL) && (!s->bInitialized) && (init != NULL))
            {
                init(&s->sStyle, args);
                s->bInitialized = true;
            }

            return &s->sStyle;
        }

        Style *Schema::get(const LSPString *id, StyleInitializer *init)
        {
            style_t *s = get_style(id);

            // Check that style is initialized
            if ((s != NULL) && (!s->bInitialized) && (init != NULL))
            {
                init->init(&s->sStyle);
                s->bInitialized = true;
            }

            return &s->sStyle;
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
            return sCtx.vColors.get(id);
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
