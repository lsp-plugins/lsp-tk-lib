/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 1 авг. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/lltl/phashset.h>

namespace lsp
{
    namespace tk
    {
        class GenericWidgetSet: public Property
        {
            protected:
                const w_class_t                *pMeta;
                prop::CollectionListener       *pCListener;
                mutable lltl::phashset<Widget>  sSet;

            public:
                explicit GenericWidgetSet(const w_class_t *meta, prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL);
                GenericWidgetSet(const GenericWidgetSet &) = delete;
                GenericWidgetSet(GenericWidgetSet &&) = delete;
                virtual ~GenericWidgetSet() override;

                GenericWidgetSet & operator = (const GenericWidgetSet &) = delete;
                GenericWidgetSet & operator = (GenericWidgetSet &&) = delete;

            public:
                status_t        add(Widget *w);
                status_t        remove(Widget *w);
                status_t        toggle(Widget *w);
                void            clear();

                status_t        swap(GenericWidgetSet *dst);
                inline status_t swap(GenericWidgetSet &dst)         { return swap(&dst);                            }

                inline bool     values(lltl::parray<Widget> *dst)   { return sSet.values(dst);                      }
                inline bool     values(lltl::parray<Widget> &dst)   { return sSet.values(&dst);                     }
        };

        template <class widget_t>
        class WidgetSet: public GenericWidgetSet
        {
            protected:
                inline widget_t    *wcast(Widget *w)                    { return (w != NULL) ? static_cast<widget_t*>(w) : NULL;    }

            public:
                explicit WidgetSet(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): GenericWidgetSet(&widget_t::metadata, listener, clistener) {}
                WidgetSet(const WidgetSet<widget_t> &) = delete;
                WidgetSet(WidgetSet<widget_t> &&) = delete;

                WidgetSet<widget_t> & operator = (const WidgetSet<widget_t> &) = delete;
                WidgetSet<widget_t> & operator = (WidgetSet<widget_t> &&) = delete;

            public:
                inline size_t       size() const                        { return sSet.size();                           }
                inline status_t     add(widget_t *w)                    { return GenericWidgetSet::add(w);              }
                inline status_t     remove(widget_t *w)                 { return GenericWidgetSet::remove(w);           }
                inline bool         contains(const widget_t *w) const   { return sSet.contains(w);                      }
                inline status_t     toggle(widget_t *w)                 { return GenericWidgetSet::toggle(w);           }
                inline widget_t    *any()                               { return wcast(sSet.any());                     }

                inline bool         values(lltl::parray<widget_t> *dst)
                {
                    union {
                        lltl::parray<widget_t> *wt;
                        lltl::parray<Widget>   *wg;
                    } x;
                    x.wt = dst;
                    if (!sSet.values(x.wg))
                        return false;

                    for (size_t i=0, n=dst->size(); i<n; ++i)
                        dst->set(i, wcast(dst->uget(i)));

                    return true;
                }
                inline bool         values(lltl::parray<Widget> &dst)   { return values(&dst);                          }

                using GenericWidgetSet::values;

                inline status_t     swap(WidgetSet<widget_t> *lst)      { return GenericWidgetSet::swap(lst);           }
                inline status_t     swap(WidgetSet<widget_t> &lst)      { return GenericWidgetSet::swap(&lst);          }
        };

        namespace prop
        {
            template <class widget_t>
            class WidgetSet: public tk::WidgetSet<widget_t>
            {
                public:
                    explicit WidgetSet(prop::Listener *listener = NULL, prop::CollectionListener *clistener = NULL): tk::WidgetSet<widget_t>(listener, clistener) {}
                    WidgetSet(const WidgetSet<widget_t> &) = delete;
                    WidgetSet(WidgetSet<widget_t> &&) = delete;
                    WidgetSet<widget_t> & operator = (const WidgetSet<widget_t> &) = delete;
                    WidgetSet<widget_t> & operator = (WidgetSet<widget_t> &&) = delete;

                public:
                    inline void         flush()                             { this->sSet.flush();                                               }
            };


        } /* namespace prop */
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETSET_H_ */
