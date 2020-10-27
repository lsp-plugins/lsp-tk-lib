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
            sWWarning(dpy),

            sMode(&sProperties),
            sCustomAction(&sProperties),
            sActionText(&sProperties)
        {
            pWConfirm       = NULL;
            pWSearch        = NULL;
            pWMessage       = NULL;

            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;

            pClass          = &metadata;
        }

        FileDialog::~FileDialog()
        {
        }


        void FileDialog::destroy()
        {
            Window::destroy();

            drop_bookmarks();
//            destroy_file_entries(&vFiles);

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

        status_t FileDialog::init()
        {
            Label *l;

            // Initialize window
            LSP_STATUS_ASSERT(Window::init());

            // Add slots
            handler_id_t id = 0;
            if (id >= 0) id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_CANCEL, slot_on_cancel, self());
            if (id < 0)
                return -id;

            lsp_trace("Scaling factor: %f", sScaling.get());

            // Initialize labels
            LSP_STATUS_ASSERT(sWPath.init());
            sWPath.allocation()->set_hexpand(true);
            LSP_STATUS_ASSERT(sWSearch.init());
            LSP_STATUS_ASSERT(sWFilter.init());
            sWFilter.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(sWFiles.init());
            sWFiles.constraints()->set_min(400, 320);
            sWFiles.allocation()->set_hexpand(true);
            LSP_STATUS_ASSERT(sWAction.init());
            sWAction.constraints()->set_min_width(96);
            sWAction.allocation()->set_fill(false);
            sWAction.flat()->set(true);
            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(sWCancel.text()->set("actions.cancel"));
            sWCancel.constraints()->set_min_width(96);
            sWCancel.allocation()->set_fill(false);
            sWCancel.flat()->set(true);
            LSP_STATUS_ASSERT(sWWarning.init());
            sWWarning.visibility()->set(false);
            sWWarning.allocation()->set_hexpand(true);
            sWWarning.text_layout()->set(1.0f, 0.5f);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.text()->set("actions.nav.go"));
            wGo.allocation()->set_fill(false);
            wGo.constraints()->set_min_width(32);
            wGo.flat()->set(true);
            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.text()->set("actions.nav.up"));
            wUp.allocation()->set_fill(false);
            wUp.constraints()->set_min_width(32);
            wUp.flat()->set(true);
            LSP_STATUS_ASSERT(sBMAdd.init());
            LSP_STATUS_ASSERT(sBMAdd.text()->set("actions.to_bookmarks"));
            sBMAdd.allocation()->set_fill(false);
            sBMAdd.constraints()->set_min_width(32);
            sBMAdd.flat()->set(true);

            LSP_STATUS_ASSERT(wPathBox.init());
            wPathBox.orientation()->set_horizontal();
            wPathBox.spacing()->set(2);
            wPathBox.allocation()->set_fill(true);

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

            LSP_STATUS_ASSERT(init_bm_popup_menu());

            // Initialize supplementary elements
            // Path box
            sBMAdd.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(wPathBox.add(&sBMAdd));
            LSP_STATUS_ASSERT(wPathBox.add(&wUp));
            LSP_STATUS_ASSERT(wPathBox.add(&wGo));
            LSP_STATUS_ASSERT(add_label(&wPathBox, "labels.location", 1.0f, &l));
            l->allocation()->set(true, true);
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

            // Initialize structure
            wAutoExt.led()->set(true);
            wAutoExt.mode()->set_toggle();
            wAutoExt.down()->set(true);

            LSP_STATUS_ASSERT(add(&sMainGrid));

            // Bind events
            status_t result;

            result = sWAction.slots()->bind(SLOT_SUBMIT, slot_on_btn_action, self());
            if (result < 0)
                return -result;
            result = sWCancel.slots()->bind(SLOT_SUBMIT, slot_on_btn_cancel, self());
            if (result < 0)
                return -result;
            result = sWSearch.slots()->bind(SLOT_CHANGE, slot_on_search, self());
            if (result < 0)
                return -result;
            result = sWFilter.slots()->bind(SLOT_SUBMIT, slot_on_search, self());
            if (result < 0)
                return -result;
            result = sWFiles.slots()->bind(SLOT_MOUSE_DBL_CLICK, slot_mouse_dbl_click, self());
            if (result < 0)
                return -result;
            result = sWFiles.slots()->bind(SLOT_CHANGE, slot_list_change, self());
            if (result < 0)
                return -result;
            result = wGo.slots()->bind(SLOT_SUBMIT, slot_on_go, self());
            if (result < 0)
                return -result;
            result = wUp.slots()->bind(SLOT_SUBMIT, slot_on_up, self());
            if (result < 0)
                return -result;
            result = sBMAdd.slots()->bind(SLOT_SUBMIT, slot_on_bm_add, self());
            if (result < 0)
                return -result;
            result = sWPath.slots()->bind(SLOT_KEY_UP, slot_on_path_key_up, self());
            if (result < 0)
                return -result;
            result = sBookmarks.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll, self());
            if (result < 0)
                return -result;
            result = sSBBookmarks.slots()->bind(SLOT_REALIZED, slot_on_bm_realized, self());
            if (result < 0)
                return -result;

            padding()->set_all(8);
            border_style()->set(ws::BS_DIALOG);
            actions()->set_actions(ws::WA_DIALOG | ws::WA_RESIZE | ws::WA_CLOSE);
            layout()->set(0.0f, 1.0f);

            // Bind properties
            sMode.bind("mode", &sStyle);
            sCustomAction.bind("custom.action", &sStyle);
            sActionText.bind(&sStyle, pDisplay->dictionary());

            Style *sclass = style_class();
            if (sclass != NULL)
            {
                sMode.init(sclass, FDM_OPEN_FILE);
                sCustomAction.init(sclass, false);
            }

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
            return (dlg != NULL) ? dlg->on_btn_action(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_btn_cancel(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_btn_cancel(data) : STATUS_BAD_STATE;
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

        status_t FileDialog::slot_mouse_dbl_click(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_mouse_dbl_click(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_list_change(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_list_change(data) : STATUS_BAD_STATE;
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
//            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
//            if (dlg != NULL)
//                dlg->add_new_bookmark();
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
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            bm_entry_t *bm = _this->pPopupBookmark;

//            return ((_this != NULL) && (bm != NULL)) ? _this->set_path(&bm->sBookmark.path) : STATUS_OK;
            return STATUS_OK;
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

            _area->vstep()->set(lsp_max(sdelta, ydelta));

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_mouse_dbl_click(void *data)
        {
//            file_entry_t *ent = selected_entry();
//            if (ent == NULL)
//                return STATUS_OK;
//
//            // Analyze what to do
//            LSPString path;
//            if (ent->nFlags & F_DOTDOT)
//                return on_dlg_up(NULL);
//            else if (ent->nFlags & F_ISDIR)
//            {
//                LSP_STATUS_ASSERT(sWPath.get_text(&path));
//                LSP_STATUS_ASSERT(LSPFileMask::append_path(&path, &ent->sName));
//                return set_path(&path);
//            }
//            else
//                return on_dlg_action(data);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_list_change(void *data)
        {
//            if (enMode != FDM_SAVE_FILE)
//                return STATUS_OK;
//
//            file_entry_t *ent = selected_entry();
//            if (ent == NULL)
//                return STATUS_OK;
//
//            // Analyze what to do
//            if ((ent->nFlags & F_DOTDOT) || (ent->nFlags & F_ISDIR))
//                return STATUS_OK;
//
//            return sWSearch.set_text(&ent->sName);
            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_search(void *data)
        {
//            if (invisible())
//                return STATUS_OK;
//
//            return apply_filters();
            return STATUS_OK;
        }

        status_t FileDialog::on_btn_action(void *data)
        {
//            bool committed = false;
//
//            if (enMode == FDM_SAVE_FILE) // Use 'File name' field
//            {
//                LSPString fname;
//                LSP_STATUS_ASSERT(sWSearch.get_text(&fname));
//
//                if (wAutoExt.is_down())
//                {
//                    LSPString ext;
//                    ssize_t sel = sWFilter.selected();
//
//                    LSPFileFilterItem *item  = sFilter.get((sel < 0) ? 0 : sel);
//                    status_t res = (item != NULL) ? item->get_extension(&ext) : STATUS_NOT_FOUND;
//
//                    if (res == STATUS_OK)
//                    {
//                        lsp_trace("fname = %s, ext = %s", fname.get_native(), ext.get_native());
//                        if (!fname.ends_with_nocase(&ext))
//                            fname.append(&ext);
//                        lsp_trace("fname = %s", fname.get_native());
//                    }
//                }
//
//                if (LSPFileMask::is_dots(&fname) || (!LSPFileMask::valid_file_name(&fname)))
//                    return show_message("titles.attention", "headings.attention", "messages.file.invalid_name");
//
//                LSP_STATUS_ASSERT(build_full_path(&sSelected, &fname));
//                committed = true;
//            }
//            else
//            {
//                LSPString fname;
//                LSP_STATUS_ASSERT(sWSearch.get_text(&fname));
//                if ((!LSPFileMask::is_dots(&fname)) && (LSPFileMask::valid_file_name(&fname)))
//                {
//                    LSP_STATUS_ASSERT(build_full_path(&sSelected, &fname));
//                    committed = true;
//                }
//            }
//
//            // Use selection
//            if (!committed)
//            {
//                file_entry_t *ent = selected_entry();
//                if (ent == NULL)
//                    return show_message("titles.attention", "headings.attention", "messages.file.not_specified");
//
//                // Analyze what to do
//                if (ent->nFlags & F_DOTDOT)
//                    return on_dlg_up(NULL);
//                else if (ent->nFlags & F_ISDIR)
//                {
//                    LSPString path;
//                    LSP_STATUS_ASSERT(sWPath.get_text(&path));
//                    LSP_STATUS_ASSERT(LSPFileMask::append_path(&path, &ent->sName));
//                    return set_path(&path);
//                }
//                else
//                {
//                    LSPString path;
//                    LSP_STATUS_ASSERT(sWPath.get_text(&path));
//                    LSP_STATUS_ASSERT(LSPFileMask::append_path(&sSelected, &path, &ent->sName));
//                }
//            }
//
//            // Special case for saving file
//            io::fattr_t fattr;
//            status_t stat_result = io::File::sym_stat(&sSelected, &fattr);
//
//            if (enMode == FDM_SAVE_FILE)
//            {
//                if (!bUseConfirm)
//                    return on_dlg_confirm(data);
//
//                // Check that file exists and avoid confirmation if it doesn't
//                lsp_trace("Checking file: %s", sSelected.get_native());
//                if (stat_result != STATUS_OK)
//                    return on_dlg_confirm(data);
//            }
//            else
//            {
//                if (stat_result != 0)
//                    return show_message("titles.attention", "headings.attention", "messages.file.not_exists");
//
//                if (!bUseConfirm)
//                    return on_dlg_confirm(data);
//            }
//
//            if (pWConfirm == NULL)
//            {
//                // Create dialog object
//                pWConfirm = new LSPMessageBox(pDisplay);
//                if (pWConfirm == NULL)
//                    return STATUS_NO_MEM;
//                pWConfirm->init();
//
//                pWConfirm->title()->set("titles.confirmation");
//                pWConfirm->heading()->set("headings.confirmation");
//                pWConfirm->add_button("actions.confirm.yes", slot_on_confirm, self());
//                pWConfirm->add_button("actions.confirm.no");
//            }
//            pWConfirm->message()->set(&sConfirm);
//            pWConfirm->show(this);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_confirm(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            hide();
//            destroy_file_entries(&vFiles);
            drop_bookmarks();

            // Execute slots
            return sSlots.execute(SLOT_SUBMIT, this, data);
        }

        status_t FileDialog::on_btn_cancel(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            drop_bookmarks();
            hide();
//            destroy_file_entries(&vFiles);

            // Execute slots
            return sSlots.execute(SLOT_CANCEL, this, data);
        }

        status_t FileDialog::on_show()
        {
//            ssize_t idx = sFilter.get_default();
//            if ((idx < 0) && (sFilter.size() > 0))
//                idx = 0;
//            sWFilter.set_selected(idx);
            refresh_bookmarks();
//            refresh_current_path();
            return STATUS_OK;
        }

        status_t FileDialog::on_close(const ws::event_t *e)
        {
            ws::event_t ev = *e;
            return on_btn_cancel(&ev);
        }

        status_t FileDialog::on_bm_submit(Widget *sender)
        {
//            bm_entry_t *ent = find_bookmark(sender);
//            return (ent != NULL) ? set_path(&ent->sBookmark.path) : STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_go(void *data)
        {
//            LSPString path;
//            LSP_STATUS_ASSERT(sWPath.get_text(&path));
//            return set_path(&path);
            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_up(void *data)
        {
//            LSPString path;
//            LSP_STATUS_ASSERT(sWPath.get_text(&path));
//            ssize_t pos = path.rindex_of(FILE_SEPARATOR_C);
//            if (pos < 0)
//                return STATUS_OK;
//            path.truncate(pos);
//            if (path.length() <= 0)
//                path.append(FILE_SEPARATOR_C);
//            return set_path(&path);
            return STATUS_OK;
        }

        status_t FileDialog::on_path_key_up(ws::event_t *e)
        {
            lsp_trace("Path key code released=%x, modifiers=%x", int(e->nCode), int(e->nState));
            ws::code_t key = KeyboardHandler::translate_keypad(e->nCode);
            if (key == ws::WSK_RETURN)
                return on_dlg_go(e);
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

                ent->sHlink.text_layout()->set_halign(-1.0f);
                ent->sHlink.follow()->set(false);
                ent->sHlink.url()->set_raw(&url);
                ent->sHlink.padding()->set_horizontal(8, 8);
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
//            status_t res;
//            LSPString spath;
//            io::Path path;
//
//            // Get path and canonicalize
//            if ((res = sWPath.get_text(&spath)) != STATUS_OK)
//                return res;
//            if ((res = path.set(&spath)) != STATUS_OK)
//                return res;
//            if ((res = path.canonicalize()) != STATUS_OK)
//                return res;
//
//            bm_entry_t *found = NULL;
//            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
//            {
//                bm_entry_t *ent = vBookmarks.at(i);
//                if ((ent != NULL) && (ent->sPath.equals(&path)))
//                {
//                    found = ent;
//                    break;
//                }
//            }
//
//            // Check state
//            if (found == pSelBookmark)
//                return STATUS_OK;
//
//            // Deactivate selected bookmark
//            if (pSelBookmark != NULL)
//                pSelBookmark->sHlink.bg_color()->set_default();
//            pSelBookmark = found;
//            if (found != NULL)
//                init_color(C_BACKGROUND2, found->sHlink.bg_color());

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

        status_t FileDialog::on_submit()
        {
            return STATUS_OK;
        }

        status_t FileDialog::on_cancel()
        {
            return STATUS_OK;
        }
    }
}


