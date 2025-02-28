/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
            LSP_TK_BUILTIN_STYLE(FileDialog, "FileDialog", "Window");

            //-----------------------------------------------------------------
            // FileDialog::NavButton style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__NavButton, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__NavButton, Button)
                // Override
                sConstraints.set_min_width(32);
                sAllocation.set_fill(false);
                // Commit
                sConstraints.override();
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__NavButton, "FileDialog::NavButton", "Button");

            //-----------------------------------------------------------------
            // FileDialog::NavBox style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__NavBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__NavBox, Box)
                // Override
                sSpacing.set(2);
                sAllocation.set(true, false);
                // Commit
                sSpacing.override();
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__NavBox, "FileDialog::NavBox", "Box");

            //-----------------------------------------------------------------
            // FileDialog::ActionButton style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ActionButton, Button)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ActionButton, Button)
                // Override
                sConstraints.set_min_width(96);
                sAllocation.set_fill(false);
                // Commit
                sConstraints.override();
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ActionButton, "FileDialog::ActionButton", "Button");

            //-----------------------------------------------------------------
            // FileDialog::ActionBox style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ActionBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ActionBox, Box)
                // Override
                sSpacing.set(8);
                // Commit
                sSpacing.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ActionBox, "FileDialog::ActionBox", "Box");

            //-----------------------------------------------------------------
            // FileDialog::ActionAlign style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ActionAlign, Align)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ActionAlign, Align)
                // Override
                sLayout.set(0.0f, 0.0f, 1.0f, 0.0f);
                // Commit
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ActionAlign, "FileDialog::ActionAlign", "Align");

            //-----------------------------------------------------------------
            // FileDialog::PreviewAlign style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__PreviewAlign, Align)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__PreviewAlign, Align)
                // Override
                sLayout.set(0.0f, 0.0f, 1.0f, 1.0f);
                // Commit
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__PreviewAlign, "FileDialog::PreviewAlign", "Align");

            //-----------------------------------------------------------------
            // FileDialog::Bookmark style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Bookmark, Hyperlink)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Bookmark, Hyperlink)
                // Override
                sPadding.set(2, 4);
                sTextLayout.set_halign(-1.0f);
                sFollow.set(false);
                // Commit
                sPadding.override();
                sTextLayout.override();
                sFollow.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Bookmark, "FileDialog::Bookmark", "Hyperlink");

            //-----------------------------------------------------------------
            // FileDialog::Bookmark.selected style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Bookmark_selected, FileDialog__Bookmark)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Bookmark_selected, FileDialog__Bookmark)
                // Override
                sColor.set("#ffffff");
                sBgColor.set("#888888");
                // Commit
                sColor.override();
                sBgColor.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Bookmark_selected, "FileDialog::Bookmark.selected", "FileDialog::Bookmark");

            //-----------------------------------------------------------------
            // FileDialog::Warning
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Warning, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Warning, Label)
                // Override
                sAllocation.set_fill(true, false);
                sAllocation.set_vreduce(true);
                sTextLayout.set(1.0f, 0.5f);
                sColor.set("#ff0000");
                sAllocation.set_hexpand(true);
                // Commit
                sAllocation.override();
                sTextLayout.override();
                sColor.override();
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Warning, "FileDialog::Warning", "Label");

            //-----------------------------------------------------------------
            // FileDialog::Label
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Label, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Label, Label)
                // Override
                sAllocation.set_hfill(true);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Label, "FileDialog::Label", "Label");

            //-----------------------------------------------------------------
            // FileDialog::FileListLabel
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__FileListLabel, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__FileListLabel, Label)
                // Override
                sAllocation.set_hfill(true);
                sAllocation.set_vreduce(true);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__FileListLabel, "FileDialog::FileListLabel", "Label");

            //-----------------------------------------------------------------
            // FileDialog::ExtCheck
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__ExtCheck, CheckBox)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__ExtCheck, CheckBox)
                // Override
                vColors[CHECKBOX_NORMAL].sColor.set("#ffff00");
                sChecked.set(true);
                // Commit
                vColors[CHECKBOX_NORMAL].sColor.override();
                sChecked.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__ExtCheck, "FileDialog::ExtCheck", "CheckBox");

            //-----------------------------------------------------------------
            // FileDialog::FilterComboBox
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__FilterComboBox, ComboBox)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__FilterComboBox, ComboBox)
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__FilterComboBox, "FileDialog::FilterComboBox", "ComboBox");

            //-----------------------------------------------------------------
            // FileDialog::MainGrid
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__MainGrid, Grid)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__MainGrid, Grid)
                // Override
                sHSpacing.set(4);
                sVSpacing.set(4);
                // Commit
                sHSpacing.override();
                sVSpacing.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__MainGrid, "FileDialog::MainGrid", "Grid");

            //-----------------------------------------------------------------
            // FileDialog::MainGrid
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__FileList, ListBox)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__FileList, ListBox)
                // Override
                sSizeConstraints.set_min(400, 320);
                sAllocation.set_hexpand(true);
                // Commit
                sSizeConstraints.override();
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__FileList, "FileDialog::FileList", "ListBox");

            //-----------------------------------------------------------------
            // FileDialog::Path
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__Path, Edit)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__Path, Edit)
                // Override
                sAllocation.set_hexpand(true);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__Path, "FileDialog::Path", "Edit");

            //-----------------------------------------------------------------
            // FileDialog::WarningBox style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__WarningBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__WarningBox, Box)
                // Override
                sSpacing.set(8);
                sOrientation.set_vertical();
                // Commit
                sSpacing.override();
                sOrientation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__WarningBox, "FileDialog::WarningBox", "Box");

            //-----------------------------------------------------------------
            // FileDialog::BookmarkArea style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__BookmarkArea, ScrollArea)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__BookmarkArea, ScrollArea)
                // Override
                sHScrollMode.set(SCROLL_OPTIONAL);
                sVScrollMode.set(SCROLL_OPTIONAL);
                sAllocation.set_expand(true);
                sLayout.set(-1.0f, -1.0f, 1.0f, 0.0f);
                // Commit
                sHScrollMode.override();
                sVScrollMode.override();
                sAllocation.override();
                sLayout.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__BookmarkArea, "FileDialog::BookmarkArea", "ScrollArea");

            //-----------------------------------------------------------------
            // FileDialog::BookmarkBox style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__BookmarkBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__BookmarkBox, Box)
                // Override
                sAllocation.set_expand(true);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__BookmarkBox, "FileDialog::BookmarkBox", "Box");

            //-----------------------------------------------------------------
            // FileDialog::OptionBox style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__OptionBox, Box)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__OptionBox, Box)
                // Override
                sConstraints.set_min_width(192);
                sAllocation.set_expand(true);
                sSpacing.set(4);
                // Commit
                sConstraints.override();
                sAllocation.override();
                sSpacing.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__OptionBox, "FileDialog::OptionBox", "Box");

            //-----------------------------------------------------------------
            // FileDialog::OptionSeparator style
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__OptionSeparator, Separator)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__OptionSeparator, Separator)
                // Override
                sAllocation.set_hexpand(true);
                // Commit
                sAllocation.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__OptionSeparator, "FileDialog::OptionSeparator", "Separator");

            //-----------------------------------------------------------------
            // FileDialog::NavLabel
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__NavLabel, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__NavLabel, Label)
                // Override
                sAllocation.set(true, true);
                sPadding.set_left(8);
                // Commit
                sAllocation.override();
                sPadding.override();
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__NavLabel, "FileDialog::NavLabel", "Label");

            //-----------------------------------------------------------------
            // FileDialog::BookmarkLabel
            LSP_TK_STYLE_DEF_BEGIN(FileDialog__BookmarkLabel, Label)
            LSP_TK_STYLE_DEF_END

            LSP_TK_STYLE_IMPL_BEGIN(FileDialog__BookmarkLabel, Label)
            LSP_TK_STYLE_IMPL_END
            LSP_TK_BUILTIN_STYLE(FileDialog__BookmarkLabel, "FileDialog::BookmarkLabel", "Label");
        }

        const w_class_t FileDialog::metadata            = { "FileDialog", &Window::metadata };

        template <class T>
        static inline void destroy_widget(T * &w)
        {
            if (w == NULL)
                return;
            w->destroy();
            delete w;
            w = NULL;
        }

        FileDialog::FileDialog(Display *dpy):
            Window(dpy),

            sWPath(dpy),
            sWSearch(dpy),
            sWFilter(dpy),
            sWFiles(dpy),
            sWAction(dpy),
            sWCancel(dpy),
            sMainGrid(dpy),
            wSAAccess(dpy),
            wSABox(dpy),
            wOptions(dpy),
            wSeparator(dpy),
            wVolumes(dpy),
            wBookmarks(dpy),
            wVolPopup(dpy),
            wBMPopup(dpy),
            wBMAdd(dpy),
            sActionBox(dpy),
            sActionAlign(dpy),
            sAppendExt(dpy),
            wAutoExt(dpy),
            wGo(dpy),
            wUp(dpy),
            wNavBox(dpy),
            wPreviewHeading(dpy),
            wPreview(dpy),
            wWWarning(dpy),

            sMode(&sProperties),
            sCustomAction(&sProperties),
            sActionText(&sProperties),
            sPath(&sProperties),
            sFilter(&sProperties),
            sSelFilter(&sProperties),
            sSelected(&sProperties),
            sUseConfirm(&sProperties),
            sConfirmMsg(&sProperties),
            sOptions(&sProperties),
            sPreview(&sProperties)
        {
            pWConfirm       = NULL;
            pWSearch        = NULL;
            pWMessage       = NULL;

            pSelBookmark    = NULL;
            pPopupBookmark  = NULL;

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

            drop_volumes();
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
            sActionBox.destroy();
            sActionAlign.destroy();
            wSAAccess.destroy();
            wSABox.destroy();
            wOptions.destroy();
            wSeparator.destroy();
            wVolumes.destroy();
            wBookmarks.destroy();
            wVolPopup.destroy();
            wBMPopup.destroy();
            wBMAdd.destroy();
            sMainGrid.destroy();
            wWWarning.destroy();
            sAppendExt.destroy();
            wAutoExt.destroy();
            wGo.destroy();
            wUp.destroy();
            wNavBox.destroy();
            wPreviewHeading.destroy();
            wPreview.destroy();

            pWSearch = NULL;

            destroy_widget(pWConfirm);
            destroy_widget(pWMessage);
        }

        void FileDialog::drop_bookmark_list(lltl::parray<bm_entry_t> *list)
        {
            for (size_t i=0, n=list->size(); i<n; ++i)
            {
                bm_entry_t *ent = list->uget(i);
                if (pSelBookmark == ent)
                    pSelBookmark    = NULL;
                if (pPopupBookmark == ent)
                    pPopupBookmark  = NULL;

                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
            }
            list->flush();
        }

        void FileDialog::drop_bookmarks()
        {
            // Deactivate currently selected bookmark
            wBookmarks.remove_all();
            drop_bookmark_list(&vBookmarks);
        }

        void FileDialog::drop_volumes()
        {
            // Deactivate currently selected bookmark
            wVolumes.remove_all();
            drop_bookmark_list(&vVolumes);
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
            LSP_STATUS_ASSERT(inject_style(&sWPath, "FileDialog::Path"));

            LSP_STATUS_ASSERT(sWSearch.init());
            LSP_STATUS_ASSERT(inject_style(&sWSearch, "FileDialog::Search"));

            LSP_STATUS_ASSERT(sWFilter.init());
            LSP_STATUS_ASSERT(inject_style(&sWFilter, "FileDialog::FilterComboBox"));

            LSP_STATUS_ASSERT(sWFiles.init());
            LSP_STATUS_ASSERT(inject_style(&sWFiles, "FileDialog::FileList"));

            LSP_STATUS_ASSERT(sWAction.init());
            LSP_STATUS_ASSERT(inject_style(&sWAction, "FileDialog::ActionButton"));

            LSP_STATUS_ASSERT(sWCancel.init());
            LSP_STATUS_ASSERT(inject_style(&sWCancel, "FileDialog::ActionButton"));
            LSP_STATUS_ASSERT(sWCancel.text()->set("actions.cancel"));

            LSP_STATUS_ASSERT(wPreviewHeading.init());
            LSP_STATUS_ASSERT(inject_style(&wPreviewHeading, "FileDialog::Label"));
            wPreviewHeading.text()->set("labels.file_preview");
            wPreviewHeading.text_layout()->set_halign(-1.0f);
            wPreviewHeading.visibility()->set(false);

            LSP_STATUS_ASSERT(wPreview.init());
            LSP_STATUS_ASSERT(inject_style(&wPreview, "FileDialog::PreviewAlign"));
            wPreview.visibility()->set(false);

            LSP_STATUS_ASSERT(wWWarning.init());
            LSP_STATUS_ASSERT(wWWarning.style()->inject_parent(pWarning));
            wWWarning.visibility()->set(false);

            LSP_STATUS_ASSERT(wGo.init());
            LSP_STATUS_ASSERT(inject_style(&wGo, "FileDialog::NavButton"));
            LSP_STATUS_ASSERT(wGo.text()->set("actions.nav.go"));

            LSP_STATUS_ASSERT(wUp.init());
            LSP_STATUS_ASSERT(inject_style(&wUp, "FileDialog::NavButton"));
            LSP_STATUS_ASSERT(wUp.text()->set("actions.nav.up"));

            LSP_STATUS_ASSERT(wBMAdd.init());
            LSP_STATUS_ASSERT(inject_style(&wBMAdd, "FileDialog::NavButton"));
            LSP_STATUS_ASSERT(wBMAdd.text()->set("actions.to_bookmarks"));

            LSP_STATUS_ASSERT(wNavBox.init());
            LSP_STATUS_ASSERT(inject_style(&wNavBox, "FileDialog::NavBox"));
            wNavBox.orientation()->set_horizontal();

            LSP_STATUS_ASSERT(sMainGrid.init());
            LSP_STATUS_ASSERT(inject_style(&sMainGrid, "FileDialog::MainGrid"));
            sMainGrid.rows()->set(8);
            sMainGrid.columns()->set(3);
            sMainGrid.orientation()->set_horizontal();

            LSP_STATUS_ASSERT(sActionBox.init());
            LSP_STATUS_ASSERT(inject_style(&sActionBox, "FileDialog::ActionBox"));
            sActionBox.orientation()->set_horizontal();

            LSP_STATUS_ASSERT(sActionAlign.init());
            LSP_STATUS_ASSERT(inject_style(&sActionAlign, "FileDialog::ActionAlign"));

            LSP_STATUS_ASSERT(wSAAccess.init());
            LSP_STATUS_ASSERT(inject_style(&wSAAccess, "FileDialog::BookmarkArea"));

            LSP_STATUS_ASSERT(wSABox.init());
            wSABox.orientation()->set_vertical();
            LSP_STATUS_ASSERT(wSAAccess.add(&wSABox));

            LSP_STATUS_ASSERT(add_label(&wSABox, "labels.volume_list", -1.0f, &l));
            l->slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll);
            LSP_STATUS_ASSERT(inject_style(l, "FileDialog::BookmarkLabel"));

            LSP_STATUS_ASSERT(wVolumes.init());
            LSP_STATUS_ASSERT(inject_style(&wVolumes, "FileDialog::BookmarkBox"));
            wVolumes.orientation()->set_vertical();
            LSP_STATUS_ASSERT(wSABox.add(&wVolumes));

            LSP_STATUS_ASSERT(add_label(&wSABox, "labels.bookmark_list", -1.0f, &l));
            l->slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll);
            LSP_STATUS_ASSERT(inject_style(l, "FileDialog::BookmarkLabel"));

            LSP_STATUS_ASSERT(wBookmarks.init());
            LSP_STATUS_ASSERT(inject_style(&wBookmarks, "FileDialog::BookmarkBox"));
            wBookmarks.orientation()->set_vertical();
            LSP_STATUS_ASSERT(wSABox.add(&wBookmarks));

            LSP_STATUS_ASSERT(wOptions.init());
            LSP_STATUS_ASSERT(inject_style(&wOptions, "FileDialog::OptionBox"));
            wOptions.orientation()->set_vertical();
            LSP_STATUS_ASSERT(wOptions.add(&wSAAccess));

            LSP_STATUS_ASSERT(wSeparator.init());
            LSP_STATUS_ASSERT(inject_style(&wSeparator, "FileDialog::OptionSeparator"));
            wSeparator.orientation()->set_horizontal();

            LSP_STATUS_ASSERT(init_bm_popup_menu(&wVolPopup, false));
            LSP_STATUS_ASSERT(init_bm_popup_menu(&wBMPopup, true));

            // Initialize supplementary elements
            // Path box
            LSP_STATUS_ASSERT(wNavBox.add(&wBMAdd));
            LSP_STATUS_ASSERT(wNavBox.add(&wUp));
            LSP_STATUS_ASSERT(wNavBox.add(&wGo));
            LSP_STATUS_ASSERT(add_label(&wNavBox, "labels.location", 1.0f, &l));
            LSP_STATUS_ASSERT(inject_style(l, "FileDialog::NavLabel"));
            // Button box
            LSP_STATUS_ASSERT(sActionBox.add(&sWAction));
            LSP_STATUS_ASSERT(sActionBox.add(&sWCancel));
            LSP_STATUS_ASSERT(sActionAlign.add(&sActionBox));

            /* File dialog main layout:
             *
             * +--------------------+--------------------+--------------------+
             * |       wNavBox      |                  sWPath                 |
             * +--------------------+--------------------+--------------------+
             * |                    |                  wWWarning              |
             * +-                  -+--------------------+--------------------+
             * |                    |      file_list     |  wPreviewHeading   |
             * +-                  -+--------------------+--------------------+
             * |                    |                    |                    |
             * |                    |                    |                    |
             * |      wOptions      |                    |                    |
             * |                    |                    |                    |
             * |                    |      sWFiles       |                    |
             * |                    |                    |                    |
             * |                    |                    |                    |
             * |                    |                    |     wPreview       |
             * |                    |                    |                    |
             * |                    |                    |                    |
             * +--------------------+--------------------+--------------------+
             * |         NULL       |           automatic_extension           |
             * +--------------------+--------------------+--------------------+
             * |     file_name      |                sWSearch                 |
             * +--------------------+--------------------+--------------------+
             * |       filter       |                sWFilter                 |
             * +--------------------+--------------------+--------------------+
             * |         NULL       |               sActionAlign              |
             * +--------------------+--------------------+--------------------+
             */

            // Initialize grid
            // Row 1
            LSP_STATUS_ASSERT(sMainGrid.add(&wNavBox));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWPath, 1, 2));
            // Row 2
            LSP_STATUS_ASSERT(sMainGrid.add(&wOptions, 3, 1)); // 3 rows
            LSP_STATUS_ASSERT(sMainGrid.add(&wWWarning, 1, 2)); // 2 columns
            // Row 3
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.file_list", -1.0f, &l));
            LSP_STATUS_ASSERT(inject_style(l, "FileDialog::FileListLabel"));
            LSP_STATUS_ASSERT(sMainGrid.add(&wPreviewHeading));
            // Row 4
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFiles));
            LSP_STATUS_ASSERT(sMainGrid.add(&wPreview, 1, 1));
            // Row 4
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(add_ext_check(&sMainGrid, "labels.automatic_extension", 1, 2));
            // Row 5
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.file_name", 1.0f, &pWSearch));
            LSP_STATUS_ASSERT(inject_style(pWSearch, "FileDialog::Label"));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWSearch, 1, 2));
            // Row 6
            LSP_STATUS_ASSERT(add_label(&sMainGrid, "labels.filter", 1.0f, &l));
            LSP_STATUS_ASSERT(inject_style(l, "FileDialog::Label"));
            LSP_STATUS_ASSERT(sMainGrid.add(&sWFilter, 1, 2));
            // Row 7
            LSP_STATUS_ASSERT(sMainGrid.add(NULL));
            LSP_STATUS_ASSERT(sMainGrid.add(&sActionAlign, 1, 2)); // Action button box

            // Add the whole structure
            LSP_STATUS_ASSERT(this->add(&sMainGrid));

            // Add slots
            handler_id_t id = 0;

            // Self-defined events
            id = sSlots.add(SLOT_SUBMIT, slot_on_submit, self());
            if (id >= 0) id = sSlots.add(SLOT_CANCEL, slot_on_cancel, self());
            if (id >= 0) id = sSlots.add(SLOT_CHANGE, slot_on_change, self());

            // Event handlers
            if (id >= 0) id = sWAction.slots()->bind(SLOT_SUBMIT, slot_on_btn_action, self());
            if (id >= 0) id = sWCancel.slots()->bind(SLOT_SUBMIT, slot_on_btn_cancel, self());
            if (id >= 0) id = sWSearch.slots()->bind(SLOT_CHANGE, slot_on_search, self());
            if (id >= 0) id = sWSearch.slots()->bind(SLOT_KEY_DOWN, slot_on_search_key_down, self());
            if (id >= 0) id = sWFilter.slots()->bind(SLOT_SUBMIT, slot_on_filter_submit, self());
            if (id >= 0) id = sWFilter.slots()->bind(SLOT_KEY_DOWN, slot_on_filter_key_down, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_MOUSE_DBL_CLICK, slot_on_list_dbl_click, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_CHANGE, slot_on_list_change, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_REALIZED, slot_on_list_realized, self());
            if (id >= 0) id = sWFiles.slots()->bind(SLOT_KEY_DOWN, slot_on_list_key_down, self());
            if (id >= 0) id = wGo.slots()->bind(SLOT_SUBMIT, slot_on_go, self());
            if (id >= 0) id = wUp.slots()->bind(SLOT_SUBMIT, slot_on_up, self());
            if (id >= 0) id = wBMAdd.slots()->bind(SLOT_SUBMIT, slot_on_bm_add, self());
            if (id >= 0) id = sWPath.slots()->bind(SLOT_KEY_UP, slot_on_path_key_up, self());
            if (id >= 0) id = wBookmarks.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll, self());
            if (id >= 0) id = wSAAccess.slots()->bind(SLOT_REALIZED, slot_on_bm_realized, self());

            if (id < 0)
                return -id;

            Shortcut *scut = NULL;
            if ((scut = shortcuts()->append(ws::WSK_ESCAPE, KM_NONE)) != NULL)
                scut->slot()->bind(slot_on_dialog_escape, self());
            if ((scut = shortcuts()->append(ws::WSK_RETURN, KM_NONE)) != NULL)
                scut->slot()->bind(slot_on_dialog_return, self());
            if ((scut = shortcuts()->append(ws::WSK_KEYPAD_ENTER, KM_NONE)) != NULL)
                scut->slot()->bind(slot_on_dialog_return, self());

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
            sOptions.bind(NULL);
            sPreview.bind(NULL);

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

        status_t FileDialog::inject_style(tk::Widget *w, const char *name)
        {
            Style *s = pDisplay->schema()->get(name);
            if (s == NULL)
                return STATUS_BAD_STATE;
            return w->style()->inject_parent(s);
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

        status_t FileDialog::init_bm_popup_menu(tk::Menu *menu, bool editable)
        {
            LSP_STATUS_ASSERT(menu->init());
            LSP_STATUS_ASSERT(add_menu_item(menu, "actions.open", slot_on_bm_menu_open));
            LSP_STATUS_ASSERT(add_menu_item(menu, "actions.link.follow", slot_on_bm_menu_follow));
            LSP_STATUS_ASSERT(add_menu_item(menu, "actions.link.copy", slot_on_bm_menu_copy));

            if (editable)
            {
                LSP_STATUS_ASSERT(add_menu_item(menu, "actions.edit.delete", slot_on_bm_menu_delete));
                LSP_STATUS_ASSERT(add_menu_item(menu, NULL, NULL));
                LSP_STATUS_ASSERT(add_menu_item(menu, "actions.edit.move_first", slot_on_bm_menu_first));
                LSP_STATUS_ASSERT(add_menu_item(menu, "actions.edit.move_up", slot_on_bm_menu_up));
                LSP_STATUS_ASSERT(add_menu_item(menu, "actions.edit.move_down", slot_on_bm_menu_down));
                LSP_STATUS_ASSERT(add_menu_item(menu, "actions.edit.move_last", slot_on_bm_menu_last));
            }

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
                {
                    sync_filters();
                    refresh_current_path();
                }
            }
            if (sOptions.is(prop))
            {
                for (size_t i=wOptions.items()->size(); i > 1; --i)
                    wOptions.items()->remove(i-1);
                Widget *opts = sOptions.get();
                if (opts != NULL)
                {
                    wOptions.add(&wSeparator);
                    wOptions.add(opts);
                }
            }
            if (sPreview.is(prop))
            {
                tk::Widget *w = sPreview.get();
                wPreview.remove_all();
                if (w != NULL)
                    wPreview.add(w);
                wPreview.visibility()->set(w != NULL);
                wPreviewHeading.visibility()->set(w != NULL);
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

        status_t FileDialog::add_ext_check(Grid *c, const char *text, size_t rows, size_t cols)
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
            if (result == STATUS_OK)
                result = inject_style(lbl, "FileDialog::Label");

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
                result = c->add(&sAppendExt, rows, cols);

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

        status_t FileDialog::slot_on_change(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            return (dlg != NULL) ? dlg->on_change() : STATUS_BAD_STATE;
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

        status_t FileDialog::slot_on_filter_submit(Widget *sender, void *ptr, void *data)
        {
            FileDialog *dlg = widget_ptrcast<FileDialog>(ptr);
            if (dlg == NULL)
                return STATUS_OK;

            const Widget *w = dlg->sWFilter.selected()->get();
            const size_t index  = dlg->sWFilter.items()->index_of(w);
            dlg->sSelFilter.commit_value(index);

            return dlg->on_dlg_search(data);
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
            if (_this == NULL)
                return STATUS_OK;
            bm_entry_t *bm = _this->pPopupBookmark;
            return (bm != NULL) ? _this->on_bm_submit(&bm->sHlink) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_follow(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            if (_this == NULL)
                return STATUS_OK;
            bm_entry_t *bm = _this->pPopupBookmark;

            return (bm != NULL) ? bm->sHlink.follow_url() : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_copy(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            if (_this == NULL)
                return STATUS_OK;
            bm_entry_t *bm = _this->pPopupBookmark;

            return (bm != NULL) ? bm->sHlink.copy_url(ws::CBUF_CLIPBOARD) : STATUS_OK;
        }

        status_t FileDialog::slot_on_bm_menu_delete(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            if (_this == NULL)
                return STATUS_OK;
            bm_entry_t *bm = _this->pPopupBookmark;

            return (bm != NULL) ? _this->remove_bookmark(bm) : STATUS_OK;
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

            ssize_t index = _this->vBookmarks.index_of(bm);
            if (index < 0)
                return STATUS_OK;

            if (!_this->vBookmarks.prepend(bm))
                return STATUS_UNKNOWN_ERR;
            _this->vBookmarks.remove(index + 1);

            return _this->sync_bookmarks();
        }

        status_t FileDialog::slot_on_bm_menu_last(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            bm_entry_t *bm = _this->pPopupBookmark;
            if (bm == NULL)
                return STATUS_OK;

            ssize_t index = _this->vBookmarks.index_of(bm);
            if (index < 0)
                return STATUS_OK;

            if (!_this->vBookmarks.append(bm))
                return STATUS_UNKNOWN_ERR;
            _this->vBookmarks.remove(index);

            return _this->sync_bookmarks();
        }

        status_t FileDialog::slot_on_bm_realized(Widget *sender, void *ptr, void *data)
        {
            FileDialog *_this = widget_ptrcast<FileDialog>(ptr);
            ScrollArea *_area = widget_cast<ScrollArea>(sender);
            if ((_this == NULL) || (_area == NULL))
                return STATUS_OK;
            size_t n    = _this->vBookmarks.size() + _this->vVolumes.size();
            if (n <= 0)
                return STATUS_OK;

            ws::rectangle_t sa, sb;
            _this->wSAAccess.get_rectangle(&sa);
            _this->wSABox.get_rectangle(&sb);
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
            // Analyze what to do
            f_entry_t *ent = selected_entry();
            if (ent == NULL)
            {
                LSP_STATUS_ASSERT(sSelected.set_raw(""));
                sSlots.execute(SLOT_CHANGE, this, NULL);
                return STATUS_OK;
            }

            // Update the search field in the 'save file' mode
            if (sMode.save_file())
            {
                if (!(ent->nFlags & (F_DOTDOT | F_ISDIR)))
                    LSP_STATUS_ASSERT(sWSearch.text()->set_raw(&ent->sName));
            }

            // Update the selected file variable
            LSPString spath;
            io::Path path;
            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath, &ent->sName));
            LSP_STATUS_ASSERT(sSelected.set_raw(path.as_string()));
            sSlots.execute(SLOT_CHANGE, this, NULL);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_search(void *data)
        {
            if (!sVisibility.get())
                return STATUS_OK;

            sWFiles.selected()->clear();
            status_t res = apply_filters();

            LSP_STATUS_ASSERT(sSelected.set_raw(""));
            sSlots.execute(SLOT_CHANGE, this, NULL);

            return res;
        }

        status_t FileDialog::on_dlg_action(void *data, bool list)
        {
            LSPString fname, spath;
            io::Path path;

            LSP_STATUS_ASSERT(sWSearch.text()->format(&fname));
            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));

            f_entry_t *ent = selected_entry();

            if (list)
            {
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
                if (wAutoExt.checked()->get())
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
            else if ((ent == NULL) && (!io::Path::is_dots(&fname)) && (io::Path::valid_file_name(&fname)))
            {
                LSP_STATUS_ASSERT(path.append_child(&fname));
            }
            else // Use selection
            {
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

                Shortcut *scut = NULL;
                if ((scut = pWConfirm->shortcuts()->append(ws::WSK_ESCAPE, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_confirm_escape, self());
                if ((scut = pWConfirm->shortcuts()->append(ws::WSK_RETURN, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_confirm_return, self());
                if ((scut = pWConfirm->shortcuts()->append(ws::WSK_KEYPAD_ENTER, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_confirm_return, self());
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
            drop_volumes();
            drop_bookmarks();

            // Execute slots
            return sSlots.execute(SLOT_SUBMIT, this, data);
        }

        status_t FileDialog::on_dlg_cancel(void *data)
        {
            // Hide dialogs
            if (pWConfirm != NULL)
                pWConfirm->hide();
            drop_volumes();
            drop_bookmarks();
            hide();
            destroy_file_entries(&vFiles);

            // Execute slots
            return sSlots.execute(SLOT_CANCEL, this, data);
        }

        status_t FileDialog::on_show()
        {
            sync_filters();
            refresh_volumes();
            refresh_bookmarks();
            select_current_bookmark();
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
            if (bm == NULL)
                return STATUS_OK;
            status_t res = sPath.set_raw(&bm->sBookmark.path);
            if (res == STATUS_OK)
                select_bookmark(bm);
            return res;
        }

        status_t FileDialog::on_dlg_go(void *data)
        {
            io::Path path;
            LSPString spath;

            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));
            LSP_STATUS_ASSERT(path.canonicalize());
            LSP_STATUS_ASSERT(sPath.set_raw(path.as_string()));

            // Clear the file selection
            LSP_STATUS_ASSERT(sSelected.set_raw(""));
            sSlots.execute(SLOT_CHANGE, this, NULL);

            return STATUS_OK;
        }

        status_t FileDialog::on_dlg_up(void *data)
        {
            io::Path path;
            LSPString spath;

            LSP_STATUS_ASSERT(sWPath.text()->format(&spath));
            LSP_STATUS_ASSERT(path.set(&spath));
            LSP_STATUS_ASSERT(path.remove_last());
            LSP_STATUS_ASSERT(path.canonicalize());
            LSP_STATUS_ASSERT(sPath.set_raw(path.as_string()));

            // Clear the file selection
            LSP_STATUS_ASSERT(sSelected.set_raw(""));
            sSlots.execute(SLOT_CHANGE, this, NULL);

            return STATUS_OK;
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

        status_t FileDialog::read_lnk_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm)
        {
            io::Path path;
            status_t res = system::get_home_directory(&path);
            if (res != STATUS_OK)
                return res;
            if ((res = path.append_child(LNK_BOOKMARK_PATH)) != STATUS_OK)
                return res;

            return bookmarks::read_bookmarks_lnk(&vbm, &path);
        }

        ssize_t FileDialog::compare_volume_info(const system::volume_info_t *a, const system::volume_info_t *b)
        {
            return a->target.compare_to(&b->target);
        }

        status_t FileDialog::refresh_volumes()
        {
            status_t res;
            io::Path path;

            // Drop previously created volumes
            drop_volumes();

            // Obtain the volume information
            lltl::parray<system::volume_info_t> vi;
            if ((res = system::get_volume_info(&vi)) != STATUS_OK)
                return res;
            lsp_finally { system::free_volume_info(&vi); };
            vi.qsort(compare_volume_info);

            // Create widgets
            bm_entry_t *ent = NULL;
            lsp_finally {
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
            };

            for (size_t i=0, n=vi.size(); i<n; ++i)
            {
                system::volume_info_t *v = vi.uget(i);
                if ((v == NULL) || (!(v->flags & (system::VF_DRIVE | system::VF_REMOTE))))
                    continue;

                // Allocate entry
                if ((ent = new bm_entry_t(pDisplay)) == NULL)
                {
                    res = STATUS_NO_MEM;
                    break;
                }

                // Initialize data
                if ((res = path.set(&v->target)) != STATUS_OK)
                    break;
                if ((res = init_bookmark_entry(ent, &v->target, &path, false)) != STATUS_OK)
                    break;
                ent->sBookmark.origin = 0;

                // Add to the list
                if ((res = wVolumes.add(&ent->sHlink)) != STATUS_OK)
                    break;

                // Commit to list
                res     = (vVolumes.add(ent)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;
                ent     = NULL;
            }

            if (res != STATUS_OK)
                drop_volumes();

            return STATUS_OK;
        }

        status_t FileDialog::refresh_bookmarks()
        {
            io::Path path;

            // Drop previously created bookmarks
            drop_bookmarks();

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
            if ((res = read_lnk_bookmarks(tmp)) == STATUS_OK)
                bookmarks::merge_bookmarks(&bm, &changes, &tmp, bookmarks::BM_LNK);
            bookmarks::destroy_bookmarks(&tmp);

            // Check if we need to store bookmarks
            if ((changes > 0) || (xres != STATUS_OK))
                save_bookmarks(&bm);

            // Create widgets
            bm_entry_t *ent = NULL;
            lsp_finally {
                if (ent != NULL)
                {
                    ent->sHlink.destroy();
                    delete ent;
                }
            };

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
                if ((res = path.set(&b->path)) != STATUS_OK)
                    break;
                if ((res = init_bookmark_entry(ent, &b->name, &path, true)) != STATUS_OK)
                    break;
                ent->sBookmark.origin = b->origin;

                if (b->origin & bookmarks::BM_LSP)
                {
                    if ((res = wBookmarks.add(&ent->sHlink)) != STATUS_OK)
                        break;
                }

                // Commit to list
                res     = (vBookmarks.add(ent)) ? STATUS_OK : STATUS_NO_MEM;
                if (res != STATUS_OK)
                    break;
                ent     = NULL;
            }

            bookmarks::destroy_bookmarks(&bm);

            if (res != STATUS_OK)
                drop_bookmarks();

            return res;
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
            wBookmarks.remove(&entry->sHlink);
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

        void FileDialog::select_bookmark(bm_entry_t *bm)
        {
            // Check state
            if (bm == pSelBookmark)
                return;

            // Deactivate selected bookmark
            if (pSelBookmark != NULL)
            {
                pSelBookmark->sHlink.style()->remove_parent(pBMSel);
                pSelBookmark->sHlink.style()->inject_parent(pBMNormal);
            }
            pSelBookmark = bm;
            if (pSelBookmark != NULL)
            {
                pSelBookmark->sHlink.style()->remove_parent(pBMNormal);
                pSelBookmark->sHlink.style()->inject_parent(pBMSel);
            }

            // Clear the file selection
            sSelected.set_raw("");
            sSlots.execute(SLOT_CHANGE, this, NULL);
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

            // Lookup around volumes
            for (size_t i=0, n=vVolumes.size(); i<n; ++i)
            {
                bm_entry_t *ent = vVolumes.uget(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    select_bookmark(ent);
                    return STATUS_OK;
                }
            }

            // Lookup around bookmarks
            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if ((ent != NULL) && (ent->sPath.equals(&path)))
                {
                    select_bookmark(ent);
                    return STATUS_OK;
                }
            }

            return STATUS_OK;
        }

        status_t FileDialog::sync_bookmarks()
        {
            status_t res;
            if ((res = wBookmarks.remove_all()) != STATUS_OK)
                return res;

            for (size_t i=0, n=vBookmarks.size(); i<n; ++i)
            {
                bm_entry_t *ent = vBookmarks.uget(i);
                if ((ent == NULL) || (!(ent->sBookmark.origin & bookmarks::BM_LSP)))
                    continue;
                if ((res = wBookmarks.add(&ent->sHlink)) != STATUS_OK)
                    break;
            }

            if (res != STATUS_OK)
                wBookmarks.remove_all();

            return (res == STATUS_OK) ? save_bookmarks(NULL) : res;
        }

        FileDialog::bm_entry_t *FileDialog::find_bookmark(Widget *sender)
        {
            Hyperlink *hlink = widget_cast<Hyperlink>(sender);
            if (hlink == NULL)
                return NULL;

            for (size_t i=0, n=vVolumes.size(); i<n; ++i)
            {
                bm_entry_t *ent = vVolumes.uget(i);
                if ((ent != NULL) && (hlink == &ent->sHlink))
                    return ent;
            }
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

            if ((res = init_bookmark_entry(ent, NULL, &path, true)) != STATUS_OK)
            {
                vBookmarks.premove(ent);
                ent->sHlink.destroy();
                delete ent;
                return STATUS_NO_MEM;
            }

            // Synchronize bookmarks and exit
            return sync_bookmarks();
        }

        status_t FileDialog::init_bookmark_entry(bm_entry_t *ent, const LSPString *name, const io::Path *path, bool editable)
        {
            status_t res;
            LSPString url;

            ent->sBookmark.origin   = bookmarks::BM_LSP;
            if (name == NULL)
                res     = path->get_last(&ent->sBookmark.name);
            else
                res = (ent->sBookmark.name.set(name)) ? STATUS_OK : STATUS_NO_MEM;
            if (res != STATUS_OK)
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
            ent->sHlink.style()->inject_parent(pBMNormal);
            ent->sHlink.url()->set_raw(&url);
            ent->sHlink.slots()->bind(SLOT_SUBMIT, slot_on_bm_submit, self());
            ent->sHlink.slots()->bind(SLOT_BEFORE_POPUP, slot_on_bm_popup, self());
            ent->sHlink.slots()->bind(SLOT_MOUSE_SCROLL, slot_on_bm_scroll);
            ent->sHlink.popup()->set((editable) ? &wBMPopup : &wVolPopup);

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

        status_t FileDialog::on_change()
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
                wWWarning.hide();

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
                    case STATUS_PERMISSION_DENIED:      text = "permission denied"; break;
                    case STATUS_NOT_FOUND:              text = "directory does not exist"; break;
                    case STATUS_NOT_DIRECTORY:          text = "not a directory"; break;
                    case STATUS_NO_MEM:                 text = "not enough memory"; break;
                    case STATUS_NO_DATA:                text = "no data"; break;
                    default: break;
                }

                str.set_native("Access error: ");
                path.set_native(text);
                str.append(&path);
                wWWarning.text()->set_raw(&str);
                wWWarning.show();
            }

            // Now we have the complete list of files, need to reorder them
            scanned.qsort(cmp_file_entry);

            // Alright, now we can swap contents and delete previous contents
            vFiles.swap(&scanned);
            destroy_file_entries(&scanned);

            apply_filters();

            return select_current_bookmark();
        }

        ssize_t FileDialog::cmp_file_entry(const f_entry_t *a, const f_entry_t *b)
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
                {
                    item->tag()->set(i);
                    res = sWFilter.items()->madd(item); // Add in managed mode
                }
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
            status_t res;

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

                if ((res = item->init()) != STATUS_OK)
                {
                    delete item;
                    return res;
                }
                item->text()->set_raw(psrc);
                item->tag()->set(i);
                if ((res = lst->madd(item)) != STATUS_OK)
                {
                    delete item;
                    return res;
                }

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

                // Global message box event handling
                Shortcut *scut = NULL;
                if ((scut = pWMessage->shortcuts()->append(ws::WSK_ESCAPE, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_message_close, pWMessage->self());
                if ((scut = pWMessage->shortcuts()->append(ws::WSK_RETURN, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_message_close, pWMessage->self());
                if ((scut = pWMessage->shortcuts()->append(ws::WSK_KEYPAD_ENTER, KM_NONE)) != NULL)
                    scut->slot()->bind(slot_on_message_close, pWMessage->self());
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

        status_t FileDialog::slot_on_dialog_escape(Widget *sender, void *ptr, void *data)
        {
            FileDialog *self = widget_ptrcast<FileDialog>(ptr);
            if (self == NULL)
                return STATUS_OK;

            lsp_trace("on_dialog_escape sender=%p", sender);

            return self->on_dlg_cancel(data);
        }

        status_t FileDialog::slot_on_dialog_return(Widget *sender, void *ptr, void *data)
        {
            FileDialog *self = widget_ptrcast<FileDialog>(ptr);
            if (self == NULL)
                return STATUS_OK;

            if ((sender == &self->sWFiles) || (sender == &self->sWPath))
                return STATUS_OK;

            lsp_trace("on_dialog_return sender=%p", sender);

            return self->on_dlg_action(data, false);
        }

        status_t FileDialog::slot_on_message_close(Widget *sender, void *ptr, void *data)
        {
            MessageBox *mbox = widget_ptrcast<MessageBox>(ptr);
            if (mbox == NULL)
                return STATUS_OK;

            mbox->hide();
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_confirm_escape(Widget *sender, void *ptr, void *data)
        {
            FileDialog *self = widget_ptrcast<FileDialog>(ptr);
            if (self == NULL)
                return STATUS_OK;

            self->pWConfirm->hide();
            return STATUS_OK;
        }

        status_t FileDialog::slot_on_confirm_return(Widget *sender, void *ptr, void *data)
        {
            FileDialog *self = widget_ptrcast<FileDialog>(ptr);
            if (self == NULL)
                return STATUS_OK;

            return self->on_dlg_confirm(data);
        }

    } /* namespace tk */
} /* namespace lsp */


