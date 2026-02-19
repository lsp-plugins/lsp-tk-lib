/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 февр. 2026 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        RawString::RawString(prop::Listener *listener):
            SimpleProperty(listener)
        {
            nAtom       = -1;
        }

        RawString::~RawString()
        {
            SimpleProperty::unbind(&sListener);
        }

        void RawString::commit(atom_t property)
        {
            if (pStyle->get_string(nAtom, &sValue) != STATUS_OK)
                return;
        }

        void RawString::push()
        {
            pStyle->set_string(nAtom, &sValue);
        }

        bool RawString::set(const LSPString & value)
        {
            if (sValue.equals(&value))
                return true;

            if (!sValue.set(&value))
                return false;

            sync();
            return true;
        }

        bool RawString::set(const LSPString * value)
        {
            if (value == NULL)
                return false;

            if (sValue.equals(value))
                return true;

            if (!sValue.set(value))
                return false;

            sync();
            return true;
        }

        bool RawString::set(const char * value, const char *charset)
        {
            LSPString tmp;
            if (!tmp.set_native(value, charset))
                return false;

            return set(tmp);
        }

        void RawString::swap(RawString *dst)
        {
            if (dst == this)
                return;

            if (sValue.equals(&dst->sValue))
                return;

            dst->sValue.swap(sValue);
            dst->sync();
            sync();
        }

        void RawString::swap(LSPString *dst)
        {
            if (sValue.equals(dst))
                return;

            dst->swap(sValue);
            sync();
        }

        namespace prop
        {

            bool RawString::commit_value(const LSPString & value)
            {
                return sValue.set(&value);
            }

            bool RawString::commit_value(const LSPString *value)
            {
                return sValue.set(value);
            }

            bool RawString::commit_value(const char *value, const char *charset)
            {
                return sValue.set_native(value, charset);
            }

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



