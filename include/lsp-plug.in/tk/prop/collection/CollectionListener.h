/*
 * CollectionListener.h
 *
 *  Created on: 29 июл. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_

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
             * @param w widget that has been added
             */
            typedef void (* collection_cb_t)(void *obj, Property *prop, Widget *w);

            /**
             * Collection listener
             */
            class CollectionListener
            {
                private:
                    CollectionListener &operator = (const CollectionListener &);

                protected:
                    void               *pAddObj;
                    collection_cb_t     pAddCb;
                    void               *pRemObj;
                    collection_cb_t     pRemCb;

                public:
                    explicit CollectionListener();
                    virtual ~CollectionListener();

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
                     * @param w widget that has been added
                     */
                    virtual void    add(Property *prop, Widget *w);

                    /**
                     * Callback that notifies that widget has been removed
                     * @param prop property that triggered event
                     * @param w widget that has been removed
                     */
                    virtual void    remove(Property *prop, Widget *w);
            };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_COLLECTIONLISTENER_H_ */
