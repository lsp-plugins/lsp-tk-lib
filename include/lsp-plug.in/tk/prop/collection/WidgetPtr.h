/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 15 июн. 2020 г.
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

#ifndef LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETPTR_H_
#define LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETPTR_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Pointer to the widget
         */
        template <class widget_t>
            class WidgetPtr: public Ptr
            {
                private:
                    WidgetPtr<widget_t> & operator = (const WidgetPtr<widget_t> &);

                protected:
                    inline WidgetPtr<widget_t> *ccast() const       { return const_cast< WidgetPtr<widget_t> >(this);                               }
                    inline widget_t            *wcast(void *w)      { return (w != NULL) ? static_cast<widget_t*>(static_cast<Widget *>(w)) : NULL; }
                    static inline void         *vcast(Widget *w)    { return w;                         }

                protected:
                    const w_class_t            *pMeta;

                protected:
                    explicit WidgetPtr(prop::Listener *listener = NULL): Ptr(listener)
                    {
                        pMeta   = &widget_t::metadata;
                    }

                    ~WidgetPtr()
                    {
                        pMeta   = NULL;
                    }

                public:
                    inline widget_t            *get()               { return wcast(pCurr);                  }
                    inline const widget_t      *get() const         { return ccast()->get();                }

                    inline widget_t            *get_default()       { return wcast(pDfl);                   }
                    inline const widget_t      *get_default() const { return ccast()->get_default();        }

                    inline widget_t            *set(widget_t *w)    { return ((w != NULL) && (w->instance_of(pMeta))) ? wcast(Ptr::set(vcast(w))) : NULL; }
                    inline widget_t            *set_default()       { return wcast(Ptr::set_default());     }
            };

        namespace prop
        {
            template <class widget_t>
                class WidgetPtr: public tk::WidgetPtr<widget_t>
                {
                    private:
                        WidgetPtr<widget_t> & operator = (const WidgetPtr<widget_t> &);

                    public:
                        explicit WidgetPtr(prop::Listener *listener = NULL): tk::WidgetPtr<widget_t>(listener) {}

                    public:
                        inline void             bind(widget_t *dfl)
                        {
                            this->pDfl      = tk::WidgetPtr<widget_t>::vcast(dfl);
                            this->pCurr     = this->pDfl;
                        }
                };
        }
    }
}


#endif /* LSP_PLUG_IN_TK_PROP_COLLECTION_WIDGETPTR_H_ */
