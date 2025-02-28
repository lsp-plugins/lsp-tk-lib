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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {
        class GenericWidgetList: public Property
        {
            protected:
                typedef struct item_t
                {
                    Widget     *pWidget;
                    bool        bManage;
                } item_t;

            protected:
                const w_class_t                *pMeta;
                prop::CollectionListener       *pCListener;
                mutable lltl::darray<item_t>    sList;

            public:
                explicit GenericWidgetList(const w_class_t *meta, prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL);
                GenericWidgetList(const GenericWidgetList &) = delete;
                GenericWidgetList(GenericWidgetList &&) = delete;
                virtual ~GenericWidgetList() override;

                GenericWidgetList & operator = (const GenericWidgetList &) = delete;
                GenericWidgetList & operator = (GenericWidgetList &&) = delete;

            public:
                status_t        add(Widget *w, bool manage);
                status_t        premove(const Widget *w);
                status_t        remove(size_t index);
                status_t        remove(size_t index, size_t count);
                status_t        truncate(size_t count);
                status_t        insert(Widget *w, size_t index, bool manage);
                void            clear();
                void            flush();
                ssize_t         index_of(const Widget *w) const;
                inline bool     contains(const Widget *w) const             { return index_of(w) >= 0;                              }
                Widget         *get(size_t index);

                status_t        set(Widget *w, size_t index, bool manage);
                status_t        swap(GenericWidgetList *dst);
                status_t        xswap(size_t i1, size_t i2);
        };

        template <class widget_t>
        class WidgetList: public GenericWidgetList
        {
            protected:
                inline widget_t    *wcast(Widget *w)                    { return (w != NULL) ? static_cast<widget_t*>(w) : NULL;    }

            public:
                explicit WidgetList(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): GenericWidgetList(&widget_t::metadata, listener, clistener) {}
                WidgetList(const WidgetList<widget_t> &) = delete;
                WidgetList(WidgetList<widget_t> &&) = delete;

                WidgetList<widget_t> & operator = (const WidgetList<widget_t> &) = delete;
                WidgetList<widget_t> & operator = (WidgetList<widget_t> &&) = delete;

            public:
                inline size_t       size() const                        { return sList.size();                                  }
                inline bool         is_empty() const                    { return sList.is_empty();                              }
                inline status_t     add(widget_t *w)                    { return GenericWidgetList::add(w, false);              }
                inline status_t     madd(widget_t *w)                   { return GenericWidgetList::add(w, true);               }
                inline status_t     insert(widget_t *w, size_t index)   { return GenericWidgetList::insert(w, index, false);    }
                inline status_t     minsert(widget_t *w, size_t index)  { return GenericWidgetList::insert(w, index, true);     }
                inline status_t     set(widget_t *w, size_t index)      { return GenericWidgetList::set(w, index, false);       }
                inline status_t     mset(widget_t *w, size_t index)     { return GenericWidgetList::set(w, index, true);        }
                inline widget_t    *get(size_t index)                   { return wcast(GenericWidgetList::get(index));          }

                inline status_t     swap(WidgetList<widget_t> *lst)     { return GenericWidgetList::swap(lst);                  }
                inline status_t     swap(WidgetList<widget_t> &lst)     { return GenericWidgetList::swap(&lst);                 }
        };

        namespace prop
        {
            template <class widget_t>
            class WidgetList: public tk::WidgetList<widget_t>
            {
                public:
                    explicit WidgetList(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): tk::WidgetList<widget_t>(listener, clistener) {}
                    WidgetList(const WidgetList<widget_t> &) = delete;
                    WidgetList(WidgetList<widget_t> &&) = delete;
                    WidgetList<widget_t> & operator = (const WidgetList<widget_t> &) = delete;
                    WidgetList<widget_t> & operator = (WidgetList<widget_t> &&) = delete;

                public:
                    inline void         flush()                             { tk::WidgetList<widget_t>::flush();                }
                    inline bool         unlink(Widget *w)                   { return tk::WidgetList<widget_t>::premove(w);      }
            };

        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETLIST_H_ */
