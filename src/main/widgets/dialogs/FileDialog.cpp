/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 23 окт. 2020 г.
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

#include <lsp-plug.in/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t FileDialog::metadata            = { "FileDialog", &Window::metadata };

        FileDialog::FileDialog(Display *dpy):
            Window(dpy),
            sWPath(dpy),
            sWSearch(dpy),
            sWFilter(dpy),
            sWFiles(dpy),
            sWAction(dpy),
            sWCancel(dpy),
            sMainGrid(dpy),
            sSBBookmarks(dpy),
            sSBAlign(dpy),
            sBookmarks(dpy),
            sBMPopup(dpy),
            sBMAdd(dpy),
            sHBox(dpy),
            sWarnBox(dpy),
            sAppendExt(dpy),
            wAutoExt(dpy),
            wGo(dpy),
            wUp(dpy),
            wPathBox(dpy),
            sWWarning(dpy)
        {
            pWConfirm       = NULL;
            pWSearch        = NULL;
            pWMessage       = NULL;
            pClass          = &metadata;
        }

        FileDialog::~FileDialog()
        {
        }

        status_t FileDialog::init()
        {
            // Initialize labels
            LSP_STATUS_ASSERT(Window::init());
            LSP_STATUS_ASSERT(sWPath.init());
            sWPath.allocation()->set_expand(true);
            LSP_STATUS_ASSERT(sWSearch.init());
            LSP_STATUS_ASSERT(sWFilter.init());
            sWFilter.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(sWFiles.init());
            sWFiles.constraints()->set_min(400, 320);
            sWFiles.allocation()->set_expand(true);
            LSP_STATUS_ASSERT(sWAction.init());
            sWAction.constraints()->set_min(96, 24);
            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(sWCancel.text()->set("actions.cancel"));
            sWCancel.constraints()->set_min(96, 24);
            LSP_STATUS_ASSERT(sWWarning.init());
            sWWarning.visibility()->set(false);
            sWWarning.allocation()->set_expand(true);
            sWWarning.text_layout()->set(1.0f, 0.5f);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.text()->set("actions.nav.go"));
            wGo.constraints()->set_min_width(32);
            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.text()->set("actions.nav.up"));
            wUp.constraints()->set_min_width(32);
            LSP_STATUS_ASSERT(sBMAdd.init());
            LSP_STATUS_ASSERT(sBMAdd.text()->set("actions.to_bookmarks"));
            sBMAdd.constraints()->set_min_width(32);

            LSP_STATUS_ASSERT(wPathBox.init());
            wPathBox.orientation()->set_horizontal();
            wPathBox.spacing()->set(2);
            wPathBox.allocation()->set_fill(true);

            LSP_STATUS_ASSERT(sMainGrid.init());
            sMainGrid.rows()->set(7);
            sMainGrid.columns()->set(2);
            sMainGrid.hspacing()->set(4);
            sMainGrid.vspacing()->set(4);

            LSP_STATUS_ASSERT(sHBox.init());
            sHBox.orientation()->set_horizontal();
            sHBox.spacing()->set(8);

            LSP_STATUS_ASSERT(sWarnBox.init());
            sWarnBox.orientation()->set_horizontal();
            sWarnBox.spacing()->set(8);

            LSP_STATUS_ASSERT(sSBBookmarks.init());
            sSBBookmarks.hscroll_mode()->set(SCROLL_NONE);
            sSBBookmarks.vscroll_mode()->set(SCROLL_OPTIONAL);
            sSBBookmarks.allocation()->set_expand(true);
            sSBBookmarks.constraints()->set_min_width(192);

            LSP_STATUS_ASSERT(sSBAlign.init());
            sSBAlign.layout()->set_align(0.0f, -1.0f); // Middle, Top
            sSBAlign.layout()->set_scale(1.0f, 0.0f); // Maximum width, minimum height
            LSP_STATUS_ASSERT(sSBBookmarks.add(&sSBAlign));

            LSP_STATUS_ASSERT(sBookmarks.init());
            sBookmarks.orientation()->set_vertical();
            sBookmarks.spacing()->set(4);
            sBookmarks.allocation()->set_expand(true);
            LSP_STATUS_ASSERT(sSBAlign.add(&sBookmarks));

