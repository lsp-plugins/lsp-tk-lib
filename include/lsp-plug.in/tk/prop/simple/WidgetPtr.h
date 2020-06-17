/*
 * WidgetPtr.h
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_PROP_SIMPLE_WIDGETPTR_H_
#define LSP_PLUG_IN_TK_PROP_SIMPLE_WIDGETPTR_H_

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
                    explicit WidgetPtr(const w_class_t *meta, prop::Listener *listener = NULL): Ptr(listener)
                    {
                        pMeta   = meta;
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
                        explicit WidgetPtr(const w_class_t *meta, prop::Listener *listener = NULL): tk::WidgetPtr<widget_t>(meta, listener) {}

                    public:
                        inline void             bind(widget_t *dfl) { this->pDfl  = tk::WidgetPtr<widget_t>::vcast(dfl);    }
                };
        }
    }
}



#endif /* LSP_PLUG_IN_TK_PROP_SIMPLE_WIDGETPTR_H_ */
