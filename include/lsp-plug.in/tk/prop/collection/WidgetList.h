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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_

#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {
        class GenericWidgetList: public Property
        {
            private:
                GenericWidgetList & operator = (const GenericWidgetList &);

            protected:
                const w_class_t                *pMeta;
                prop::CollectionListener       *pCListener;
                mutable lltl::parray<Widget>    sList;

            public:
                explicit GenericWidgetList(const w_class_t *meta, prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL);
                ~GenericWidgetList();

            public:
                status_t        add(Widget *w);
                status_t        premove(Widget *w);
                status_t        remove(size_t index);
                status_t        remove(size_t index, size_t count);
                status_t        insert(Widget *w, size_t index);
                void            clear();

                status_t        set(Widget *w, size_t index);
                status_t        swap(GenericWidgetList *dst);
                status_t        xswap(size_t i1, size_t i2);
        };

        template <class widget_t>
            class WidgetList: public GenericWidgetList
            {
                private:
                    WidgetList<widget_t> & operator = (const WidgetList<widget_t> &);

                protected:
                    inline widget_t    *wcast(Widget *w)                    { return (w != NULL) ? static_cast<widget_t*>(w) : NULL;    }

                public:
                    explicit WidgetList(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): GenericWidgetList(&widget_t::metadata, listener, clistener) {}
                    ~WidgetList() {}

                public:
                    inline size_t       size() const                        { return sList.size();                          }
                    inline status_t     add(widget_t *w)                    { return GenericWidgetList::add(w);             }
                    inline status_t     premove(widget_t *w)                { return GenericWidgetList::premove(w);         }
                    inline status_t     insert(widget_t *w, size_t index)   { return GenericWidgetList::insert(w, index);   }
                    inline status_t     set(widget_t *w, size_t index)      { return GenericWidgetList::set(w, index);      }
                    inline widget_t    *get(size_t index)                   { return wcast(sList.get(index));               }
                    inline bool         contains(const widget_t *w) const   { return sList.contains(w);                     }
                    inline ssize_t      index_of(const widget_t *w) const   { return sList.index_of(w);                     }

                    inline status_t     swap(WidgetList<widget_t> *lst)     { return GenericWidgetList::swap(lst);          }
                    inline status_t     swap(WidgetList<widget_t> &lst)     { return GenericWidgetList::swap(&lst);         }
            };

        namespace prop
        {
            template <class widget_t>
                class WidgetList: public tk::WidgetList<widget_t>
                {
                    private:
                        WidgetList<widget_t> & operator = (const WidgetList<widget_t> &);

                    public:
                        explicit WidgetList(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): tk::WidgetList<widget_t>(listener, clistener) {}
                        ~WidgetList() {}

                    public:
                        inline void         flush()                             { this->sList.flush();                                              }
                        inline bool         unlink(Widget *w)                   { return this->sList.premove(w);                                    }
                        inline widget_t    *uget(size_t index)                  { return tk::WidgetList<widget_t>::wcast(this->sList.get(index));   }
                };
        }
    }

}

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_ */