// TODO
//            LSP_STATUS_ASSERT(init_bm_popup_menu());

//            init_color(C_YELLOW, sWWarning.font()->color());

            // Initialize supplementary elements
            // Path box
            sBMAdd.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(wPathBox.add(&sBMAdd));
            LSP_STATUS_ASSERT(wPathBox.add(&wUp));
            LSP_STATUS_ASSERT(wPathBox.add(&wGo));
            LSP_STATUS_ASSERT(add_label(&wPathBox, "labels.location", 1.0f));
            // Button box
            LSP_STATUS_ASSERT(sHBox.add(&sWAction));
            LSP_STATUS_ASSERT(sHBox.add(&sWCancel));
            // Warning box
            LSP_STATUS_ASSERT(add_label(&sWarnBox, "labels.file_list"));
            LSP_STATUS_ASSERT(sWarnBox.add(&sWWarning));

            // Initialize grid
            // Row 1
            LSP_STATUS_ASSERT(sMainGrid.add(&wPathBox));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWPath));
            // Row 2
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.bookmark_list"));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWarnBox));
            // Row 3
            LSP_STATUS_ASSERT(sMainGrid.add(&sSBBookmarks));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFiles));
            // Row 4
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(add_ext_button(&sMainGrid, "labels.automatic_extension"));
            // Row 5
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.file_name", 1.0f, &pWSearch));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWSearch));
            // Row 6
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.filter", 1.0f));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFilter));
            // Row 7
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(sMainGrid.add(&sHBox));

            // Initialize structure
//            init_color(C_YELLOW, wAutoExt.color());
            wAutoExt.led()->set(true);
            wAutoExt.mode()->set_toggle();
            wAutoExt.down()->set(true);

            LSP_STATUS_ASSERT(this->add(&sMainGrid));

            // Bind events
// TODO
//            status_t result = sWAction.slots()->bind(SLOT_SUBMIT, slot_on_action, self());
//            if (result < 0)
//                return -result;
//            result = sWCancel.slots()->bind(SLOT_SUBMIT, slot_on_cancel, self());
//            if (result < 0)
//                return -result;
//            result = sWSearch.slots()->bind(SLOT_CHANGE, slot_on_search, self());
//            if (result < 0)
//                return -result;
//            result = sWFilter.slots()->bind(SLOT_SUBMIT, slot_on_search, self());
//            if (result < 0)
//                return -result;
//            result = sWFiles.slots()->bind(SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, self());
//            if (result < 0)
//                return -result;
//            result = sWFiles.slots()->bind(SLOT_CHANGE, slot_list_change, self());
//            if (result < 0)
//                return -result;
//            result = wGo.slots()->bind(SLOT_SUBMIT, slot_on_go, self());
//            if (result < 0)
//                return -result;
//            result = wUp.slots()->bind(SLOT_SUBMIT, slot_on_up, self());
//            if (result < 0)
//                return -result;
//            result = sBMAdd.slots()->bind(SLOT_SUBMIT, slot_on_bm_add, self());
//            if (result < 0)
//                return -result;
//            result = sWPath.slots()->bind(SLOT_KEY_UP, slot_on_path_key_up, self());
//            if (result < 0)
//                return -result;

            padding()->set_all(8);
            border_style()->set(ws::BS_DIALOG);
            actions()->set_actions(ws::WA_DIALOG | ws::WA_RESIZE | ws::WA_CLOSE);

// TODO
//            sync_mode();

            return STATUS_OK;
        }

        status_t FileDialog::add_label(WidgetContainer *c, const char *text, float align, Label **label)
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileDialog::add_menu_item(Menu *m, const char *text, event_handler_t handler)
        {
            // TODO
            return STATUS_OK;
        }

        status_t FileDialog::add_ext_button(WidgetContainer *c, const char *text)
        {
            // TODO
            return STATUS_OK;
        }

        void FileDialog::destroy()
        {
            Window::destroy();
        }
    }
}


