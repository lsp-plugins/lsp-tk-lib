/*
 * Size.cpp
 *
 *  Created on: 10 мая 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Size::DESC[] =
        {
            { "",           PT_STRING   },
            { ".width",     PT_INT      },
            { ".height",    PT_INT      },
            { NULL,         PT_UNKNOWN  }
        };

        void Size::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Size::Size(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nWidth              = 0;
            nHeight             = 0;
        }

        Size::~Size()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Size::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            ssize_t v;
            if ((property == vAtoms[P_WIDTH]) && (pStyle->get_int(vAtoms[P_WIDTH], &v) == STATUS_OK))
                nWidth      = lsp_max(v, 0);
            if ((property == vAtoms[P_HEIGHT]) && (pStyle->get_int(vAtoms[P_HEIGHT], &v) == STATUS_OK))
                nHeight     = lsp_max(v, 0);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                ssize_t xv[2];
                size_t n = Property::parse_ints(xv, 2, &s);
                if (n == 2)
                {
                    nWidth      = lsp_max(xv[0], 0);
                    nHeight     = lsp_max(xv[1], 0);
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Size::sync(bool notify)
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_WIDTH] >= 0)
                        pStyle->set_int(vAtoms[P_WIDTH], nWidth);
                    if (vAtoms[P_HEIGHT] >= 0)
                        pStyle->set_int(vAtoms[P_HEIGHT], nHeight);

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%ld %ld", long(nWidth), long(nHeight)))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }
            if ((pListener != NULL) && (notify))
                pListener->notify(this);
        }

        size_t Size::set_width(size_t value)
        {
            size_t old      = nWidth;
            if (old == value)
                return value;

            nWidth          = value;
            sync(true);
            return old;
        }

        size_t Size::set_height(size_t value)
        {
            size_t old      = nHeight;
            if (old == value)
                return value;

            nHeight         = value;
            sync(true);
            return old;
        }

        void Size::set(size_t width, size_t height)
        {
            if ((nWidth == width) &&
                (nHeight == height))
                return;

            nWidth      = width;
            nHeight     = height;
            sync(true);
        }

        void Size::set(const Size *p)
        {
            if ((nWidth == p->nWidth) &&
                (nHeight == p->nHeight))
                return;

            nWidth      = p->nWidth;
            nHeight     = p->nHeight;
            sync(true);
        }

        namespace prop
        {
            status_t Size::init(Style *style, size_t width, size_t height)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_int(vAtoms[P_WIDTH], width);
                    style->create_int(vAtoms[P_HEIGHT], height);
                }
                style->end();
                return STATUS_OK;
            }

            void Size::commit(size_t width, size_t height, float scale)
            {
                if (scale > 0.0f)
                {
                    nWidth  = size_t(float(width) / scale);
                    nHeight = size_t(float(height) / scale);
                }
                else
                {
                    nWidth  = width;
                    nHeight = height;
                }
                sync(false);
            }
        }
    } /* namespace tk */
} /* namespace lsp */



