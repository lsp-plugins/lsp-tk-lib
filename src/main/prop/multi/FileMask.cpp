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
        void FileMask::Listener::notify(Property *prop)
        {
            pMask->commit(prop);
        }

        FileMask::FileMask(prop::Listener *listener):
            sTitle(&sListener),
            sExtension(&sListener),
            sPattern(&sListener),
            sListener(this)
        {
        }

        FileMask::~FileMask()
        {
            unbind();
        }

        status_t FileMask::bind(atom_t property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sListener.set_lock(true);
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtension.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sListener.set_lock(false);

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::bind(const char *property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sListener.set_lock(true);
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtension.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sListener.set_lock(false);

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::bind(const LSPString *property, Style *style, i18n::IDictionary *dict)
        {
            status_t res;
            sListener.set_lock(true);
            {
                res = sTitle.bind(property, style, dict);
                if (res == STATUS_OK)
                {
                    res = sExtension.bind(property, style, dict);
                    if (res != STATUS_OK)
                        sTitle.unbind();
                }
            }
            sListener.set_lock(false);

            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t FileMask::unbind()
        {
            status_t res;
            sListener.set_lock(true);
            {
                res = sTitle.unbind();
                if (res == STATUS_OK)
                    res = sExtension.unbind();
                else
                    sExtension.unbind();
            }
            sListener.set_lock(false);

            if (res == STATUS_OK)
                sync();

            return res;
        }

        void FileMask::commit(Property *prop)
        {
            if (pListener != NULL)
                pListener->notify(this);
        }

        void FileMask::sync()
        {
            if (pListener != NULL)
                pListener->notify(this);
        }
    }
}

