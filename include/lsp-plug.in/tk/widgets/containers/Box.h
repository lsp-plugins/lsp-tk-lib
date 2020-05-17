/*
 * Box.h
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BOX_H_
#define LSP_PLUG_IN_TK_BOX_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        /**
         * Implements widget container that allows to pack all elements
         * horizontally or vertically depending on settings
         */
        class Box: public WidgetContainer
        {
            private:
                Box & operator = (const Box &);

            public:
                static const w_class_t    metadata;

            protected:
                typedef struct cell_t
                {
                    ws::size_limit_t    r;          // Required space
                    ws::rectangle_t     a;          // Allocated space
                    ws::rectangle_t     s;          // Really used space
                    padding_t           p;          // Padding
                    Widget             *pWidget;    // Widget contained in the cell
                } cell_t;

            protected:
                lltl::darray<cell_t>        vItems;

                prop::Integer               sSpacing;
                prop::Boolean               sHomogeneous;
                prop::Orientation           sOrientation;

            protected:
                static inline bool          hidden_widget(const cell_t *w);
                static status_t             visible_items(lltl::parray<cell_t> *out, lltl::darray<cell_t> *list);
                void                        do_destroy();

                status_t                    allocate_homogeneous(const ws::rectangle_t *r, lltl::parray<cell_t> &visible);
                status_t                    allocate_proportional(const ws::rectangle_t *r, lltl::parray<cell_t> &visible);
                void                        realize_widgets(lltl::parray<cell_t> &visible);

                virtual Widget             *find_widget(ssize_t x, ssize_t y);
                virtual void                size_request(ws::size_limit_t *r);
                virtual void                property_changed(Property *prop);


            public:
                explicit Box(Display *dpy);
                virtual ~Box();

                virtual status_t            init();
                virtual void                destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /**
                 * Spacing between widget allocations
                 * @return spacing property
                 */
                inline Integer             *spacing()               { return &sSpacing;         }
                inline const Integer       *spacing() const         { return &sSpacing;         }

                /**
                 * Get proportional flag
                 * @return proportional flag property
                 */
                inline Boolean             *homogeneous()           { return &sHomogeneous;    }
                inline const Boolean       *homogeneous() const     { return &sHomogeneous;    }

                /**
                 * Get box orientation
                 * @return box orientation
                 */
                inline Orientation         *orientation()           { return &sOrientation;     }
                inline const Orientation   *orientation() const     { return &sOrientation;     }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual void                render(ws::ISurface *s, bool force);

                virtual status_t            add(Widget *widget);

                virtual status_t            remove(Widget *child);

                virtual status_t            remove_all();

                virtual void                realize(const ws::rectangle_t *r);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_BOX_H_ */
