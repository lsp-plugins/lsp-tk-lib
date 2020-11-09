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

#ifndef LSP_PLUG_IN_TK_WIDGETS_DIALOGS_FILEDIALOG_H_
#define LSP_PLUG_IN_TK_WIDGETS_DIALOGS_FILEDIALOG_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/fmt/bookmarks.h>
#include <lsp-plug.in/lltl/parray.h>

namespace lsp
{
    namespace tk
    {
        namespace style
        {
            LSP_TK_STYLE_DEF_BEGIN(FileDialog, Window)
                prop::FileDialogMode        sMode;
                prop::Boolean               sCustomAction;
                prop::String                sActionText;
                prop::String                sPath;
                prop::FileFilters           sFilter;
                prop::Integer               sSelFilter;
                prop::String                sSelected;
                prop::Boolean               sUseConfirm;
                prop::String                sConfirmMsg;
            LSP_TK_STYLE_DEF_END
        }

        /**
         * File load/save dialog
         */
        class FileDialog: public Window
        {
            public:
                static const w_class_t metadata;

            private:
                FileDialog & operator = (const FileDialog &);

            protected:
                enum
                {
                    F_ISDIR     = 1 << 0,
                    F_ISLINK    = 1 << 1,
                    F_ISREG     = 1 << 2,
                    F_ISOTHER   = 1 << 3,
                    F_ISINVALID = 1 << 4,
                    F_DOTDOT    = 1 << 5,
                    F_ISHIDDEN  = 1 << 6
                };

                typedef struct f_entry_t
                {
                    LSPString               sName;
                    size_t                  nFlags;
                } f_entry_t;

                typedef struct bm_entry_t
                {
                    Hyperlink               sHlink;
                    io::Path                sPath;
                    bookmarks::bookmark_t   sBookmark;

                    inline bm_entry_t(Display *dpy): sHlink(dpy) {}
                } bm_entry_t;

            protected:
                Edit                        sWPath;
                Edit                        sWSearch;
                ComboBox                    sWFilter;
                ListBox                     sWFiles;
                Button                      sWAction;
                Button                      sWCancel;
                Grid                        sMainGrid;
                ScrollArea                  sSBBookmarks;
                Align                       sSBAlign;
                Box                         sBookmarks;
                Menu                        sBMPopup;
                Button                      wBMAdd;
                Box                         sActionBox;
                Align                       sActionAlign;
                Box                         sWarnBox;
                Align                       sAppendExt;
                Button                      wAutoExt;
                Button                      wGo;
                Button                      wUp;
                Box                         wNavBox;
                MessageBox                 *pWConfirm;
                MessageBox                 *pWMessage;
                Label                       sWWarning;
                Label                      *pWSearch;

                lltl::parray<Widget>        vWidgets;
                lltl::parray<bm_entry_t>    vBookmarks;
                lltl::parray<f_entry_t>     vFiles;

                bm_entry_t                 *pSelBookmark;
                bm_entry_t                 *pPopupBookmark;

                prop::FileDialogMode        sMode;
                prop::Boolean               sCustomAction;
                prop::String                sActionText;
                prop::String                sPath;
                prop::FileFilters           sFilter;
                prop::Integer               sSelFilter;
                prop::String                sSelected;
                prop::Boolean               sUseConfirm;
                prop::String                sConfirmMsg;

                // External properties
                prop::Color                 sBMTextColor;
                prop::Color                 sBMBgColor;
                prop::Color                 sBMSelTextColor;
                prop::Color                 sBMSelBgColor;
                prop::Color                 sWarnColor;
                prop::Color                 sExtColor;

                // Styles
                Style                      *pNavButton;
                Style                      *pActButton;
                Style                      *pBMNormal;
                Style                      *pBMSel;
                Style                      *pWarning;
                Style                      *pExtCheck;
                Style                      *pActionBox;
                Style                      *pActionAlign;
                Style                      *pNavBox;

