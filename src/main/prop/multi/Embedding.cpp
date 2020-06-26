/*
 * Embedding.cpp
 *
 *  Created on: 26 июн. 2020 г.
 *      Author: sadko
 */

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const prop::desc_t Embedding::DESC[] =
        {
            { "",           PT_STRING   },
            { ".left",      PT_BOOL     },
            { ".right",     PT_BOOL     },
            { ".top",       PT_BOOL     },
            { ".bottom",    PT_BOOL     },
            NULL
        };

        void Embedding::Listener::notify(atom_t property)
        {
            pValue->commit(property);
        }

        Embedding::Embedding(prop::Listener *listener):
            MultiProperty(listener),
            sListener(this)
        {
            nFlags      = 0;
        }

        Embedding::~Embedding()
        {
            MultiProperty::unbind(vAtoms, DESC, &sListener);
        }

        void Embedding::commit(atom_t property)
        {
            if ((pStyle == NULL) || (property < 0))
                return;

            bool v;
            if ((property == vAtoms[P_LEFT]) && (pStyle->get_bool(vAtoms[P_LEFT], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_LEFT, v);
            if ((property == vAtoms[P_RIGHT]) && (pStyle->get_bool(vAtoms[P_RIGHT], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_RIGHT, v);
            if ((property == vAtoms[P_TOP]) && (pStyle->get_bool(vAtoms[P_TOP], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_TOP, v);
            if ((property == vAtoms[P_BOTTOM]) && (pStyle->get_bool(vAtoms[P_BOTTOM], &v) == STATUS_OK))
                nFlags      = lsp_setflag(nFlags, M_BOTTOM, v);

            LSPString s;
            if ((property == vAtoms[P_VALUE]) && (pStyle->get_string(vAtoms[P_VALUE], &s) == STATUS_OK))
            {
                bool xv[4];
                size_t n = Property::parse_bools(xv, 4, &s);
                switch (n)
                {
                    case 1:
                        nFlags      = lsp_setflag(nFlags, M_ALL, xv[0]);
                        break;
                    case 2:
                        nFlags      = lsp_setflag(nFlags, M_HOR,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_VERT, xv[1]);
                        break;
                    case 3:
                        nFlags      = lsp_setflag(nFlags, M_LEFT,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_RIGHT, xv[1]);
                        nFlags      = lsp_setflag(nFlags, M_VERT,  xv[2]);
                        break;
                    case 4:
                        nFlags      = lsp_setflag(nFlags, M_LEFT,  xv[0]);
                        nFlags      = lsp_setflag(nFlags, M_RIGHT, xv[1]);
                        nFlags      = lsp_setflag(nFlags, M_TOP,   xv[2]);
                        nFlags      = lsp_setflag(nFlags, M_BOTTOM,xv[3]);
                        break;
                    default:
                        break;
                }
            }

            if (pListener != NULL)
                pListener->notify(this);
        }

        void Embedding::sync()
        {
            if (pStyle != NULL)
            {
                pStyle->begin(&sListener);
                {
                    // Simple components
                    if (vAtoms[P_LEFT] >= 0)
                        pStyle->set_int(vAtoms[P_LEFT], left());
                    if (vAtoms[P_RIGHT] >= 0)
                        pStyle->set_int(vAtoms[P_RIGHT], right());
                    if (vAtoms[P_TOP] >= 0)
                        pStyle->set_int(vAtoms[P_TOP], top());
                    if (vAtoms[P_BOTTOM] >= 0)
                        pStyle->set_int(vAtoms[P_BOTTOM], bottom());

                    // Compound objects
                    LSPString s;
                    if (vAtoms[P_VALUE] >= 0)
                    {
                        if (s.fmt_ascii("%s %s %s %s",
                                (left()) ? "true" : "false",
                                (right()) ? "true" : "false",
                                (top()) ? "true" : "false",
                                (bottom()) ? "true" : "false"
                            ))
                            pStyle->set_string(vAtoms[P_VALUE], &s);
                    }
                }
                pStyle->end();
            }
            if (pListener != NULL)
                pListener->notify(this);
        }

        void Embedding::set(bool left, bool right, bool top, bool bottom)
        {
            size_t flags = nFlags;
            flags = (left)  ? flags | M_LEFT   : flags & (~M_LEFT);
            flags = (right) ? flags | M_RIGHT  : flags & (~M_RIGHT);
            flags = (top)   ? flags | M_TOP    : flags & (~M_TOP);
            flags = (bottom)? flags | M_BOTTOM : flags & (~M_BOTTOM);

            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set(bool hor, bool vert)
        {
            size_t flags = nFlags;
            flags = (hor)   ? flags | M_HOR    : flags & (~M_HOR);
            flags = (vert)  ? flags | M_VERT   : flags & (~M_VERT);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        void Embedding::set(bool on)
        {
            size_t flags = nFlags;
            flags = (on)    ? flags | M_ALL    : flags & (~M_ALL);
            if (flags == nFlags)
                return;

            nFlags  = flags;
            sync();
        }

        namespace prop
        {
            status_t Embedding::init(Style *style, bool left, bool right, bool top, bool bottom)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->create_bool(vAtoms[P_LEFT],   left);
                    style->create_bool(vAtoms[P_RIGHT],  right);
                    style->create_bool(vAtoms[P_TOP],    top);
                    style->create_bool(vAtoms[P_BOTTOM], bottom);

                    LSPString s;
                    s.fmt_ascii("%s %s %s %s",
                                (left) ? "true" : "false",
                                (right) ? "true" : "false",
                                (top) ? "true" : "false",
                                (bottom) ? "true" : "false"
                        );
                    style->create_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }

            status_t Embedding::override(Style *style, bool left, bool right, bool top, bool bottom)
            {
                if (pStyle == NULL)
                    return STATUS_BAD_STATE;

                style->begin();
                {
                    style->override_bool(vAtoms[P_LEFT],   left);
                    style->override_bool(vAtoms[P_RIGHT],  right);
                    style->override_bool(vAtoms[P_TOP],    top);
                    style->override_bool(vAtoms[P_BOTTOM], bottom);

                    LSPString s;
                    s.fmt_ascii("%s %s %s %s",
                                (left) ? "true" : "false",
                                (right) ? "true" : "false",
                                (top) ? "true" : "false",
                                (bottom) ? "true" : "false"
                        );
                    style->override_string(vAtoms[P_VALUE], &s);
                }
                style->end();
                return STATUS_OK;
            }
        }

    }
}


