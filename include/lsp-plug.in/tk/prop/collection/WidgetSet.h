/*
 * WidgetSet.h
 *
 *  Created on: 1 авг. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_

#include <lsp-plug.in/lltl/phashset.h>

namespace lsp
{
    namespace tk
    {
        class GenericWidgetSet: public Property
        {
            private:
                GenericWidgetSet & operator = (const GenericWidgetSet &);

            protected:
                const w_class_t                *pMeta;
                prop::CollectionListener       *pCListener;
                mutable lltl::phashset<Widget>  sSet;

            public:
                explicit GenericWidgetSet(const w_class_t *meta, prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL);
                ~GenericWidgetSet();

            public:
                status_t        add(Widget *w);
                status_t        remove(Widget *w);
                void            clear();

                status_t        swap(GenericWidgetSet *dst);
                inline status_t swap(GenericWidgetSet &dst)     { return swap(&dst);            }
        };

        template <class widget_t>
            class WidgetSet: public GenericWidgetSet
            {
                private:
                    WidgetSet<widget_t> & operator = (const WidgetSet<widget_t> &);

                protected:
                    inline widget_t    *wcast(Widget *w)                    { return (w != NULL) ? static_cast<widget_t*>(w) : NULL;    }

                public:
                    explicit WidgetSet(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): GenericWidgetSet(&widget_t::metadata, listener, clistener) {}
                    ~WidgetSet() {}

                public:
                    inline size_t       size() const                        { return sSet.size();                           }
                    inline status_t     add(widget_t *w)                    { return GenericWidgetSet::add(w);              }
                    inline status_t     remove(widget_t *w)                 { return GenericWidgetSet::remove(w);           }
                    inline bool         contains(const widget_t *w) const   { return sSet.contains(w);                      }

                    inline status_t     swap(WidgetSet<widget_t> *lst)      { return GenericWidgetSet::swap(lst);           }
                    inline status_t     swap(WidgetSet<widget_t> &lst)      { return GenericWidgetSet::swap(&lst);         }
            };

        namespace prop
        {
            template <class widget_t>
                class WidgetSet: public tk::WidgetSet<widget_t>
                {
                    private:
                        WidgetSet<widget_t> & operator = (const WidgetSet<widget_t> &);

                    public:
                        explicit WidgetSet(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): tk::WidgetSet<widget_t>(listener, clistener) {}
                        ~WidgetSet() {}

                    public:
                        inline void         flush()                             { this->sSet.flush();                                               }
                };
        }
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_ */