            protected:
                static status_t         slot_on_submit(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_cancel(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_btn_action(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_btn_cancel(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_confirm(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_search(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_go(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_up(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_path_key_up(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_list_dbl_click(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_list_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_list_realized(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_list_key_down(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_bm_scroll(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_popup(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_add(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_submit(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_open(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_follow(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_copy(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_delete(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_up(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_down(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_first(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_last(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_realized(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_filter_key_down(Widget *sender, void *ptr, void *data);

                static status_t         slot_on_search_key_down(Widget *sender, void *ptr, void *data);

            protected:
                virtual status_t        on_dlg_action(void *data, bool list);
                virtual status_t        on_dlg_cancel(void *data);

                virtual status_t        on_dlg_confirm(void *data);
                virtual status_t        on_dlg_search(void *data);
                virtual status_t        on_dlg_list_dbl_click(void *data);
                virtual status_t        on_dlg_list_change(void *data);
                virtual status_t        on_dlg_go(void *data);
                virtual status_t        on_dlg_up(void *data);
                virtual status_t        on_path_key_up(ws::event_t *e);
                virtual status_t        on_bm_submit(Widget *sender);

            protected:
                status_t                add_label(WidgetContainer *c, const char *key, float align = -1.0f, Label **label = NULL);
                status_t                add_menu_item(Menu *m, const char *key, event_handler_t handler);
                status_t                add_ext_button(WidgetContainer *c, const char *text);
                status_t                init_bm_popup_menu();
                void                    sync_mode();
                status_t                show_message(const char *title, const char *heading, const char *message, const io::Path *path);

                void                    drop_bookmarks();
                status_t                read_lsp_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm);
                static status_t         read_gtk2_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm);
                static status_t         read_gtk3_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm);
                static status_t         read_qt5_bookmarks(lltl::parray<bookmarks::bookmark_t> &vbm);
                status_t                save_bookmarks(lltl::parray<bookmarks::bookmark_t> *vbm);
                status_t                sync_bookmarks();
                status_t                refresh_bookmarks();
                status_t                select_current_bookmark();
                status_t                remove_bookmark(bm_entry_t *entry);
                bm_entry_t             *find_bookmark(Widget *sender);
                status_t                add_new_bookmark();
                status_t                init_bookmark_entry(bm_entry_t *ent, const io::Path *path);

                void                    destroy_file_entries(lltl::parray<f_entry_t> *list);
                status_t                refresh_current_path();
                status_t                add_file_entry(lltl::parray<f_entry_t> *dst, const char *name, size_t flags);
                status_t                add_file_entry(lltl::parray<f_entry_t> *dst, const LSPString *name, size_t flags);
                static int              cmp_file_entry(const f_entry_t *a, const f_entry_t *b);
                f_entry_t              *selected_entry();

                status_t                sync_filters();
                status_t                apply_filters();

            protected:
                virtual void            property_changed(Property *prop);

            public:
                explicit FileDialog(Display *dpy);
                virtual ~FileDialog();

                virtual status_t                init();
                virtual void                    destroy();

            public:
                LSP_TK_PROPERTY(FileDialogMode,             mode,                           &sMode);
                LSP_TK_PROPERTY(Boolean,                    custom_action,                  &sCustomAction);
                LSP_TK_PROPERTY(String,                     action_text,                    &sActionText);
                LSP_TK_PROPERTY(String,                     path,                           &sPath);
                LSP_TK_PROPERTY(FileFilters,                filter,                         &sFilter);
                LSP_TK_PROPERTY(Integer,                    selected_filter,                &sSelFilter);
                LSP_TK_PROPERTY(String,                     selected_file,                  &sSelected);
                LSP_TK_PROPERTY(Boolean,                    use_confirm,                    &sUseConfirm);
                LSP_TK_PROPERTY(String,                     confirm_message,                &sConfirmMsg);

                LSP_TK_PROPERTY(Color,                      warn_color,                     &sWarnColor);
                LSP_TK_PROPERTY(Color,                      auto_ext_color,                 &sExtColor);

                LSP_TK_PROPERTY(Color,                      bookmark_bg_color,              &sBMBgColor);
                LSP_TK_PROPERTY(Color,                      bookmark_text_color,            &sBMTextColor);
                LSP_TK_PROPERTY(Color,                      bookmark_selected_bg_color,     &sBMSelBgColor);
                LSP_TK_PROPERTY(Color,                      bookmark_selected_text_color,   &sBMSelTextColor);

            public:
                virtual status_t        on_show();

                virtual status_t        on_close(const ws::event_t *e);

                virtual status_t        on_submit();

                virtual status_t        on_cancel();
        };
    }
}



#endif /* LSP_PLUG_IN_TK_WIDGETS_DIALOGS_FILEDIALOG_H_ */
