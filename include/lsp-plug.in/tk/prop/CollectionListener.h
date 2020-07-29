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
             * Collection listener
             */
            class CollectionListener
            {
                private:
                    CollectionListener &operator = (const CollectionListener &);

                public:
                    explicit CollectionListener();
                    virtual ~CollectionListener();

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
