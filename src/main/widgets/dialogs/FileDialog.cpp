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
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/runtime/system.h>
#include <lsp-plug.in/io/Dir.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            //-----------------------------------------------------------------
            // FileDialog style
            LSP_TK_STYLE_IMPL_BEGIN(FileDialog, Window)
                sMode.bind("mode", this);
                sCustomAction.bind("custom.action", this);
                sSelFilter.bind("filter.selected", this);
                sUseConfirm.bind("confirm", this);
                // Configure
                sMode.set(FDM_OPEN_FILE);
                sCustomAction.set(false);
                sSelFilter.set(0);
                sUseConfirm.set(false);
                // Override
                sPadding.set_all(8);
                sBorderStyle.set(ws::BS_DIALOG);
                sActions.set_actions(ws::WA_DIALOG | ws::WA_RESIZE | ws::WA_CLOSE);
                sLayout.set(0.0f, 1.0f);
                // Commit
                sMode.override();
                sCustomAction.override();
                sSelFilter.override();
                sUseConfirm.override();
                sPadding.override();
                sBorderStyle.override();
                sActions.override();
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog, "FileDialog");

            //-----------------------------------------------------------------
            // FileDialog::NavButton style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__NavButton, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__NavButton, Button)
                // Override
                sConstraints.set_min_width(32);
                sAllocation.set_fill(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__NavButton, "FileDialog::NavButton");

            //-----------------------------------------------------------------
            // FileDialog::ActionButton style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ActionButton, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ActionButton, Button)
                // Override
                sConstraints.set_min_width(96);
                sAllocation.set_fill(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ActionButton, "FileDialog::ActionButton");

            //-----------------------------------------------------------------
            // FileDialog::Bookmark style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Bookmark, Hyperlink)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Bookmark, Hyperlink)
                // Override
                sPadding.set(2, 8);
                sTextLayout.set_halign(-1.0f);
                sFollow.set(false);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Bookmark, "FileDialog::Bookmark");

            //-----------------------------------------------------------------
            // FileDialog::Bookmark.selected style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Bookmark_selected, FileDialog__Bookmark)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Bookmark_selected, FileDialog__Bookmark)
                // Override
                sColor.set("#ffffff");
                sBgColor.set("#888888");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Bookmark_selected, "FileDialog::Bookmark.selected");

            //-----------------------------------------------------------------
            // FileDialog::Warning
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Warning, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Warning, Label)
                // Override
                sTextLayout.set(1.0f, 0.5f);
                sColor.set("#ff0000");
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Warning, "FileDialog::Warning");

            //-----------------------------------------------------------------
            // FileDialog::ExtCheck
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ExtCheck, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ExtCheck, Button)
                // Override
                sMode.set_toggle();
                sColor.set("#00ff00");
                sLed.set(true);
                sDown.set(true);
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ExtCheck, "FileDialog::ExtCheck");
        }

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
            sWWarning(dpy),

            sMode(&sProperties),
            sCustomAction(&sProperties),
            sActionText(&sProperties),
            sPath(&sProperties),
            sFilter(&sProperties),
            sSelFilter(&sProperties),
            sSelected(&sProperties),
            sUseConfirm(&sProperties),
            sConfirmMsg(&sProperties)
        {
            pWConfirm       = NULL;
            pWSearch        = NULL;
            pWMessage       = NULL;

            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;

            pNavButton      = NULL;
            pActButton      = NULL;
            pBMNormal       = NULL;
            pBMSel          = NULL;
            pWarning        = NULL;
            pExtCheck       = NULL;

            pClass          = &metadata;
        }

        FileDialog::~FileDialog()
        {
            nFlags     |= FINALIZED;
        }

        void FileDialog::destroy()
        {
            nFlags     |= FINALIZED;
            Window::destroy();

            drop_bookmarks();
            destroy_file_entries(&vFiles);

            // Clear dynamically allocated widgets
            size_t n = vWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                Widget *w       = vWidgets.uget(i);
                if (w == NULL)
                    continue;
                w->destroy();
                delete w;
            }
            vWidgets.flush();

            sWPath.destroy();
            sWSearch.destroy();
            sWFilter.destroy();
            sWFiles.destroy();
            sWAction.destroy();
            sWCancel.destroy();
            sHBox.destroy();
            sWarnBox.destroy();
            sSBBookmarks.destroy();
            sSBAlign.destroy();
            sBookmarks.destroy();
            sBMPopup.destroy();
            sBMAdd.destroy();
            sMainGrid.destroy();
            sWWarning.destroy();
            sAppendExt.destroy();
            wAutoExt.destroy();
            wGo.destroy();
            wUp.destroy();
            wPathBox.destroy();

            pWSearch = NULL;

            if (pWConfirm != NULL)
            {
                pWConfirm->destroy();
                delete pWConfirm;
                pWConfirm = NULL;
            }

            if (pWMessage != NULL)
            {
                pWMessage->destroy();
                delete pWMessage;
                pWMessage = NULL;
            }
        }

        void FileDialog::drop_bookmarks()
        {
            // Deactivate currently selected bookmark
            sBookmarks.remove_all();
            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;

            // Destroy bookmarks storage
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
            }
            vBookmarks.flush();
        }

        void FileDialog::destroy_file_entries(lltl::parray<f_entry_t> *list)
        {
            for (size_t i=0, n = list->size(); i<n; ++i)
            {
                f_entry_t *fd = list->uget(i);
                if (fd != NULL)
                    delete fd;
            }
            list->clear();
        }

        status_t FileDialog::init()
        {
            Label *l;

            // Initialize window
            LSP_STATUS_ASSERT(Window::init());

            lsp_trace("Scaling factor: %f", sScaling.get());

            // Init styles
            pNavButton      = pDisplay->schema()->get("FileDialog::NavButton");
            if (pNavButton == NULL)
                return STATUS_BAD_STATE;
            pActButton      = pDisplay->schema()->get("FileDialog::ActionButton");
            if (pActButton == NULL)
                return STATUS_BAD_STATE;
            pBMNormal       = pDisplay->schema()->get("FileDialog::Bookmark");
            if (pBMNormal == NULL)
                return STATUS_BAD_STATE;
            pBMSel          = pDisplay->schema()->get("FileDialog::Bookmark.selected");
            if (pBMSel == NULL)
                return STATUS_BAD_STATE;
            pWarning        = pDisplay->schema()->get("FileDialog::Warning");
            if (pWarning == NULL)
                return STATUS_BAD_STATE;
            pExtCheck       = pDisplay->schema()->get("FileDialog::ExtCheck");
            if (pExtCheck == NULL)
                return STATUS_BAD_STATE;

            // Initialize widgets
            LSP_STATUS_ASSERT(sWPath.init());
            sWPath.allocation()->set_hexpand(true);

            LSP_STATUS_ASSERT(sWSearch.init());

            LSP_STATUS_ASSERT(sWFilter.init());

            LSP_STATUS_ASSERT(sWFiles.init());
            sWFiles.constraints()->set_min(400, 320);
            sWFiles.allocation()->set_hexpand(true);

            LSP_STATUS_ASSERT(sWAction.init());
            LSP_STATUS_ASSERT(sWAction.style()->inject_parent(pActButton));

            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(sWCancel.style()->inject_parent(pActButton));
            LSP_STATUS_ASSERT(sWCancel.text()->set("actions.cancel"));

            LSP_STATUS_ASSERT(sWWarning.init());
            LSP_STATUS_ASSERT(sWWarning.style()->inject_parent(pWarning));
            sWWarning.visibility()->set(false);
            sWWarning.allocation()->set_hexpand(true);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.style()->inject_parent(pNavButton));
            LSP_STATUS_ASSERT(wGo.text()->set("actions.nav.go"));

            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.style()->inject_parent(pNavButton));
            LSP_STATUS_ASSERT(wUp.text()->set("actions.nav.up"));

            LSP_STATUS_ASSERT(sBMAdd.init());
            LSP_STATUS_ASSERT(sBMAdd.style()->inject_parent(pNavButton));
            LSP_STATUS_ASSERT(sBMAdd.text()->set("actions.to_bookmarks"));

            LSP_STATUS_ASSERT(wPathBox.init());
            wPathBox.orientation()->set_horizontal();
            wPathBox.allocation()->set_fill(true);
            wPathBox.spacing()->set(2);

            LSP_STATUS_ASSERT(sMainGrid.init());
            sMainGrid.rows()->set(7);
            sMainGrid.columns()->set(2);
            sMainGrid.hspacing()->set(4);
            sMainGrid.vspacing()->set(4);
            sMainGrid.orientation()->set(O_HORIZONTAL);

            LSP_STATUS_ASSERT(sHBox.init());
            sHBox.orientation()->set_horizontal();
            sHBox.spacing()->set(8);

            LSP_STATUS_ASSERT(sWarnBox.init());
            sWarnBox.orientation()->set_horizontal();
            sWarnBox.spacing()->set(8);

            LSP_STATUS_ASSERT(sSBBookmarks.init());
            sSBBookmarks.hscroll_mode()->set(SCROLL_OPTIONAL);
            sSBBookmarks.vscroll_mode()->set(SCROLL_OPTIONAL);
            sSBBookmarks.allocation()->set_expand(true);
            sSBBookmarks.constraints()->set_min_width(192);
            sSBBookmarks.layout()->set(-1.0f, -1.0f, 1.0f, 1.0f);

            LSP_STATUS_ASSERT(sSBAlign.init());
            sSBAlign.layout()->set_align(0.0f, -1.0f); // Middle, Top
            sSBAlign.layout()->set_scale(1.0f, 0.0f); // Maximum width, minimum height
            LSP_STATUS_ASSERT(sSBBookmarks.add(&sSBAlign));

            LSP_STATUS_ASSERT(sBookmarks.init());
            sBookmarks.orientation()->set_vertical();
            sBookmarks.allocation()->set_expand(true);
            LSP_STATUS_ASSERT(sSBAlign.add(&sBookmarks));

            LSP_STATUS_ASSERT(init_bm_popup_menu());

            // Initialize supplementary elements
            // Path box
            sBMAdd.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(wPathBox.add(&sBMAdd));
            LSP_STATUS_ASSERT(wPathBox.add(&wUp));
            LSP_STATUS_ASSERT(wPathBox.add(&wGo));
            LSP_STATUS_ASSERT(add_label(&wPathBox, "labels.location", 1.0f, &l));
            l->allocation()->set(true, true);
            l->padding()->set_left(8);
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
            LSP_STATUS_ASSERT(sMainGrid.add(&sHBox)); // Action button box

            // Add the whole structure
            LSP_STATUS_ASSERT(this->add(&sMainGrid));

            // Add slots
            handler_id_t id = 0;
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_CANCEL, slot_on_cancel, self());
            if (id >= 0) id = sWAction.slots()->bind(SLOT_SUBMIT, slot_on_btn_action, self());
            if (id >= 0) id = sWCancel.slots()->bind(SLOT_SUBMIT, slot_on_btn_cancel, self());
            if (id >= 0) id = sWSearch.slots()->bind(SLOT_CHANGE, slot_on_search, self());
            if (id >= 0) id = sWSearch.slots()->bind(SLOT_KEY_DOWN, slot_on_search_key_down, self());
            if (id >= 0) id = sWFilter.slots()->bind(SLOT_SUBMIT, slot_on_search, self());
            if (id >= 0) id = sWFilter.slots()->bind(SLOT_KEY_DOWN, slot_on_filter_key_down, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_MOUSE_DBL_CLICK, slot_on_list_dbl_click, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_REALIZED, slot_on_list_realized, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_KEY_DOWN, slot_on_list_key_down, self());
            if (id >= 0) id = wGo.slots()->bind(SLOT_SUBMIT, slot_on_go, self());
            if (id >= 0) id = wUp.slots()->bind(SLOT_SUBMIT, slot_on_up, self());
            if (id >= 0) id = sBMAdd.slots()->bind(SLOT_SUBMIT, slot_on_bm_add, self());
            if (id >= 0) id = sWPath.slots()->bind(SLOT_KEY_UP, slot_on_path_key_up, self());
            if (id >= 0) id = sBookmarks.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll, self());
            if (id >= 0) id = sSBBookmarks.slots()->bind(SLOT_REALIZED, slot_on_bm_realized, self());

            if (id < 0)
                return -id;

            // Bind properties
            sMode.bind("mode", &sStyle);
            sCustomAction.bind("custom.action", &sStyle);
            sActionText.bind(&sStyle, pDisplay->dictionary());
            sPath.bind(&sStyle, pDisplay->dictionary());
            sFilter.bind(&sStyle, pDisplay->dictionary());
            sSelFilter.bind("filter.selected", &sStyle);
            sSelected.bind(&sStyle, pDisplay->dictionary());
            sUseConfirm.bind("confirm", &sStyle);
            sConfirmMsg.bind(&sStyle, pDisplay->dictionary());

            // Bind foreign properties
            sBMTextColor.bind("text.color", pBMNormal);
            sBMBgColor.bind("bg.color", pBMNormal);
            sBMSelTextColor.bind("text.color", pBMSel);
            sBMSelBgColor.bind("bg.color", pBMSel);
            sWarnColor.bind("text.color", pWarning);
            sExtColor.bind("color", pExtCheck);

            // Sync mode
            sync_mode();

            return STATUS_OK;
        }

        void FileDialog::sync_mode()
        {
            if (sMode.open_file())
            {
                if (pWSearch != NULL)
                    pWSearch->text()->set("labels.search");
                sAppendExt.visibility()->set(false);
            }
            else if (sMode.save_file())
            {
                if (pWSearch != NULL)
                    pWSearch->text()->set("labels.file_name");
                sAppendExt.visibility()->set(true);
            }

            if (sCustomAction.get())
                sWAction.text()->set(&sActionText);
            else if (sMode.save_file())
                sWAction.text()->set("actions.save");
            else
                sWAction.text()->set("actions.open");
        }

        status_t FileDialog::init_bm_popup_menu()
        {
            LSP_STATUS_ASSERT(sBMPopup.init());
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.open", slot_on_bm_menu_open));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.link.follow", slot_on_bm_menu_follow));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.link.copy", slot_on_bm_menu_copy));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.delete", slot_on_bm_menu_delete));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, NULL, NULL));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_first", slot_on_bm_menu_first));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_up", slot_on_bm_menu_up));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_down", slot_on_bm_menu_down));
            LSP_STATUS_ASSERT(add_menu_item(&sBMPopup, "actions.edit.move_last", slot_on_bm_menu_last));

            return STATUS_OK;
        }

        void FileDialog::property_changed(Property *prop)
        {
            Window::property_changed(prop);

            if (sMode.is(prop))
                sync_mode();
            if (sCustomAction.is(prop))
                sync_mode();
            if (sActionText.is(prop))
                sync_mode();
            if (sPath.is(prop))
            {
                sWPath.text()->set(&sPath);
                if (sVisibility.get())
                    refresh_current_path();
            }
            if (sFilter.is(prop))
            {
                if (sVisibility.get())
                {
                    sync_filters();
                    refresh_current_path();
                }
            }
            if (sSelFilter.is(prop))
            {
                if (sVisibility.get())
                    refresh_current_path();
            }
        }

        status_t FileDialog::add_label(WidgetContainer *c, const char *key, float align, Label **label)
        {
            Label *lbl = new Label(pDisplay);
            if (lbl == NULL)
                return STATUS_NO_MEM;

            status_t result = (vWidgets.add(lbl)) ? STATUS_OK : STATUS_NO_MEM;

            if (result == STATUS_OK)
                result = lbl->init();
            if (result == STATUS_OK)
                result = lbl->text()->set(key);
            if (result == STATUS_OK)
                result = c->add(lbl);
            lbl->text_layout()->set_halign(align);

            if (result != STATUS_OK)
            {
                vWidgets.premove(lbl);
                lbl->destroy();
                delete lbl;
            }

            if (label != NULL)
                *label = lbl;

            return result;
        }

        status_t FileDialog::add_menu_item(Menu *m, const char *key, event_handler_t handler)
        {
            MenuItem *mi = new MenuItem(pDisplay);
            if (mi == NULL)
                return STATUS_NO_MEM;
            if (!vWidgets.add(mi))
            {
                mi->destroy();
                delete mi;
                return STATUS_NO_MEM;
            }

            LSP_STATUS_ASSERT(mi->init());
            if (key != NULL)
            {
                LSP_STATUS_ASSERT(mi->text()->set(key));
                handler_id_t id = mi->slots()->bind(SLOT_SUBMIT, handler, self());
                if (id < 0)
                    return STATUS_UNKNOWN_ERR;
            }
            else
                mi->type()->set_separator();

            LSP_STATUS_ASSERT(m->add(mi));

            return STATUS_OK;
        }

        status_t FileDialog::add_ext_button(WidgetContainer *c, const char *text)
        {
            LSP_STATUS_ASSERT(sAppendExt.init());
            LSP_STATUS_ASSERT(wAutoExt.init());
            LSP_STATUS_ASSERT(wAutoExt.style()->inject_parent(pExtCheck));

            Label *lbl = new Label(pDisplay);
            if (lbl == NULL)
                return STATUS_NO_MEM;

            Box *box = new Box(pDisplay);
            if (box == NULL)
            {
                delete lbl;
                return STATUS_NO_MEM;
            }
            box->orientation()->set_horizontal();

            status_t result = (vWidgets.add(lbl)) ? STATUS_OK : STATUS_NO_MEM;
            if (result == STATUS_OK)
                result = (vWidgets.add(box)) ? STATUS_OK : STATUS_NO_MEM;

            if (result == STATUS_OK)
                result = lbl->init();
            if (result == STATUS_OK)
                result = box->init();

            box->spacing()->set(4);
            sAppendExt.layout()->set_align(-1.0f);

            if (result == STATUS_OK)
                result = lbl->text()->set(text);
            if (result == STATUS_OK)
                result = sAppendExt.add(box);
            if (result == STATUS_OK)
                result = box->add(&wAutoExt);
            if (result == STATUS_OK)
                result = box->add(lbl);
            if (result == STATUS_OK)
                result = c->add(&sAppendExt);

            if (result != STATUS_OK)
            {
                vWidgets.premove(lbl);
                vWidgets.premove(box);
                lbl->destroy();
                delete lbl;
                box->destroy();
                delete box;
            }

            return result;
        }

        status_t FileDialog::slot_on_submit(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_submit() : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_cancel(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_cancel() : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_btn_action(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_action(data, false) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_btn_cancel(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_cancel(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_confirm(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_confirm(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_search(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_search(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_list_dbl_click(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_list_dbl_click(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_list_change(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_list_change(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_list_realized(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            ListBox *list   = widget_ptrcast<ListBox>(sender);
            if ((dlg == NULL) || (list == NULL))
                return STATUS_OK;

            ssize_t n = list->items()->size();
            if (n <= 0)
                return n;
            float delta = (4.0f * list->vscroll()->range()) / n;
            list->vstep()->set(delta);
            list->vaccel_step()->set(delta * 2.0f);
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_list_key_down(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            const ws::event_t *ev = static_cast<ws::event_t *>(data);
            ws::code_t key = KeyboardHandler::translate_keypad(ev->nCode);

            switch (key)
            {
                case ws::WSK_KEYPAD_ENTER:
                case ws::WSK_RETURN:
                    return (dlg != NULL) ? dlg->on_dlg_list_dbl_click(data) : STATUS_BAD_STATE;
                case ws::WSK_BACKSPACE:
                    dlg->on_dlg_up(data);
                    break;
                case ws::WSK_ESCAPE:
                    dlg->on_dlg_cancel(data);
                    break;
                default:
                    break;
            }
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_filter_key_down(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            const ws::event_t *ev = static_cast<ws::event_t *>(data);
            ws::code_t key = KeyboardHandler::translate_keypad(ev->nCode);

            switch (key)
            {
                case ws::WSK_BACKSPACE:
                    dlg->on_dlg_up(data);
                    break;
                case ws::WSK_ESCAPE:
                    dlg->on_dlg_cancel(data);
                    break;
                default:
                    break;
            }
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_search_key_down(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            const ws::event_t *ev = static_cast<ws::event_t *>(data);
            ws::code_t key = KeyboardHandler::translate_keypad(ev->nCode);

            switch (key)
            {
                case ws::WSK_ESCAPE:
                {
                    LSPString pattern;
                    LSP_STATUS_ASSERT(dlg->sWSearch.text()->format(&pattern));
                    if (pattern.is_empty())
                        return dlg->on_dlg_cancel(data);

                    dlg->sWSearch.text()->set_raw("");
                    return dlg->on_dlg_search(data);
                }
                default:
                    break;
            }
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_go(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_go(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_up(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_up(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_bm_add(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            if (dlg != NULL)
                dlg->add_new_bookmark();
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_path_key_up(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            ws::event_t *ev  = static_cast<ws::event_t *>(data);
            return (dlg != NULL) ? dlg->on_path_key_up(ev) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_bm_submit(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            return (_this != NULL) ? _this->on_bm_submit(sender) : STATUS_BAD_ARGUMENTS;
        }

        status_t FileDialog::slot_on_bm_scroll(Widget *sender, void *ptr, void *data)
        {
            ScrollArea *_parent = parent_widget<ScrollArea>(sender);
            ws::event_t *ev = static_cast<ws::event_t *>(data);
            return (_parent != NULL) ? _parent->handle_event(ev) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_popup(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            Widget *_widget = widget_ptrcast<Widget>(data);
            _this->pPopupBookmark = _this->find_bookmark(_widget);
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_open(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;
            return ((_this != NULL) && (bm != NULL)) ? _this->on_bm_submit(&bm->sHlink) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_follow(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;

            return ((_this != NULL) && (bm != NULL)) ? bm->sHlink.follow_url() : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_copy(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;

            return ((_this != NULL) && (bm != NULL)) ? bm->sHlink.copy_url(ws::CBUF_CLIPBOARD) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_delete(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;

            return ((_this != NULL) && (bm != NULL)) ? _this->remove_bookmark(bm) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_up(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;
            ssize_t idx = bm ? _this->vBookmarks.index_of(bm) : -1;

            // Find previous visible bookmark
            ssize_t prev = idx - 1;
            for ( ; prev >= 0; --prev)
            {
                bm_entry_t *ent = _this->vBookmarks.uget(prev);
                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
                    break;
            }

            if (prev < 0)
                return STATUS_OK;

            return (_this->vBookmarks.xswap(prev, idx)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t FileDialog::slot_on_bm_menu_down(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            ssize_t items = _this->vBookmarks.size();
            bm_entry_t *bm = _this->pPopupBookmark;
            ssize_t idx = bm ? _this->vBookmarks.index_of(bm) : -1;

            // Find next visible bookmark
            ssize_t next = idx + 1;
            for ( ; next < items; ++next)
            {
                bm_entry_t *ent = _this->vBookmarks.uget(next);
                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
                    break;
            }

            if (next >= items)
                return STATUS_OK;

            return (_this->vBookmarks.xswap(idx, next)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
        }

        status_t FileDialog::slot_on_bm_menu_first(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;
            if (bm == NULL)
                return STATUS_OK;

            bm_entry_t **dst = _this->vBookmarks.prepend((bm_entry_t *)(NULL));
            if (dst == NULL)
                return STATUS_UNKNOWN_ERR;
            _this->vBookmarks.premove(bm);
            *dst        = bm;

            return _this->sync_bookmarks();
        }

        status_t FileDialog::slot_on_bm_menu_last(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;
            if (bm == NULL)
                return STATUS_OK;

            bm_entry_t **dst = _this->vBookmarks.append((bm_entry_t *)(NULL));
            if (dst == NULL)
                return STATUS_UNKNOWN_ERR;
            _this->vBookmarks.premove(bm);
            *dst        = bm;

            return _this->sync_bookmarks();
        }

        status_t FileDialog::slot_on_bm_realized(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            ScrollArea *_area = widget_cast<ScrollArea>(sender);
            if ((_this == NULL) || (_area == NULL))
                return STATUS_OK;
            size_t n    = _this->vBookmarks.size();
            if (n <= 0)
                return STATUS_OK;

            ws::rectangle_t sa, sb;
            _this->sBookmarks.get_rectangle(&sa);
            _this->sBookmarks.get_rectangle(&sb);
            float ydelta    = float(sb.nHeight) / n;

            if (sa.nHeight >= (ydelta * 4))
                ydelta         *= 4;

            float smin      = _area->vscroll()->min();
            float smax      = _area->vscroll()->max();
            float sdelta    = (smax - smin) / n;

            float step      = lsp_max(sdelta, ydelta);
            _area->vstep()->set(step);
            _area->vaccel_step()->set(step * 2.0f);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_list_dbl_click(void *data)
        {
            f_entry_t *ent = selected_entry();
            if (ent == NULL)
                return STATUS_OK;

            // Analyze what to do
            LSPString spath;
            io::Path path;

            if (ent->nFlags & F_DOTDOT)
                return on_dlg_up(NULL);

            if (ent->nFlags & F_ISDIR)
            {
                LSP_STATUS_ASSERT(sPath.format(&spath));
                LSP_STATUS_ASSERT(path.set(&spath));
                LSP_STATUS_ASSERT(path.append_child(&ent->sName));
                LSP_STATUS_ASSERT(path.canonicalize());

                return sPath.set_raw(path.as_string());
            }

            return on_dlg_action(data, true);
        }

        status_t FileDialog::on_dlg_list_change(void *data)
        {
            if (!sMode.save_file())
                return STATUS_OK;

            f_entry_t *ent = selected_entry();
            if (ent == NULL)
                return STATUS_OK;

            // Analyze what to do
            if ((ent->nFlags & F_DOTDOT) || (ent->nFlags & F_ISDIR))
                return STATUS_OK;

            return sWSearch.text()->set_raw(&ent->sName);
        }

        status_t FileDialog::on_dlg_search(void *data)
        {
            return (sVisibility.get()) ? apply_filters() : STATUS_OK;
        }

        status_t FileDialog::on_dlg_action(void *data, bool list)
        {
            LSPString fname, spath;
            io::Path path;

            LSP_STATUS_ASSERT(sWSearch.text()->format(&fname));
            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));

            if (list)
            {
                f_entry_t *ent = selected_entry();
                if (ent == NULL)
                    return show_message("titles.attention", "headings.attention", "messages.file.not_specified", NULL);

                // Analyze what to do
                if (ent->nFlags & F_DOTDOT)
                    return on_dlg_up(NULL);

                LSP_STATUS_ASSERT(path.append_child(&ent->sName));
                if (ent->nFlags & F_ISDIR)
                    return sPath.set_raw(path.as_utf8());
            }
            else if (sMode.save_file()) // Use 'File name' field
            {
                if (wAutoExt.down()->get())
                {
                    LSPString ext;
                    ListBoxItem *isel = sWFilter.selected()->get();
                    FileMask *mask = sFilter.get((isel != NULL) ? isel->tag()->get() : 0);
                    if (mask != NULL)
                        mask->append_extension(&fname);
                }

                LSP_STATUS_ASSERT(path.append_child(&fname));

                if (io::Path::is_dots(&fname) || (!io::Path::valid_file_name(&fname)))
                    return show_message("titles.attention", "headings.attention", "messages.file.invalid_name", &path);
            }
            else if ((!io::Path::is_dots(&fname)) && (io::Path::valid_file_name(&fname)))
            {
                LSP_STATUS_ASSERT(path.append_child(&fname));
            }
            else // Use selection
            {
                f_entry_t *ent = selected_entry();
                if (ent == NULL)
                    return show_message("titles.attention", "headings.attention", "messages.file.not_specified", NULL);

                // Analyze what to do
                if (ent->nFlags & F_DOTDOT)
                    return on_dlg_up(NULL);

                LSP_STATUS_ASSERT(path.append_child(&ent->sName));
                if (ent->nFlags & F_ISDIR)
                    return sPath.set_raw(path.as_utf8());
            }

            LSP_STATUS_ASSERT(sSelected.set_raw(path.as_string()));

            // Special case for saving file
            io::fattr_t fattr;
            status_t stat_result = io::File::sym_stat(&path, &fattr);

            if (sMode.save_file())
            {
                if (!sUseConfirm.get())
                    return on_dlg_confirm(data);

                // Check that file exists and avoid confirmation if it doesn't
                lsp_trace("Checking file: %s", path.as_utf8());
                if (stat_result != STATUS_OK)
                    return on_dlg_confirm(data);
            }
            else
            {
                if (stat_result != 0)
                    return show_message("titles.attention", "headings.attention", "messages.file.not_exists", &path);

                if (!sUseConfirm.get())
                    return on_dlg_confirm(data);
            }

            if (pWConfirm == NULL)
            {
                // Create dialog object
                pWConfirm = new MessageBox(pDisplay);
                if (pWConfirm == NULL)
                    return STATUS_NO_MEM;
                status_t res = pWConfirm->init();
                if (res != STATUS_OK)
                {
                    pWConfirm->destroy();
                    delete pWConfirm;
                    pWConfirm = NULL;
                    return res;
                }

                LSP_STATUS_ASSERT(pWConfirm->title()->set("titles.confirmation"));
                LSP_STATUS_ASSERT(pWConfirm->heading()->set("headings.confirmation"));
                LSP_STATUS_ASSERT(pWConfirm->add("actions.confirm.yes", slot_on_confirm, self()));
                LSP_STATUS_ASSERT(pWConfirm->add("actions.confirm.no"));
                pWConfirm->buttons()->get(0)->constraints()->set_min_width(96);
                pWConfirm->buttons()->get(1)->constraints()->set_min_width(96);
            }

            LSP_STATUS_ASSERT(pWConfirm->message()->set(&sConfirmMsg));
            LSP_STATUS_ASSERT(path.get_parent(&spath));
            LSP_STATUS_ASSERT(pWConfirm->message()->params()->set_string("path", &spath));
            LSP_STATUS_ASSERT(path.get_last(&spath));
            LSP_STATUS_ASSERT(pWConfirm->message()->params()->set_string("name", &spath));
            LSP_STATUS_ASSERT(pWConfirm->message()->params()->set_string("file", path.as_string()));

            pWConfirm->show(this);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_confirm(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            hide();
            destroy_file_entries(&vFiles);
            drop_bookmarks();

            // Execute slots
            return sSlots.execute(SLOT_SUBMIT, this, data);
        }

        status_t FileDialog::on_dlg_cancel(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            drop_bookmarks();
            hide();
            destroy_file_entries(&vFiles);

            // Execute slots
            return sSlots.execute(SLOT_CANCEL, this, data);
        }

        status_t FileDialog::on_show()
        {
            sync_filters();
            refresh_bookmarks();
            refresh_current_path();
            return STATUS_OK;
        }

        status_t FileDialog::on_close(const ws::event_t *e)
        {
            ws::event_t ev = *e;
            return on_dlg_cancel(&ev);
        }

        status_t FileDialog::on_bm_submit(Widget *sender)
        {
            bm_entry_t *bm = find_bookmark(sender);
            return (bm != NULL) ? sPath.set_raw(&bm->sBookmark.path) : STATUS_OK;
        }

        status_t FileDialog::on_dlg_go(void *data)
        {
            io::Path path;
            LSPString spath;
            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));
            LSP_STATUS_ASSERT(path.canonicalize());

            return sPath.set_raw(path.as_string());
        }

        status_t FileDialog::on_dlg_up(void *data)
        {
            io::Path path;
            LSPString spath;

            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));
            LSP_STATUS_ASSERT(path.remove_last());
            LSP_STATUS_ASSERT(path.canonicalize());

            return sPath.set_raw(path.as_string());
        }

        status_t FileDialog::on_path_key_up(ws::event_t *e)
        {
            lsp_trace("Path key code released=%x, modifiers=%x", int(e->nCode), int(e->nState));
            ws::code_t key = KeyboardHandler::translate_keypad(e->nCode);
            switch (key)
            {
                case ws::WSK_RETURN:
                    return on_dlg_go(e);

                case ws::WSK_ESCAPE:
                    return on_dlg_cancel(e);

                default:
                    break;
            }
            return STATUS_OK;
        }

        status_t FileDialog::read_lsp_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_user_config_path(&path);
            if (res != STATUS_OK)
                return res;

            const char *bm_path = pDisplay->environment()->get_utf8(LSP_TK_ENV_CONFIG, LSP_TK_ENV_CONFIG_DFL);
            if ((res = path.append_child(bm_path)) != STATUS_OK)
                return res;
            if ((res = path.append_child("bookmarks.json")) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks(&vbm, &path);
        }

        status_t FileDialog::read_gtk2_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(GTK2_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_gtk2(&vbm, &path);
        }

        status_t FileDialog::read_gtk3_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(GTK3_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_gtk3(&vbm, &path);
        }

        status_t FileDialog::read_qt5_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(QT5_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_qt5(&vbm, &path);
        }

        status_t FileDialog::refresh_bookmarks()
        {
            drop_bookmarks();
            LSPString url;

            // Read LSP bookmarks
            lltl::parray<bookmarks::bookmark_t> bm, tmp;
            status_t res, xres;
            size_t changes = 0;

            // Read bookmarks from different sources and merge
            xres = read_lsp_bookmarks(bm);
            if ((res = read_gtk2_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_GTK2);
            if ((res = read_gtk3_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_GTK3);
            if ((res = read_qt5_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_QT5);
            bookmarks::destroy_bookmarks(&tmp);

            // Check if we need to store bookmarks
            if ((changes > 0) || (xres != STATUS_OK))
                save_bookmarks(&bm);

            // Create widgets
            bm_entry_t *ent = NULL;
            for (size_t i=0, n=bm.size(); i<n; ++i)
            {
                bookmarks::bookmark_t *b = bm.uget(i);
                if (b == NULL)
                    continue;

                // Allocate entry
                if ((ent = new bm_entry_t(pDisplay)) == NULL)
                {
                    res = STATUS_NO_MEM;
                    break;
                }

                // Initialize data
                if ((res = ent->sPath.set(&b->path)) != STATUS_OK)
                    break;
                if ((res = ent->sPath.canonicalize()) != STATUS_OK)
                    break;
                if ((res = ent->sHlink.init()) != STATUS_OK)
                    break;
                if ((res = ent->sHlink.text()->set_raw(&b->name)) != STATUS_OK)
                    break;
                res = (url.set_ascii("file://")) ? STATUS_OK : STATUS_NO_MEM;
                if (res == STATUS_OK)
                    res = (url.append(&b->path)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;

                ent->sHlink.style()->inject_parent(pBMNormal);
                ent->sHlink.url()->set_raw(&url);

                ent->sHlink.slots()->bind(SLOT_SUBMIT, slot_on_bm_submit, self());
                ent->sHlink.slots()->bind(SLOT_BEFORE_POPUP, slot_on_bm_popup, self());
                ent->sHlink.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll);
                ent->sHlink.popup()->set(&sBMPopup);
                if (b->origin & bookmarks::BM_LSP)
                {
                    if ((res = sBookmarks.add(&ent->sHlink)) != STATUS_OK)
                        break;
                }
                ent->sBookmark.path.swap(&b->path);
                ent->sBookmark.name.swap(&b->name);
                ent->sBookmark.origin = b->origin;

                // Commit to list if
                res = (vBookmarks.add(ent)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;
            }

            bookmarks::destroy_bookmarks(&bm);

            if (res != STATUS_OK)
            {
                drop_bookmarks();
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
                return res;
            }

            return select_current_bookmark();
        }

        status_t FileDialog::save_bookmarks(lltl::parray<bookmarks::bookmark_t> *vbm)
        {
            io::Path path, parent;
            lltl::parray<bookmarks::bookmark_t> tmp;

            status_t res = system::get_user_config_path(&path);
            if (res != STATUS_OK)
                return res;

            const char *bm_path = pDisplay->environment()->get_utf8(LSP_TK_ENV_CONFIG, LSP_TK_ENV_CONFIG_DFL);
            if ((res = path.append_child(bm_path)) != STATUS_OK)
                return res;
            if ((res = path.append_child("bookmarks.json")) != STATUS_OK)
                return res;

            // Create directory
            if ((res = path.get_parent(&parent)) != STATUS_OK)
                return res;
            if ((res = parent.mkdir(true)) != STATUS_OK)
                return res;

            if (vbm == NULL)
            {
                // Build list of bookmarks
                for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
                {
                    bm_entry_t *ent = vBookmarks.uget(i);
                    if ((ent != NULL) && (!tmp.add(&ent->sBookmark)))
                    {
                        tmp.flush();
                        return STATUS_NO_MEM;
                    }
                }

                vbm = &tmp;
            }

            return bookmarks::save_bookmarks(vbm, &path);
        }

        status_t FileDialog::remove_bookmark(bm_entry_t *entry)
        {
            // Make invisible for LSP toolkit
            entry->sBookmark.origin &= ~bookmarks::BM_LSP;
            sBookmarks.remove(&entry->sHlink);
            if (pSelBookmark == entry)
                pSelBookmark = NULL;
            if (pPopupBookmark == entry)
                pPopupBookmark = NULL;

            // Drop entry if origin is emtpy
            if (!entry->sBookmark.origin)
            {
                vBookmarks.premove(entry);
                entry->sHlink.destroy();
                delete entry;
            }

            // Synchronize bookmarks and save state
            return sync_bookmarks();
        }


        status_t FileDialog::select_current_bookmark()
        {
            status_t res;
            LSPString spath;
            io::Path path;

            // Get path and canonicalize
            if ((res = sWPath.text()->format(&spath)) != STATUS_OK)
                return res;
            if ((res = path.set(&spath)) != STATUS_OK)
                return res;
            if ((res = path.canonicalize()) != STATUS_OK)
                return res;

            bm_entry_t *found = NULL;
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    found = ent;
                    break;
                }
            }

            // Check state
            if (found == pSelBookmark)
                return STATUS_OK;

            // Deactivate selected bookmark
            if (pSelBookmark != NULL)
            {
                pSelBookmark->sHlink.style()->remove_parent(pBMSel);
                pSelBookmark->sHlink.style()->inject_parent(pBMNormal);
            }
            pSelBookmark = found;
            if (pSelBookmark != NULL)
            {
                pSelBookmark->sHlink.style()->remove_parent(pBMNormal);
                pSelBookmark->sHlink.style()->inject_parent(pBMSel);
            }

            return STATUS_OK;
        }

        status_t FileDialog::sync_bookmarks()
        {
            status_t res;
            if ((res = sBookmarks.remove_all()) != STATUS_OK)
                return res;

            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if ((ent == NULL) || (!(ent->sBookmark.origin & bookmarks::BM_LSP)))
                    continue;
                if ((res = sBookmarks.add(&ent->sHlink)) != STATUS_OK)
                    break;
            }

            if (res != STATUS_OK)
                sBookmarks.remove_all();

            return (res == STATUS_OK) ? save_bookmarks(NULL) : res;
        }

        FileDialog::bm_entry_t *FileDialog::find_bookmark(Widget *sender)
        {
            Hyperlink *hlink = widget_cast<Hyperlink>(sender);
            if (hlink == NULL)
                return NULL;

            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if ((ent != NULL) && (hlink == &ent->sHlink))
                    return ent;
            }
            return NULL;
        }

        status_t FileDialog::add_new_bookmark()
        {
            status_t res;
            LSPString spath;
            io::Path path;

            // Get current path
            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));

            // Get selected entry
            f_entry_t *fent = selected_entry();
            if ((fent != NULL) && ((fent->nFlags & (F_ISDIR | F_DOTDOT)) == F_ISDIR))
            {
                if ((res = path.append_child(&fent->sName)) != STATUS_OK)
                    return res;
            }

            lsp_trace("Add bookmark path=%s", path.as_native());

            // Canonicalize path
            if ((res = path.canonicalize()) != STATUS_OK)
                return res;
            if (!path.is_dir())
                return STATUS_NOT_DIRECTORY;

            // Now seek that item is already present
            bm_entry_t *ent;
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                ent = vBookmarks.uget(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    if (ent->sBookmark.origin & bookmarks::BM_LSP)
                        return STATUS_ALREADY_EXISTS;
                    if ((res = path.get_last(&ent->sBookmark.name)) != STATUS_OK)
                        return res;
                    ent->sBookmark.origin |= bookmarks::BM_LSP;
                    return sync_bookmarks();
                }
            }

            // Create new bookmark
            if ((ent = new bm_entry_t(pDisplay)) == NULL)
                return STATUS_NO_MEM;
            if (!vBookmarks.add(ent))
            {
                delete ent;
                return STATUS_NO_MEM;
            }

            if ((res = init_bookmark_entry(ent, &path)) != STATUS_OK)
            {
                vBookmarks.premove(ent);
                ent->sHlink.destroy();
                delete ent;
                return STATUS_NO_MEM;
            }

            // Synchronize bookmarks and exit
            return sync_bookmarks();
        }

        status_t FileDialog::init_bookmark_entry(bm_entry_t *ent, const io::Path *path)
        {
            status_t res;
            LSPString url;

            ent->sBookmark.origin   = bookmarks::BM_LSP;
            if ((res = path->get_last(&ent->sBookmark.name)) != STATUS_OK)
                return res;
            if ((res = path->get(&ent->sBookmark.path)) != STATUS_OK)
                return res;

            // Initialize data
            if ((res = ent->sPath.set(path)) != STATUS_OK)
                return res;
            if ((res = ent->sHlink.init()) != STATUS_OK)
                return res;
            if ((res = ent->sHlink.text()->set_raw(&ent->sBookmark.name)) != STATUS_OK)
                return res;
            if ((res = path->get(&url)) != STATUS_OK)
                return res;
            if (!url.prepend_ascii("file://"))
                return STATUS_NO_MEM;

            ent->sHlink.text_layout()->set_halign(-1.0f);
            ent->sHlink.follow()->set(false);
            ent->sHlink.url()->set_raw(&url);
            ent->sHlink.padding()->set_horizontal(8, 8);
            ent->sHlink.padding()->set_vertical(2, 2);
            ent->sHlink.slots()->bind(SLOT_SUBMIT, slot_on_bm_submit, self());
            ent->sHlink.slots()->bind(SLOT_BEFORE_POPUP, slot_on_bm_popup, self());
            ent->sHlink.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll);
            ent->sHlink.popup()->set(&sBMPopup);

            return STATUS_OK;
        }

        status_t FileDialog::on_submit()
        {
            return STATUS_OK;
        }

        status_t FileDialog::on_cancel()
        {
            return STATUS_OK;
        }

        status_t FileDialog::refresh_current_path()
        {
            lltl::parray<f_entry_t> scanned;
            LSPString str, path;
            status_t xres;

            // Obtain the path to working directory
            io::Path xpath;
            xres = sPath.format(&path);
            if ((xres == STATUS_OK) && (path.length() > 0))
                xres = xpath.set(&path); // Directory is specified, use it
            else
            {
                xres = xpath.current(); // Directory is not specified, use current
                if (xres == STATUS_OK)
                {
                    sPath.commit_raw(xpath.as_string());
                    sWPath.text()->set_raw(xpath.as_string());
                }
            }
            if ((xres == STATUS_OK) && (!xpath.is_root())) // Need to add dotdot entry?
                xres = add_file_entry(&scanned, "..", F_DOTDOT);

            if (xres != STATUS_OK) // Check result
            {
                destroy_file_entries(&scanned);
                return xres;
            }

            // Open directory for reading
            io::Dir dir;
            xres = dir.open(&xpath);
            if (xres == STATUS_OK)
            {
                sWWarning.hide();

                // Read directory
                io::fattr_t fattr;
                io::Path fname;

                while (dir.reads(&fname, &fattr, false) == STATUS_OK)
                {
                    // Reject dot and dotdot from search
                    if ((fname.is_dot()) || (fname.is_dotdot()))
                        continue;

                    // Analyze file flags
                    size_t nflags = 0;
                    if (fname.as_string()->first() == '.')
                        nflags      |= F_ISHIDDEN;

                    if (fattr.type == io::fattr_t::FT_DIRECTORY) // Directory?
                        nflags      |= F_ISDIR;
                    else if (fattr.type == io::fattr_t::FT_SYMLINK) // Symbolic link?
                        nflags      |= F_ISLINK;
                    else if (fattr.type == io::fattr_t::FT_REGULAR)
                        nflags      |= F_ISREG;
                    else
                        nflags      |= F_ISOTHER;

                    if (nflags & F_ISLINK)
                    {
                        // Stat a file associated with symbolic link
                        xres = dir.sym_stat(&fname, &fattr);

                        if (xres != STATUS_OK)
                            nflags      |= F_ISINVALID;
                        else if (fattr.type == io::fattr_t::FT_DIRECTORY) // Directory?
                            nflags      |= F_ISDIR;
                        else if (fattr.type == io::fattr_t::FT_SYMLINK) // Symbolic link?
                            nflags      |= F_ISLINK;
                        else if (fattr.type == io::fattr_t::FT_REGULAR)
                            nflags      |= F_ISREG;
                        else
                            nflags      |= F_ISOTHER;
                    }

                    // Add entry to list of found files
                    if ((xres = add_file_entry(&scanned, fname.as_native(), nflags)) != STATUS_OK)
                    {
                        dir.close();
                        destroy_file_entries(&scanned);
                        return xres;
                    }
                }

                // Close directory
                if (dir.close() != STATUS_OK)
                {
                    destroy_file_entries(&scanned);
                    return STATUS_IO_ERROR;
                }
            }
            else // Analyze errcode
            {
                const char *text = "unknown I/O error";
                switch (xres)
                {
                    case STATUS_PERMISSION_DENIED:    text = "permission denied"; break;
                    case STATUS_NOT_FOUND:    text = "directory does not exist"; break;
                    case STATUS_NO_MEM:    text = "not enough memory"; break;
                    default: break;
                }

                str.set_native("Access error: ");
                path.set_native(text);
                str.append(&path);
                sWWarning.text()->set_raw(&str);
                sWWarning.show();
            }

            // Now we have the complete list of files, need to reorder them
            scanned.qsort(cmp_file_entry);

            // Alright, now we can swap contents and delete previous contents
            vFiles.swap(&scanned);
            destroy_file_entries(&scanned);

            apply_filters();

            return select_current_bookmark();
        }

        int FileDialog::cmp_file_entry(const f_entry_t *a, const f_entry_t *b)
        {
            ssize_t delta = ((b->nFlags & F_DOTDOT) - (a->nFlags & F_DOTDOT));
            if (delta != 0)
                return delta;
            delta = ((b->nFlags & F_ISDIR) - (a->nFlags & F_ISDIR));
            if (delta != 0)
                return delta;
            return a->sName.compare_to(&b->sName);
        }

        status_t FileDialog::add_file_entry(lltl::parray<f_entry_t> *dst, const char *name, size_t flags)
        {
            LSPString xname;
            if (!xname.set_utf8(name))
                return STATUS_NO_MEM;
            return add_file_entry(dst, &xname, flags);
        }

        status_t FileDialog::add_file_entry(lltl::parray<f_entry_t> *dst, const LSPString *name, size_t flags)
        {
            f_entry_t *ent = new f_entry_t();
            if (ent == NULL)
                return STATUS_NO_MEM;
            if (!ent->sName.set(name))
            {
                delete ent;
                return STATUS_NO_MEM;
            }
            ent->nFlags     = flags;

            if (!dst->add(ent))
            {
                delete ent;
                return STATUS_NO_MEM;
            }

            return STATUS_OK;
        }

        FileDialog::f_entry_t *FileDialog::selected_entry()
        {
            ListBoxItem *item = sWFiles.selected()->any();
            if (item == NULL)
                return NULL;
            ssize_t index = item->tag()->get();
            if (index < 0)
                return NULL;
            return vFiles.get(index);
        }

        status_t FileDialog::sync_filters()
        {
            status_t res;
            sWFilter.items()->clear();
            ssize_t index = (sFilter.size() > 0) ?
                    lsp_limit(sSelFilter.get(), 0, ssize_t(sFilter.size()) - 1) : -1;

            // Add new items
            for (size_t i=0, n=sFilter.size(); i<n; ++i)
            {
                FileMask *fm = sFilter.get(i);
                ListBoxItem *item = new ListBoxItem(pDisplay);
                if (item == NULL)
                    return STATUS_NO_MEM;
                res = item->init();
                if (res == STATUS_OK)
                    res = item->text()->set(fm->title());
                if (res == STATUS_OK)
                    item->tag()->set(i);
                if (res == STATUS_OK)
                    res = sWFilter.items()->madd(item); // Add in managed mode
                if (res != STATUS_OK)
                {
                    item->destroy();
                    delete item;
                    return res;
                }
            }

            // Update index
            ListBoxItem *sel = (index >= 0) ? sWFilter.items()->get(index) : NULL;
            sWFilter.selected()->set(sel);
            sSelFilter.commit_value(index);

            return STATUS_OK;
        }

        status_t FileDialog::apply_filters()
        {
            LSPString tmp, xfname, *psrc = NULL;
            io::PathPattern *psmask = NULL, smask;
            FileMask *fmask = NULL;

            // Initialize masks
            if (sMode.get() == FDM_OPEN_FILE) // Additional filtering is available only when opening file
            {
                LSP_STATUS_ASSERT(sWSearch.text()->format(&tmp));
                if (tmp.length() > 0)
                {
                    if (!tmp.prepend('*'))
                        return STATUS_NO_MEM;
                    if (!tmp.append('*'))
                        return STATUS_NO_MEM;
                    LSP_STATUS_ASSERT(smask.set(&tmp));
                    psmask = &smask;
                }
            }
            else
            {
                sWFiles.selected()->clear();
                LSP_STATUS_ASSERT(sWSearch.text()->format(&xfname));
            }

            if (sWFilter.items()->size() > 0)
            {
                ListBoxItem *sel = sWFilter.selected()->get();
                ssize_t tag      = (sel != NULL) ? sel->tag()->get() : -1;
                fmask            = (tag >= 0) ? sFilter.get(tag) : NULL;
            }

            // Now we need to fill data
            WidgetList<ListBoxItem> *lst = sWFiles.items();
            lst->clear();
            float xs = sWFiles.hscroll()->get(), ys = sWFiles.vscroll()->get(); // Remember scroll values

            // Process files
            for (size_t i=0, n=vFiles.size(); i<n; ++i)
            {
                f_entry_t *ent = vFiles.uget(i);
                psrc = &ent->sName;

                // Pass entry name through filter
                if (!(ent->nFlags & (F_ISDIR | F_DOTDOT)))
                {
                    // Process with masks
                    if ((fmask != NULL) && (!fmask->test(psrc)))
                        continue;
                    if ((psmask != NULL) && (!psmask->test(psrc)))
                        continue;
                }

                // Add some special characters
                if (ent->nFlags & (F_ISOTHER | F_ISDIR | F_ISLINK | F_ISINVALID))
                {
                    if (!tmp.set(psrc))
                    {
                        lst->clear();
                        return STATUS_NO_MEM;
                    }
                    psrc = &tmp;

                    // Modify the name of the item
                    bool ok = true;
                    if (ent->nFlags & F_ISOTHER)
                        ok = ok && psrc->prepend('*');
                    else if (ent->nFlags & (F_ISLINK | F_ISINVALID))
                        ok = ok && psrc->prepend((ent->nFlags & F_ISINVALID) ? '!' : '~');

                    if (ent->nFlags & F_ISDIR)
                    {
                        ok = ok && psrc->prepend('[');
                        ok = ok && psrc->append(']');
                    }

                    if (!ok)
                    {
                        lst->clear();
                        return STATUS_NO_MEM;
                    }
                }

                // Add item
                ListBoxItem *item = new ListBoxItem(pDisplay);
                if (item == NULL)
                    return STATUS_NO_MEM;
                LSP_STATUS_ASSERT(item->init());
                item->text()->set_raw(psrc);
                item->tag()->set(i);
                LSP_STATUS_ASSERT(lst->madd(item));

                // Check if is equal
                if ((!(ent->nFlags & (F_ISDIR | F_DOTDOT))) && (xfname.length() > 0))
                {
//                    lsp_trace("  %s <-> %s", ent->sName.get_native(), xfname.get_native());
                    #ifdef PLATFORM_WINDOWS
                    if (ent->sName.equals_nocase(&xfname))
                        sWFiles.selected()->add(item);
                    #else
                    if (ent->sName.equals(&xfname))
                        sWFiles.selected()->add(item);
                    #endif /* PLATFORM_WINDOWS */
                }
            }

            // Restore scroll values
            sWFiles.hscroll()->set(xs);
            sWFiles.vscroll()->set(ys);

            return STATUS_OK;
        }

        status_t FileDialog::show_message(const char *title, const char *heading, const char *message, const io::Path *path)
        {
            if (pWMessage == NULL)
            {
                pWMessage       = new MessageBox(pDisplay);
                status_t res    = pWMessage->init();
                if (res != STATUS_OK)
                {
                    delete pWMessage;
                    return res;
                }

                LSP_STATUS_ASSERT(pWMessage->add("actions.ok"));
                pWMessage->buttons()->get(0)->constraints()->set_min_width(96);
            }
            LSP_STATUS_ASSERT(pWMessage->title()->set(title));
            LSP_STATUS_ASSERT(pWMessage->heading()->set(heading));
            LSP_STATUS_ASSERT(pWMessage->message()->set(message));

            if (path != NULL)
            {
                LSPString spath;
                LSP_STATUS_ASSERT(path->get_parent(&spath));
                LSP_STATUS_ASSERT(pWMessage->message()->params()->set_string("path", &spath));
                LSP_STATUS_ASSERT(path->get_last(&spath));
                LSP_STATUS_ASSERT(pWMessage->message()->params()->set_string("name", &spath));
                LSP_STATUS_ASSERT(pWMessage->message()->params()->set_string("file", path->as_string()));
            }

            pWMessage->show(this);
            return STATUS_OK;
        }
    }
}


