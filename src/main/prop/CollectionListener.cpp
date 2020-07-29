/*
 * CollectionListener.cpp
 *
 *  Created on: 29 июл. 2020 г.
 *      Author: sadko
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

            void CollectionListener::add(Property *prop, Widget *w)
            {
                if (pAddCb != NULL)
                    pAddCb(pAddObj, prop, w);
            }

            void CollectionListener::remove(Property *prop, Widget *w)
            {
                if (pRemCb != NULL)
                    pRemCb(pRemObj, prop, w);
            }
        }
    }
}


