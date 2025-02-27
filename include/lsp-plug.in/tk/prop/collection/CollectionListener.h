/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        class Widget;

        namespace prop
        {
            /**
             * Callback type for collection listener
             * @param obj object
             * @param prop property that triggered event
             * @param item the item that has been added/removed
             */
            typedef void (* collection_cb_t)(void *obj, Property *prop, void *item);

            /**
             * Collection listener
             */
            class CollectionListener
            {
                protected:
                    void               *pAddObj;
                    collection_cb_t     pAddCb;
                    void               *pRemObj;
                    collection_cb_t     pRemCb;

                public:
                    explicit CollectionListener();
                    CollectionListener(const CollectionListener &) = delete;
                    CollectionListener(CollectionListener &&) = delete;
                    virtual ~CollectionListener();

                    CollectionListener &operator = (const CollectionListener &) = delete;
                    CollectionListener &operator = (CollectionListener &&) = delete;

                public:
                    void            bind_add(void *obj, collection_cb_t cb);
                    void            bind_remove(void *obj, collection_cb_t cb);
                    void            bind_all(void *obj, collection_cb_t add, collection_cb_t rem);

                    void            unbind_add();
                    void            unbind_remove();
                    void            unbind_all();

                public:
                    /**
                     * Callback that notifies that widget has been added
                     * @param prop property that triggered event
                     * @param item item that has been added to collection
                     */
                    virtual void    add(Property *prop, void *item);

                    /**
                     * Callback that notifies that widget has been removed
                     * @param prop property that triggered event
                     * @param item item that has been removed from collection
                     */
                    virtual void    remove(Property *prop, void *item);
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */



#endif /* LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_ */
