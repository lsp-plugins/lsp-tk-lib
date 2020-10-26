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

namespace lsp
{
    namespace tk
    {
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
                Edit                    sWPath;
                Edit                    sWSearch;
                ComboBox                sWFilter;
                ListBox                 sWFiles;
                Button                  sWAction;
                Button                  sWCancel;
                Grid                    sMainGrid;
                ScrollArea              sSBBookmarks;
                Align                   sSBAlign;
                Box                     sBookmarks;
                Menu                    sBMPopup;
                Button                  sBMAdd;
                Box                     sHBox;
                Box                     sWarnBox;
                Align                   sAppendExt;
                Button                  wAutoExt;
                Button                  wGo;
                Button                  wUp;
                Box                     wPathBox;
                MessageBox             *pWConfirm;
                MessageBox             *pWMessage;
                Label                   sWWarning;
                Label                  *pWSearch;

                lltl::parray<Widget>    vWidgets;

                prop::FileDialogMode    sMode;
                prop::Boolean           sCustomAction;
                prop::String            sActionText;

            protected:
                static status_t         slot_on_action(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_confirm(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_cancel(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_search(Widget *sender, void *ptr, void *data);
                static status_t         slot_mouse_dbl_click(Widget *sender, void *ptr, void *data);
                static status_t         slot_list_change(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_go(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_up(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_path_key_up(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_add(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_submit(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_popup(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_open(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_follow(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_copy(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_delete(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_up(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_down(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_first(Widget *sender, void *ptr, void *data);
                static status_t         slot_on_bm_menu_last(Widget *sender, void *ptr, void *data);

            protected:
                virtual status_t        on_dlg_action(void *data);
                virtual status_t        on_dlg_confirm(void *data);
                virtual status_t        on_dlg_cancel(void *data);
                virtual status_t        on_dlg_search(void *data);
                virtual status_t        on_dlg_mouse_dbl_click(void *data);
                virtual status_t        on_dlg_list_change(void *data);
                virtual status_t        on_dlg_go(void *data);
                virtual status_t        on_dlg_up(void *data);
                virtual status_t        on_path_key_up(ws::event_t *e);
                virtual status_t        on_bm_submit(Widget *sender);

            protected:
                status_t                add_label(WidgetContainer *c, const char *key, float align = -1.0f, Label **label = NULL);
                status_t                add_menu_item(Menu *m, const char *text, event_handler_t handler);
                status_t                add_ext_button(WidgetContainer *c, const char *text);
                void                    sync_mode();

            protected:
                virtual void            property_changed(Property *prop);

            public:
                explicit FileDialog(Display *dpy);
                virtual ~FileDialog();

                virtual status_t                init();
                virtual void                    destroy();

            public:
                LSP_TK_PROPERTY(FileDialogMode,             mode,               &sMode);
                LSP_TK_PROPERTY(Boolean,                    custom_action,      &sCustomAction);
                LSP_TK_PROPERTY(String,                     action_text,        &sActionText);
                LSP_TK_PROPERTY(Color,                      warn_color,         sWWarning.color());
                LSP_TK_PROPERTY(Color,                      auto_ext_color,     wAutoExt.color());

            public:
                virtual status_t        on_show();

                virtual status_t        on_close(const ws::event_t *e);
        };
    }
}



#endif /* LSP_PLUG_IN_TK_WIDGETS_DIALOGS_FILEDIALOG_H_ */
