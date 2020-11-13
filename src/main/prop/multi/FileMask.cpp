/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 22 окт. 2020 г.
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

namespace lsp
{
    namespace tk
    {
        FileMask::PListener::PListener(FileMask *prop)
        {
            nLocks  = 0;
            pMask   = prop;
        }

        void FileMask::PListener::notify(Property *prop)
        {
            if ((pMask == NULL) || (nLocks > 0))
                return;

            pMask->sync();
        }

        FileMask::FileMask(prop::Listener *listener):
            sPListener(this),
            sTitle(&sPListener),
            sExtensions(&sPListener),
            sPattern(&sPListener)
        {
        }

        FileMask::~FileMask()
        {
            unbind();
        }

        status_t FileMask::bind(atom_t property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sPListener.lock();
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtensions.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sPListener.unlock();

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::bind(const char *property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sPListener.lock();
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtensions.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sPListener.unlock();

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::bind(const LSPString *property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sPListener.lock();
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtensions.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sPListener.unlock();

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::unbind()
        {
            status_t res;
            sPListener.lock();
            {
                res = sTitle.unbind();
                if (res == STATUS_OK)
                    res = sExtensions.unbind();
                else
                    sExtensions.unbind();
            }
            sPListener.unlock();

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::append_extension(LSPString *str)
        {
            LSPString mask, ext;
            status_t res    = sExtensions.format(&mask);
            if (res != STATUS_OK)
                return res;

            ssize_t first = 0, dfl = -1, last = -1, idx;
            do
            {
                idx     = mask.index_of(first, ':');
                last    = (idx < 0) ? mask.length() : idx;

                if (idx > first)
                {
                    if (!ext.set(&mask, first, last))
                        return STATUS_NO_MEM;
                    if (str->ends_with_nocase(&ext))
                        return STATUS_OK;
                }
                if (dfl < 0)
                    dfl     = last;
                first   = idx + 1;
            } while (idx >= 0);

            if (dfl <= 0)
                return STATUS_OK;

            if (!ext.set(&mask, first, dfl))
                return STATUS_NO_MEM;
            if (!str->append(&ext))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }
    }
}

