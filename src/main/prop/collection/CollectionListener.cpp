/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 29 июл. 2020 г.
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
        namespace prop
        {
            CollectionListener::CollectionListener()
            {
                pAddObj     = NULL;
                pAddCb      = NULL;
                pRemObj     = NULL;
                pRemCb      = NULL;
            }

            CollectionListener::~CollectionListener()
            {
                unbind_all();
            }

            void CollectionListener::bind_add(void *obj, collection_cb_t cb)
            {
                pAddObj     = obj;
                pAddCb      = cb;
            }

            void CollectionListener::bind_remove(void *obj, collection_cb_t cb)
            {
                pRemObj     = obj;
                pRemCb      = cb;
            }

            void CollectionListener::bind_all(void *obj, collection_cb_t add, collection_cb_t rem)
            {
                pAddObj     = obj;
                pAddCb      = add;
                pRemObj     = obj;
                pRemCb      = rem;
            }

            void CollectionListener::unbind_add()
            {
                pAddObj     = NULL;
                pAddCb      = NULL;
            }

            void CollectionListener::unbind_remove()
            {
                pRemObj     = NULL;
                pRemCb      = NULL;
            }

            void CollectionListener::unbind_all()
            {
                pAddObj     = NULL;
                pAddCb      = NULL;
                pRemObj     = NULL;
                pRemCb      = NULL;
            }

            void CollectionListener::add(Property *prop, void *item)
            {
                if (pAddCb != NULL)
                    pAddCb(pAddObj, prop, item);
            }

            void CollectionListener::remove(Property *prop, void *item)
            {
                if (pRemCb != NULL)
                    pRemCb(pRemObj, prop, item);
            }
        }
    }
}


