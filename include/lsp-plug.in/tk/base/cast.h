/*
 * cast.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_BASE_CAST_H_
#define LSP_PLUG_IN_TK_BASE_CAST_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

namespace lsp
{
    namespace tk
    {
        template <class Target>
            inline Target *widget_cast(Widget *src)
            {
                return ((src != NULL) && (src->instance_of(&Target::metadata))) ? static_cast<Target *>(src) : NULL;
            }

        template <class Target>
            inline const Target *widget_cast(const Widget *src)
            {
                return ((src != NULL) && (src->instance_of(&Target::metadata))) ? static_cast<const Target *>(src) : NULL;
            }

        template <class Target>
            inline Target *widget_ptrcast(void *src)
            {
                Widget *w = (src != NULL) ? static_cast<Widget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&Target::metadata))) ? static_cast<Target *>(w) : NULL;
            }

        template <class Target>
            inline const Target *widget_ptrcast(const void *src)
            {
                const Widget *w = (src != NULL) ? static_cast<const Widget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&Target::metadata))) ? static_cast<const Target *>(w) : NULL;
            }
    }
}


#endif /* LSP_PLUG_IN_TK_BASE_CAST_H_ */
