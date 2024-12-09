/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 нояб. 2020 г.
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
        StyleSheet::style_t::style_t()
        {
            order_gen       = 0;
        }

        StyleSheet::style_t::~style_t()
        {
            // Destroy parents
            for (size_t i=0, n=parents.size(); i<n; ++i)
            {
                LSPString *name = parents.uget(i);
                if (name != NULL)
                    delete name;
            }
            parents.flush();

            // Destroy properties
            lltl::parray<property_t> vp;
            properties.values(&vp);
            properties.flush();

            for (size_t i=0, n=vp.size(); i<n; ++i)
            {
                property_t *p = vp.uget(i);
                if (p != NULL)
                    delete p;
            }
            vp.flush();
        }

        StyleSheet::StyleSheet()
        {
            pRoot       = NULL;
        }

        StyleSheet::~StyleSheet()
        {
            // Delete root style
            if (pRoot != NULL)
            {
                delete pRoot;
                pRoot = NULL;
            }

            // Delete colors
            lltl::parray<lsp::Color> vc;
            vColors.values(&vc);
            vColors.flush();
            for (size_t i=0, n=vc.size(); i<n; ++i)
            {
                lsp::Color *c = vc.uget(i);
                if (c != NULL)
                    delete c;
            }
            vc.flush();

            // Delete styles
            lltl::parray<style_t> vs;
            vStyles.values(&vs);
            vStyles.flush();
            for (size_t i=0, n=vs.size(); i<n; ++i)
            {
                style_t *s = vs.uget(i);
                if (s != NULL)
                    delete s;
            }
            vs.flush();

            // Delete fonts
            lltl::parray<font_t> vf;
            vFonts.values(&vf);
            vFonts.flush();
            for (size_t i=0, n=vf.size(); i<n; ++i)
            {
                font_t *f = vf.uget(i);
                if (f != NULL)
                    delete f;
            }
            vf.flush();

            // Delete constants
            lltl::parray<LSPString> vv;
            vConstants.values(&vv);
            vConstants.flush();
            for (size_t i=0, n=vv.size(); i<n; ++i)
            {
                LSPString *s = vv.uget(i);
                if (s != NULL)
                    delete s;
            }
            vv.flush();
        }

        status_t StyleSheet::parse_document(xml::PullParser *p)
        {
            status_t item, res = STATUS_OK;
            bool read = false;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                    return -item;

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
                            sError.set_ascii("Root tag should be 'schema'");
                            return STATUS_CORRUPTED;
                        }

                        if ((res = parse_schema(p)) != STATUS_OK)
                            return res;

                        read = true;
                        break;

                    default:
                    {
                        sError.set_ascii("parse_document: Unexpected XML element");
                        return STATUS_CORRUPTED;
                    }
                }
            }

            if ((read) && (res == STATUS_OK))
                res = validate();

            return (read) ? res : STATUS_CORRUPTED;
        }

        status_t StyleSheet::parse_schema(xml::PullParser *p)
        {
            status_t item, res = STATUS_OK;
            enum
            {
                F_COLORS        = 1 << 0,
                F_FONTS         = 1 << 1,
                F_ROOT          = 1 << 2,
                F_META          = 1 << 3,
                F_CONST         = 1 << 4
            };
            size_t flags = 0;

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
                        const LSPString *name = p->name();
                        if (name->equals_ascii("colors"))
                        {
                            if (flags & F_COLORS)
                            {
                                sError.set_ascii("Duplicate element 'colors'");
                                return STATUS_BAD_FORMAT;
                            }
                            res     = parse_colors(p);
                            flags  |= F_COLORS;
                        }
                        else if (name->equals_ascii("fonts"))
                        {
                            if (flags & F_FONTS)
                            {
                                sError.set_ascii("Duplicate element 'fonts'");
                                return STATUS_BAD_FORMAT;
                            }
                            res = parse_fonts(p);
                            flags  |= F_FONTS;
                        }
                        else if (name->equals_ascii("constants"))
                        {
                            if (flags & F_CONST)
                            {
                                sError.set_ascii("Duplicate element 'constants'");
                                return STATUS_BAD_FORMAT;
                            }
                            res = parse_constants(p);
                            flags  |= F_CONST;
                        }
                        else if (name->equals_ascii("style"))
                            res = parse_style(p, false);
                        else if (name->equals_ascii("root"))
                            res = parse_style(p, true);
                        else if (name->equals_ascii("meta"))
                        {
                            if (flags & F_META)
                            {
                                sError.set_ascii("Duplicate element 'meta'");
                                return STATUS_BAD_FORMAT;
                            }
                            res = parse_metadata(p);
                            flags  |= F_META;
                        }
                        else
                        {
                            sError.fmt_utf8("Unsupported element: '%s'", name->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        if (!p->name()->equals_ascii("schema"))
                        {
                            sError.fmt_utf8("Unexpected end element: '%s'", p->name()->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        return STATUS_OK;

                    default:
                        sError.fmt_utf8("parse_schema: Unexpected XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_metadata(xml::PullParser *p)
        {
            status_t item, res = STATUS_OK;
            enum
            {
                F_TITLE         = 1 << 0
            };
            size_t flags = 0;

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
                        if (p->name()->equals_ascii("title"))
                        {
                            if (flags & F_TITLE)
                            {
                                sError.set_ascii("Duplicate element 'title'");
                                return STATUS_DUPLICATED;
                            }
                            flags |= F_TITLE;
                            res = parse_string_value(p, &sTitle);
                        }
                        else
                        {
                            sError.fmt_utf8("Unsupported element: '%s'", p->name()->get_utf8());
                            return STATUS_CORRUPTED;
                        }

                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_metadata: Unexpected XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_string_value(xml::PullParser *p, LSPString *value)
        {
            status_t item, res = STATUS_OK;
            bool set = false;

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
                        if (set)
                        {
                            sError.fmt_utf8("The value has already been set");
                            return STATUS_BAD_FORMAT;
                        }
                        set     = true;

                        // Parse value
                        if (p->name()->equals_ascii("value"))
                            res = (value->set(p->value())) ? STATUS_OK : STATUS_NO_MEM;
                        else
                        {
                            sError.fmt_utf8("Unknown attribute '%s'", p->name()->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        break;

                    case xml::XT_END_ELEMENT:
                        if (!set)
                        {
                            sError.fmt_utf8("Not specified value for string property '%s'", p->name()->get_utf8());
                            return STATUS_BAD_FORMAT;
                        }
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_string_value: Unsupported XML document");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_colors(xml::PullParser *p)
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
                        if (vColors.exists(p->name()))
                        {
                            sError.fmt_utf8("Duplicated color name: '%s'", p->name()->get_utf8());
                            return STATUS_DUPLICATED;
                        }

                        // Create color object
                        lsp::Color *c = new lsp::Color();
                        if (c == NULL)
                            return STATUS_NO_MEM;

                        LSPString color_name;
                        if (!color_name.set(p->name()))
                        {
                            delete c;
                            return STATUS_NO_MEM;
                        }

                        // Try to parse color
                        if ((res = parse_color(p, &color_name, c)) == STATUS_OK)
                        {
                            if (!vColors.put(&color_name, c, NULL))
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
                        sError.set_ascii("parse_colors: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_constants(xml::PullParser *p)
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
                        if (vConstants.exists(p->name()))
                        {
                            sError.fmt_utf8("Duplicated constant name: '%s'", p->name()->get_utf8());
                            return STATUS_DUPLICATED;
                        }

                        // Create color object
                        LSPString const_name;
                        if (!const_name.set(p->name()))
                            return STATUS_NO_MEM;
                        LSPString *value = new LSPString();
                        if (value == NULL)
                            return STATUS_NO_MEM;

                        // Try to parse color
                        if ((res = parse_constant(p, value)) == STATUS_OK)
                        {
                            if (!vConstants.put(&const_name, value, NULL))
                                res = STATUS_NO_MEM;
                        }

                        // Drop color on error
                        if (res != STATUS_OK)
                            delete value;
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_constants: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_fonts(xml::PullParser *p)
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
                        // Check that font has not been previously defined
                        if (vFonts.exists(p->name()))
                        {
                            sError.fmt_utf8("Duplicated font name: '%s'", p->name()->get_utf8());
                            return STATUS_DUPLICATED;
                        }

                        // Create font object
                        LSPString font_name;
                        if (!font_name.set(p->name()))
                            return STATUS_NO_MEM;
                        font_t *f = new font_t();
                        if (f == NULL)
                            return STATUS_NO_MEM;
                        f->alias    = false;
                        if (!f->name.set(p->name()))
                        {
                            delete f;
                            return STATUS_NO_MEM;
                        }

                        // Try to parse font
                        if ((res = parse_font(p, f)) == STATUS_OK)
                        {
                            if (!vFonts.put(&font_name, f, NULL))
                                res = STATUS_NO_MEM;
                        }

                        // Drop color on error
                        if (res != STATUS_OK)
                            delete f;
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_fonts: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_style(xml::PullParser *p, bool root)
        {
            status_t item, res = STATUS_OK;
            bool bParents = false;
            bool bClass = false;

            if ((root) && (pRoot != NULL))
            {
                sError.set_ascii("Duplicated root style definition");
                return STATUS_DUPLICATED;
            }

            LSPString sClass, name;
            style_t *style = new style_t();
            if (style == NULL)
                return STATUS_NO_MEM;

            while (true)
            {
                if ((item = p->read_next()) < 0)
                {
                    delete style;
                    return -item;
                }

                switch (item)
                {
                    case xml::XT_CHARACTERS:
                    case xml::XT_COMMENT:
                        break;

                    case xml::XT_START_ELEMENT:
                    {
                        if (!name.set(p->name()))
                        {
                            res     = STATUS_NO_MEM;
                            break;
                        }

                        res     = parse_property(p, style, &name);
                        break;
                    }

                    case xml::XT_ATTRIBUTE:
                        if (p->name()->equals_ascii("class"))
                        {
                            if ((root) || (bClass))
                            {
                                res     = STATUS_BAD_FORMAT;
                                break;
                            }
                            bClass  = true;
                            res     = parse_style_class(&sClass, p->value());
                        }
                        else if (p->name()->equals_ascii("parents"))
                        {
                            if ((root) || (bParents))
                            {
                                res     = STATUS_BAD_FORMAT;
                                break;
                            }
                            bParents = true;
                            res     = parse_style_parents(style, p->value());
                        }
                        else
                            res     = STATUS_BAD_FORMAT;
                        break;

                    case xml::XT_END_ELEMENT:
                        // Post-conditions
                        if (!root)
                        {
                            if (sClass.is_empty())
                            {
                                sError.set_ascii("Not defined style class");
                                res = STATUS_BAD_FORMAT;
                            }
                            else if (vStyles.exists(&sClass))
                            {
                                sError.fmt_utf8("Style with class '%s' redefinition", sClass.get_utf8());
                                res = STATUS_DUPLICATED;
                            }
                            else if (!vStyles.put(&sClass, style, NULL))
                                res = STATUS_NO_MEM;
                            else if (!style->name.set(&sClass))
                                res = STATUS_NO_MEM;
                        }

                        if (res == STATUS_OK)
                        {
                            if (root)
                                pRoot   = style;
                            return STATUS_OK;
                        }
                        break;

                    default:
                        sError.set_ascii("parse_style: Unsupported XML element");
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

        status_t StyleSheet::parse_color(xml::PullParser *p, const LSPString *color_name, lsp::Color *color)
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
                    {
                        // Value already has been set?
                        if (value)
                        {
                            sError.fmt_utf8("Color value has already been set");
                            return STATUS_BAD_FORMAT;
                        }
                        value   = true;

                        // Parse value
                        const LSPString *name  = p->name();
                        const LSPString *value = p->value();
                        if (name->equals_ascii("value"))
                            res = color->parse3(value);
                        else if (name->equals_ascii("avalue"))
                            res = color->parse4(value);
                        else if (name->equals_ascii("rgb"))
                            res = color->parse_rgb(value);
                        else if (name->equals_ascii("rgba"))
                            res = color->parse_rgba(value);
                        else if (name->equals_ascii("hsl"))
                            res = color->parse_hsl(value);
                        else if (name->equals_ascii("hsla"))
                            res = color->parse_hsla(value);
                        else
                        {
                            sError.fmt_utf8("Unknown property '%s' for color", name->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        if (res != STATUS_OK)
                        {
                            sError.fmt_utf8("Could not assign value %s='%s' to color '%s'",
                                name->get_utf8(), value->get_utf8(), color_name->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        break;
                    }

                    case xml::XT_END_ELEMENT:
                        if (!value)
                        {
                            sError.fmt_utf8("Not specified value for color '%s'", color_name->get_utf8());
                            return STATUS_BAD_FORMAT;
                        }
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_color: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_constant(xml::PullParser *p, LSPString *value)
        {
            status_t item, res = STATUS_OK;
            bool set = false;

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
                        if (set)
                        {
                            sError.fmt_utf8("Value has already been set");
                            return STATUS_BAD_FORMAT;
                        }
                        set = true;

                        // Parse value
                        if (p->name()->equals_ascii("value"))
                            res = (value->set(p->value())) ? STATUS_OK : STATUS_NO_MEM;
                        else
                        {
                            sError.fmt_utf8("Unknown property '%s' for constant", p->name()->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        break;

                    case xml::XT_END_ELEMENT:
                        if (!value)
                        {
                            sError.fmt_utf8("Not specified value for constant '%s'", p->name()->get_utf8());
                            return STATUS_BAD_FORMAT;
                        }
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_constant: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }


        status_t StyleSheet::parse_font(xml::PullParser *p, font_t *font)
        {
            status_t item;
            enum {
                LC_FLAG_SRC     = 1 << 0,
                LC_FLAG_ALIAS   = 1 << 1
            };
            size_t flags = 0;

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
                        // Parse value
                        if (p->name()->equals_ascii("src"))
                        {
                            if (flags)
                            {
                                sError.fmt_utf8("Can not set simultaneously alias and resource location for font '%s'", font->name.get_utf8());
                                return STATUS_BAD_FORMAT;
                            }
                            flags  |= LC_FLAG_SRC;
                            if (!font->path.set(p->value()))
                                return STATUS_NO_MEM;
                            font->alias = false;
                        }
                        else if (p->name()->equals_ascii("alias"))
                        {
                            if (flags)
                            {
                                sError.fmt_utf8("Can not set simultaneously alias and resource location for font '%s'", font->name.get_utf8());
                                return STATUS_BAD_FORMAT;
                            }
                            flags  |= LC_FLAG_ALIAS;
                            if (!font->path.set(p->value()))
                                return STATUS_NO_MEM;
                            font->alias = true;
                        }
                        else
                        {
                            sError.fmt_utf8("Unknown property '%s' for font", p->name()->get_utf8());
                            return STATUS_CORRUPTED;
                        }
                        break;

                    case xml::XT_END_ELEMENT:
                        if (!flags)
                        {
                            sError.fmt_utf8("Location of font file or alias should be defined for font '%s'", p->name()->get_utf8());
                            return STATUS_BAD_FORMAT;
                        }
                        return STATUS_OK;

                    default:
                        sError.set_ascii("parse_font: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }
            }
        }

        status_t StyleSheet::parse_property(xml::PullParser *p, style_t *style, const LSPString *name)
        {
            status_t item, res = STATUS_OK;
            bool bValue = false;
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
                            {
                                sError.fmt_utf8("Duplicate 'value' attribute for property '%s' of style '%s'", name->get_utf8(), style->name.get_utf8());
                                return STATUS_BAD_FORMAT;
                            }
                            bValue = true;
                            if (!value.set(p->value()))
                                return STATUS_NO_MEM;
                        }
                        else
                        {
                            sError.fmt_utf8("Invalid '%s' attribute for property '%s' of style '%s'", p->name()->get_utf8(), name->get_utf8(), style->name.get_utf8());
                            res = STATUS_BAD_FORMAT;
                        }
                        break;

                    case xml::XT_END_ELEMENT:
                    {
                        // Ensure that value has been set
                        if (!bValue)
                        {
                            sError.fmt_utf8("Not defined value for property '%s' of style '%s'", name->get_utf8(), style->name.get_utf8());
                            return STATUS_BAD_FORMAT;
                        }

                        // Parse value according to the specified type
                        if (style->properties.contains(name))
                        {
                            sError.fmt_utf8("Duplicate property '%s' for style '%s'", name->get_utf8(), style->name.get_utf8());
                            return STATUS_DUPLICATED;
                        }

                        // Create property
                        property_t **dst    = style->properties.create(name);
                        if (dst == NULL)
                        {
                            sError.fmt_utf8("Could not register property '%s' for style '%s'", name->get_utf8(), style->name.get_utf8());
                            return STATUS_NO_MEM;
                        }

                        property_t *prop = new property_t;
                        if (prop == NULL)
                        {
                            sError.fmt_utf8("Could not allocate property '%s' for style '%s'", name->get_utf8(), style->name.get_utf8());
                            return STATUS_NO_MEM;
                        }
                        lsp_finally {
                            if (prop != NULL)
                                delete prop;
                        };
                        prop->order     = style->order_gen++;
                        if (!prop->value.set(&value))
                        {
                            sError.fmt_utf8("Could not copy value of property '%s' for style '%s'", name->get_utf8(), style->name.get_utf8());
                            return STATUS_NO_MEM;
                        }
                        *dst            = release_ptr(prop);

                        return res;
                    }

                    default:
                        sError.set_ascii("parse_property: Unsupported XML element");
                        return STATUS_CORRUPTED;
                }

                if (res != STATUS_OK)
                    return res;
            }
        }

        status_t StyleSheet::parse_style_class(LSPString *cname, const LSPString *text)
        {
            if (!cname->set(text))
                return STATUS_NO_MEM;

            cname->trim();
            if (cname->length() <= 0)
                return STATUS_BAD_FORMAT;

            for (size_t i=0, n=cname->length(); i<n; ++i)
            {
                lsp_wchar_t ch = cname->char_at(i);
                if ((ch >= 'a') && (ch <= 'z'))
                    continue;
                if ((ch >= 'A') && (ch <= 'Z'))
                    continue;
                if ((ch >= '0') && (ch <= '9'))
                    continue;
                switch (ch)
                {
                    case '_':
                    case ':':
                    case '.':
                        break;
                    default:
                        return STATUS_BAD_FORMAT;
                }
            }

            return STATUS_OK;
        }

        status_t StyleSheet::add_parent(style_t *style, const LSPString *text)
        {
            LSPString cname;
            status_t res = parse_style_class(&cname, text);
            if (res != STATUS_OK)
                return res;

            // Check for duplicates
            for (size_t i=0, n=style->parents.size(); i<n; ++i)
                if (cname.equals(style->parents.uget(i)))
                {
                    sError.fmt_utf8("Duplicate parent style '%s' for style '%s'", cname.get_utf8(), style->name.get_utf8());
                    return STATUS_DUPLICATED;
                }

            // Add item to list
            LSPString *parent = cname.clone();
            if (parent == NULL)
                return STATUS_NO_MEM;

            if (!style->parents.add(parent))
            {
                delete parent;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        status_t StyleSheet::parse_style_parents(style_t *style, const LSPString *text)
        {
            LSPString cname;
            status_t res;
            ssize_t first = 0, last = -1, len = text->length();

            while (true)
            {
                last = text->index_of(first, ',');
                if (last < 0)
                {
                    last = len;
                    break;
                }

                if (!cname.set(text, first, last))
                    return STATUS_NO_MEM;
                if ((res = add_parent(style, &cname)) != STATUS_OK)
                    return res;

                first = last + 1;
            }

            // Last token pending?
            if (last > first)
            {
                if (!cname.set(text, first, last))
                    return STATUS_NO_MEM;
                if ((res = add_parent(style, &cname)) != STATUS_OK)
                    return res;
            }

            // Check that styles are defined
            if (style->parents.is_empty())
            {
                sError.fmt_utf8("Empty list of parents specified for style '%s'", style->name.get_utf8());
                return STATUS_NO_DATA;
            }

            return STATUS_OK;
        }

        status_t StyleSheet::parse_property_type(property_type_t *pt, const LSPString *text)
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
            {
                sError.fmt_utf8("Invalid property type: '%s'", text->get_utf8());
                return STATUS_BAD_FORMAT;
            }

            return (tok.get_token(expr::TF_GET) == expr::TT_EOF) ? STATUS_OK : STATUS_BAD_FORMAT;
        }

        status_t StyleSheet::parse_file(const char *path, const char *charset)
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

        status_t StyleSheet::parse_file(const LSPString *path, const char *charset)
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

        status_t StyleSheet::parse_file(const io::Path *path, const char *charset)
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

        status_t StyleSheet::parse_data(io::IInStream *is, size_t flags, const char *charset)
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

        status_t StyleSheet::parse_data(const char *str, const char *charset)
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

        status_t StyleSheet::parse_data(const LSPString *str)
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

        status_t StyleSheet::parse_data(io::IInSequence *seq, size_t flags)
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

        status_t StyleSheet::enum_colors(lltl::parray<LSPString> *names) const
        {
            return (vColors.keys(names)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_fonts(lltl::parray<LSPString> *names) const
        {
            return (vFonts.keys(names)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_styles(lltl::parray<LSPString> *names) const
        {
            return (vStyles.keys(names)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_constants(lltl::parray<LSPString> *names) const
        {
            return (vConstants.keys(names)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_properties(const LSPString *style, lltl::parray<LSPString> *names) const
        {
            style_t *s = (style != NULL) ? vStyles.get(style) : pRoot;
            if (s == NULL)
                return STATUS_NOT_FOUND;
            return (s->properties.keys(names)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_properties(const char *style, lltl::parray<LSPString> *names) const
        {
            if (style == NULL)
                return enum_properties(static_cast<LSPString *>(NULL), names);
            LSPString name;
            return (name.set_utf8(style)) ? enum_properties(&name, names) : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_parents(const LSPString *style, lltl::parray<LSPString> *names) const
        {
            style_t *s = (style != NULL) ? vStyles.get(style) : pRoot;
            if (s == NULL)
                return STATUS_NOT_FOUND;

            return (names->add(s->parents)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t StyleSheet::enum_parents(const char *style, lltl::parray<LSPString> *names) const
        {
            if (style == NULL)
                return enum_parents(static_cast<LSPString *>(NULL), names);
            LSPString name;
            return (name.set_utf8(style)) ? enum_parents(&name, names) : STATUS_NO_MEM;
        }

        ssize_t StyleSheet::get_property(const LSPString *style, const LSPString *property, LSPString *dst) const
        {
            style_t *s = (style != NULL) ? vStyles.get(style) : pRoot;
            if (s == NULL)
                return STATUS_NOT_FOUND;
            property_t *prop = s->properties.get(property);
            if (prop == NULL)
                return STATUS_NOT_FOUND;

            return (dst->set(&prop->value)) ? STATUS_OK : STATUS_NO_MEM;
        }

        ssize_t StyleSheet::get_property(const char *style, const char *property, LSPString *dst) const
        {
            LSPString s, p;
            if (!p.set_utf8(property))
                return STATUS_NO_MEM;

            if (style == NULL)
                return get_property(static_cast<LSPString *>(NULL), &p, dst);
            else if (!s.set_utf8(style))
                return STATUS_NO_MEM;

            return get_property(&s, &p, dst);
        }

        ssize_t StyleSheet::get_color(const LSPString *color, lsp::Color *dst) const
        {
            lsp::Color *xcol = vColors.get(color);
            if (xcol == NULL)
                return STATUS_NOT_FOUND;

            dst->copy(xcol);
            return STATUS_OK;
        }

        ssize_t StyleSheet::get_color(const char *color, lsp::Color *dst) const
        {
            LSPString c;
            if (!c.set_utf8(color))
                return STATUS_NOT_FOUND;
            return get_color(&c, dst);
        }

        ssize_t StyleSheet::get_constant(const LSPString *name, LSPString *dst) const
        {
            LSPString *value = vConstants.get(name);
            if (value == NULL)
                return STATUS_NOT_FOUND;

            return (dst->set(value)) ? STATUS_OK : STATUS_NO_MEM;
        }

        ssize_t StyleSheet::get_constant(const char *name, LSPString *dst) const
        {
            LSPString n;
            if (!n.set_utf8(name))
                return STATUS_NOT_FOUND;
            return get_constant(&n, dst);
        }

        ssize_t StyleSheet::get_font(const LSPString *font, LSPString *path, bool *alias) const
        {
            font_t *f = vFonts.get(font);
            if (f == NULL)
                return STATUS_NOT_FOUND;

            if (path != NULL)
            {
                if (!path->set(&f->path))
                    return STATUS_NO_MEM;
            }
            if (alias != NULL)
                *alias = f->alias;

            return STATUS_OK;
        }

        ssize_t StyleSheet::get_font(const char *font, LSPString *path, bool *alias) const
        {
            LSPString c;
            if (!c.set_utf8(font))
                return STATUS_NOT_FOUND;
            return get_font(&c, path, alias);
        }

        status_t StyleSheet::validate()
        {
            if (pRoot != NULL)
            {
                if (!pRoot->parents.is_empty())
                {
                    sError.set_ascii("Root style can not have parents");
                    return STATUS_BAD_HIERARCHY;
                }
            }

            lltl::parray<style_t> vs;
            if (!vStyles.values(&vs))
                return STATUS_NO_MEM;

            for (size_t i=0, n=vs.size(); i<n; ++i)
            {
                // Add style to the path
                style_t *s = vs.uget(i);
                status_t res = validate_style(s);
                if (res != STATUS_OK)
                    return res;
            }

            return STATUS_OK;
        }

        void StyleSheet::drop_paths(lltl::parray<path_t> *paths)
        {
            for (size_t i=0, n=paths->size(); i<n; ++i)
            {
                path_t *p = paths->uget(i);
                if (p != NULL)
                    delete p;
            }
            paths->flush();
        }

        status_t StyleSheet::validate_style(style_t *s)
        {
            lltl::parray<path_t> stack;
            path_t *p = new path_t();
            if (p == NULL)
                return STATUS_NO_MEM;
            else if (!stack.push(p))
            {
                delete p;
                return STATUS_NO_MEM;
            }
            p->curr = s;

            while (!stack.is_empty())
            {
                // Get current path
                p = stack.pop();
                if (p == NULL)
                {
                    drop_paths(&stack);
                    return STATUS_UNKNOWN_ERR;
                }

                // There is nothing to check?
                s = p->curr;
                if (s->parents.is_empty())
                {
                    delete p;
                    continue;
                }

                // Add current style to list of visited
                if (!p->visited.add(s))
                {
                    delete p;
                    drop_paths(&stack);
                    return STATUS_NO_MEM;
                }

//                lsp_trace("Curr: %p (%s)", s, s->name.get_utf8());
//                for (size_t i=0, n=p->visited.size(); i<n; ++i)
//                {
//                    style_t *xp = p->visited.uget(i);
//                    lsp_trace("  visited: %p (%s)", xp, xp->name.get_utf8());
//                }

                for (size_t i=0, n=s->parents.size(); i<n; )
                {
                    // Obtain the parent style
                    LSPString *name = s->parents.uget(i++);
                    style_t *ps = ((name == NULL) || (name->equals_ascii("root"))) ? pRoot : vStyles.get(name);

//                    lsp_trace("  testing: %p (%s), i=%d, n=%d", ps, (ps != NULL) ? ps->name.get_utf8() : "???", int(i), int(n));
                    if ((ps == NULL) || (p->visited.contains(ps)))
                    {
                        sError.fmt_utf8("Unexisting style found in tree: '%s'", (name != NULL) ? name->get_utf8() : "root");
                        delete p;
                        drop_paths(&stack);
                        return STATUS_BAD_HIERARCHY;
                    }

                    // Check next step
                    path_t *np = p;
                    if (i < n)
                    {
                        np = new path_t();
                        if (np == NULL)
                        {
                            drop_paths(&stack);
                            delete p;
                            return STATUS_NO_MEM;
                        }

                        if (!np->visited.add(p->visited)) // Copy list of visited
                        {
                            delete np;
                            delete p;
                            drop_paths(&stack);
                            return STATUS_NO_MEM;
                        }
                    }

                    np->curr    = ps;

                    // Add path to stack
                    if (!stack.push(np))
                    {
                        delete np;
                        drop_paths(&stack);
                        return STATUS_NO_MEM;
                    }
                }
            }

            return STATUS_OK;
        }

    } /* namespace tk */
} /* namespace lsp */



