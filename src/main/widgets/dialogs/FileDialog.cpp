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
            pClass          = &metadata;
        }

        FileDialog::~FileDialog()
        {
        }

        status_t FileDialog::init()
        {
            Label *l;

            // Initialize labels
            LSP_STATUS_ASSERT(Window::init());
            LSP_STATUS_ASSERT(sWPath.init());
            sWPath.allocation()->set_hexpand(true);
            LSP_STATUS_ASSERT(sWSearch.init());
            LSP_STATUS_ASSERT(sWFilter.init());
            sWFilter.allocation()->set_fill(true);
            LSP_STATUS_ASSERT(sWFiles.init());
            sWFiles.constraints()->set_min(400, 320);
            sWFiles.allocation()->set_hexpand(true);
            LSP_STATUS_ASSERT(sWAction.init());
            sWAction.constraints()->set_min(96, 24);
            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(sWCancel.text()->set("actions.cancel"));
            sWCancel.constraints()->set_min(96, 24);
            LSP_STATUS_ASSERT(sWWarning.init());
            sWWarning.visibility()->set(false);
            sWWarning.allocation()->set_hexpand(true);
            sWWarning.text_layout()->set(1.0f, 0.5f);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(wGo.text()->set("actions.nav.go"));
            wGo.allocation()->set_fill(false);
            wGo.constraints()->set_min_width(32);
            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(wUp.text()->set("actions.nav.up"));
            wUp.allocation()->set_fill(false);
            wUp.constraints()->set_min_width(32);
            LSP_STATUS_ASSERT(sBMAdd.init());
            LSP_STATUS_ASSERT(sBMAdd.text()->set("actions.to_bookmarks"));
            sBMAdd.allocation()->set_fill(false);
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
            sMainGrid.bg_color()->set_rgb24(0xff0000);
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

// TODO
//            LSP_STATUS_ASSERT(init_bm_popup_menu());

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
            LSP_STATUS_ASSERT(sMainGrid.add(&sHBox));

            // Initialize structure
            wAutoExt.led()->set(true);
            wAutoExt.mode()->set_toggle();
            wAutoExt.down()->set(true);

            LSP_STATUS_ASSERT(add(&sMainGrid));

            // Bind events
            status_t result;

            result = sWAction.slots()->bind(SLOT_SUBMIT, slot_on_action, self());
            if (result < 0)
                return -result;
            result = sWCancel.slots()->bind(SLOT_SUBMIT, slot_on_cancel, self());
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

        void FileDialog::destroy()
        {
            Window::destroy();

//            drop_bookmarks();
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
            vWidgets.clear();

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

        status_t FileDialog::add_menu_item(Menu *m, const char *text, event_handler_t handler)
        {
            // TODO
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

        status_t FileDialog::slot_on_action(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_action(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_confirm(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_confirm(data) : STATUS_BAD_STATE;
        }

        status_t FileDialog::slot_on_cancel(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_dlg_cancel(data) : STATUS_BAD_STATE;
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

        status_t FileDialog::slot_on_bm_popup(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            _this->pPopupBookmark = _this->find_bookmark(sender);
            return STATUS_OK;;
        }

        status_t FileDialog::slot_on_bm_menu_open(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
//                _this->set_path(&_this->pPopupBookmark->sBookmark.path) :
//                STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_follow(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
//                _this->pPopupBookmark->sHlink.follow_url() :
//                STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_copy(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
//                _this->pPopupBookmark->sHlink.copy_url(CBUF_CLIPBOARD) :
//                STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_delete(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            return ((_this != NULL) && (_this->pPopupBookmark != NULL)) ?
//                _this->remove_bookmark(_this->pPopupBookmark) :
//                STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_up(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            ssize_t idx = (_this->pPopupBookmark != NULL) ?
//                    _this->vBookmarks.index_of(_this->pPopupBookmark) : -1;
//
//            // Find previous visible bookmark
//            ssize_t prev = idx - 1;
//            for ( ; prev >= 0; --prev)
//            {
//                bm_entry_t *ent = _this->vBookmarks.at(prev);
//                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
//                    break;
//            }
//
//            if (prev < 0)
//                return STATUS_OK;
//
//            return (_this->vBookmarks.swap(prev, idx)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_down(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            ssize_t items = _this->vBookmarks.size();
//            ssize_t idx = (_this->pPopupBookmark != NULL) ?
//                    _this->vBookmarks.index_of(_this->pPopupBookmark) : items;
//
//            // Find next visible bookmark
//            ssize_t next = idx + 1;
//            for ( ; next < items; ++next)
//            {
//                bm_entry_t *ent = _this->vBookmarks.at(next);
//                if ((ent != NULL) && (ent->sBookmark.origin & bookmarks::BM_LSP))
//                    break;
//            }
//
//            if (next >= items)
//                return STATUS_OK;
//
//            return (_this->vBookmarks.swap(idx, next)) ?  _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_first(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            ssize_t idx = (_this->pPopupBookmark != NULL) ?
//                    _this->vBookmarks.index_of(_this->pPopupBookmark) : -1;
//            if (idx <= 0)
//                return STATUS_OK;
//            return (_this->vBookmarks.move(idx, 0)) ? _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_last(Widget *sender, void *ptr, void *data)
        {
//            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
//            ssize_t last = _this->vBookmarks.size() - 1;
//            ssize_t idx = (_this->pPopupBookmark != NULL) ?
//                    _this->vBookmarks.index_of(_this->pPopupBookmark) : last;
//            if (idx >= last)
//                return STATUS_OK;
//            return (_this->vBookmarks.move(idx, last)) ? _this->sync_bookmarks() : STATUS_UNKNOWN_ERR;
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

        status_t FileDialog::on_dlg_action(void *data)
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
//            // Hide dialogs
//            if (pWConfirm != NULL)
//                pWConfirm->hide();
//            hide();
//            destroy_file_entries(&vFiles);
//
//            // Execute slots
//            return sAction.execute(this, data);
            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_cancel(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
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
//            refresh_bookmarks();
//            refresh_current_path();
//            return STATUS_OK;
            return STATUS_OK;
        }

        status_t FileDialog::on_close(const ws::event_t *e)
        {
            ws::event_t ev = *e;
            return on_dlg_cancel(&ev);
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
    }
}


