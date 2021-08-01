/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 21 сент. 2020 г.
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

namespace lsp
{
    namespace tk
    {
        void ColorRanges::Changes::notify(Property *prop)
        {
            if (bEnabled)
                pList->sync();
        }

        ColorRanges::ColorRanges(prop::Listener *listener):
            SimpleProperty(listener),
            sChanges(this)
        {
        }

        ColorRanges::~ColorRanges()
        {
            destroy_items(&vItems);
            SimpleProperty::unbind(&sListener);
        }

        status_t ColorRanges::build_ranges(LSPString *s)
        {
            char buf[32];

            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                ColorRange *cr          = vItems.uget(i);
                const lsp::Color *col   = cr->color();

                if (col->is_hsl())
                    col->format_hsla(buf, sizeof(buf)/sizeof(char));
                else
                    col->format_rgba(buf, sizeof(buf)/sizeof(char));

                if (i > 0)
                    s->fmt_append_utf8(", %.10f %.10f %s", cr->min(), cr->max(), buf);
                else
                    s->fmt_append_utf8("%.10f %.10f %s", cr->min(), cr->max(), buf);
            }

            return STATUS_OK;
        }

        void ColorRanges::push()
        {
            LSPString s;
            build_ranges(&s);

            // Min and max components
            if (nAtom >= 0)
                pStyle->set_string(nAtom, &s);
        }

        bool ColorRanges::deploy_items(lltl::parray<ColorRange> *out)
        {
            if (!vItems.reserve(out->size()))
                return false;

            for (size_t i=0, n=out->size(), m=vItems.size(); i<n; ++i)
            {
                ColorRange *src = out->uget(i);
                if (i < m)
                {
                    ColorRange *dst = vItems.uget(i);
                    if (dst == NULL)
                    {
                        vItems.set(i, src);
                        out->set(i, NULL);
                    }
                    else
                        dst->swap(src);
                }
                else
                {
                    vItems.add(src);
                    out->set(i, NULL);
                }
            }

            return true;
        }

        void ColorRanges::commit(atom_t property)
        {
            LSPString s;
            if (pStyle->get_string(nAtom, &s) != STATUS_OK)
                return;

            // Parse list of items and deploy to our list
            sChanges.enable(false);
            {
                lltl::parray<ColorRange> a;
                if (parse_items(&a, &s) == STATUS_OK)
                    deploy_items(&a);
                destroy_items(&a);
            }
            sChanges.enable(true);
        }

        status_t ColorRanges::parse_items(lltl::parray<ColorRange> *out, const LSPString *src)
        {
            io::InStringSequence is;
            status_t res;

            if ((res = is.wrap(src)) != STATUS_OK)
                return res;

            expr::Tokenizer t(&is);
            while (true)
            {
                // Unget token
                expr::token_t tok = t.get_token(expr::TF_GET);
                if (tok == expr::TT_EOF)
                    break;

                if (out->size() > 0)
                {
                    if (tok != expr::TT_COMMA)
                        return STATUS_BAD_FORMAT;
                }
                else
                    t.unget();

                // Create color range
                ColorRange *c = create_item();
                if (c == NULL)
                    return STATUS_NO_MEM;
                else if (!out->add(c))
                {
                    delete c;
                    return STATUS_NO_MEM;
                }

                // Parse the color range
                if ((res = ColorRange::parse(c, &t, pStyle)) != STATUS_OK)
                    return res;
            }

            if ((res = is.close()) != STATUS_OK)
                return res;

            return STATUS_OK;
        }

        void ColorRanges::destroy_items(lltl::parray<ColorRange> *out)
        {
            for (size_t i=0, n=out->size(); i<n; ++i)
            {
                ColorRange *cr = out->uget(i);
                if (cr != NULL)
                    delete cr;
            }
            out->flush();
        }

        ColorRange *ColorRanges::create_item()
        {
            // Here may be some additional initialization stuff
            return new prop::ColorRange(&sChanges);
        }

        ColorRange *ColorRanges::insert(size_t index)
        {
            ColorRange *si  = create_item();
            if (si == NULL)
                return si;

            if (vItems.insert(index, si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        ColorRange *ColorRanges::append()
        {
            ColorRange *si  = create_item();

            if (vItems.append(si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        ColorRange *ColorRanges::prepend()
        {
            ColorRange *si = create_item();
            if (si == NULL)
                return si;

            if (vItems.prepend(si))
            {
                sync();
                return si;
            }

            delete si;
            return NULL;
        }

        status_t ColorRanges::remove(size_t index, size_t count)
        {
            lltl::parray<ColorRange> removed;
            if (!vItems.remove_n(index, count, removed))
                return STATUS_NOT_FOUND;

            for (size_t i=0, n=removed.size(); i<n; ++i)
            {
                ColorRange *si = removed.uget(i);
                if (si != NULL)
                    delete si;
            }

            sync();
            return STATUS_OK;
        }

        status_t ColorRanges::remove(size_t index)
        {
            ColorRange *si = vItems.remove(index);
            if (si == NULL)
                return STATUS_NOT_FOUND;

            delete si;
            sync();

            return STATUS_OK;
        }

        status_t ColorRanges::premove(const ColorRange *s)
        {
            // Lookup for the entry
            ssize_t index = -1;
            for (size_t i=0, n=vItems.size(); i<n; ++i)
            {
                ColorRange *si = vItems.uget(i);
                if (si == s)
                {
                    index = i;
                    break;
                }
            }

            return (index >= 0) ? remove(index) : STATUS_NOT_FOUND;
        }

        status_t ColorRanges::swap(size_t i1, size_t i2)
        {
            ColorRange *si1 = vItems.get(i1);
            if (si1 == NULL)
                return STATUS_NOT_FOUND;

            ColorRange *si2 = vItems.get(i2);
            if (si1 == NULL)
                return STATUS_NOT_FOUND;

            // Prevent from multiple sync()
            sChanges.enable(false);
                si1->swap(si2);
            sChanges.enable(true);

            // Perform sync()
            sync();

            return STATUS_OK;
        }

        status_t ColorRanges::set_all(const char *values)
        {
            LSPString tmp;
            if (!tmp.set_utf8(values))
                return STATUS_NO_MEM;
            return set_all(&tmp);
        }

        status_t ColorRanges::set_all(const LSPString *values)
        {
            status_t res = STATUS_OK;

            sChanges.enable(false);
            {
                lltl::parray<ColorRange> a;
                if ((res = parse_items(&a, values)) == STATUS_OK)
                    deploy_items(&a);
                destroy_items(&a);
            }
            sChanges.enable(true);

            if (res == STATUS_OK)
                sync();

            return res;
        }
    }
}
