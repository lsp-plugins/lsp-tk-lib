/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 19 июн. 2017 г.
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
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/ws/factory.h>
#include <lsp-plug.in/i18n/Dictionary.h>
#include <private/tk/style/BuiltinStyle.h>

namespace lsp
{
    namespace tk
    {
        Display::Display(display_settings_t *settings):
            sSchema(this, this)
        {
            pDictionary     = NULL;
            pDisplay        = NULL;
            pResourceLoader = NULL;
            pEnv            = NULL;

            // Apply custom settings
            if (settings != NULL)
            {
                pResourceLoader     = settings->resources;
                pEnv                = (settings->environment != NULL) ? settings->environment->clone() : NULL;
            }
        }

        Display::~Display()
        {
            do_destroy();
        }
        
        void Display::destroy()
        {
            garbage_collect();
            do_destroy();
        }

        void Display::do_destroy()
        {
            // Auto-destruct widgets
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.uget(i);
                if (ptr == NULL)
                    continue;

                ptr->id         = NULL;
                if (ptr->widget != NULL)
                {
                    ptr->widget->destroy();
                    delete ptr->widget;
                    ptr->widget = NULL;
                }
                ::free(ptr);
            }
            sWidgets.flush();

            // Execute slot
            sSlots.execute(SLOT_DESTROY, NULL);
            sSlots.destroy();

            // Destroy display
            if (pDisplay != NULL)
            {
                ws::free_display(pDisplay);
                pDisplay = NULL;
            }

            // Destroy dictionary
            if (pDictionary != NULL)
            {
                delete pDictionary;
                pDictionary = NULL;
            }

            // Destroy environment
            if (pEnv != NULL)
            {
                delete pEnv;
                pEnv        = NULL;
            }
        }

        status_t Display::main_task_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            Display *_this   = static_cast<Display *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            _this->slots()->execute(tk::SLOT_IDLE, NULL, _this);
            _this->garbage_collect();

            return STATUS_OK;
        }

        void Display::garbage_collect()
        {
            for (size_t i=0, n=vGarbage.size(); i<n; ++i)
            {
                // Get widget
                Widget *w = vGarbage.uget(i);
                if (w == NULL)
                    continue;

                // Widget is registered?
                for (size_t j=0, m=sWidgets.size(); j<m; )
                {
                    item_t *item = sWidgets.uget(j);
                    if (item->widget == w)
                    {
                        // Free the binding
                        sWidgets.qremove(j);
                        item->id        = NULL;
                        item->widget    = NULL;
                        ::free(item);
                    }
                    else
                        ++j;
                }

                // Destroy widget
                w->destroy();
                delete w;
            }

            // Cleanup garbage
            vGarbage.flush();
        }

        status_t Display::init(int argc, const char **argv)
        {
            // Create display
            ws::IDisplay *dpy = ws::create_display(argc, argv);
            if (dpy == NULL)
                return STATUS_NO_MEM;

            if (!sSlots.add(tk::SLOT_IDLE))
                return STATUS_NO_MEM;

            status_t res = init(dpy, argc, argv);

            if (res != STATUS_OK)
            {
                dpy->destroy();
                ws::free_display(dpy);
            }

            return res;
        }

        status_t Display::init(ws::IDisplay *dpy, int argc, const char **argv)
        {
            // Should be non-null
            if (dpy == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Initialize environment
            if (pEnv == NULL)
                pEnv                = new resource::Environment();
            if (pEnv == NULL)
                return STATUS_NO_MEM;

            // Initialize dictionary
            LSPString dict_base;
            const char *env_dict_base = pEnv->get_utf8(LSP_TK_ENV_DICT_PATH, LSP_TK_ENV_DICT_PATH_DFL);
            if (!dict_base.set_utf8(env_dict_base))
                return STATUS_NO_MEM;

            // Initialize dictionary
            pDictionary  = new i18n::Dictionary(pResourceLoader);
            if (pDictionary == NULL)
                return STATUS_NO_MEM;

            status_t res = pDictionary->init(&dict_base);
            if (res != STATUS_OK)
                return res;

            // Create slots
            Slot *slot   = sSlots.add(SLOT_DESTROY);
            if (slot == NULL)
                return STATUS_NO_MEM;
            slot = sSlots.add(SLOT_RESIZE);
            if (slot == NULL)
                return STATUS_NO_MEM;

            // Initialize schema
            pDisplay        = dpy;
            if ((res = init_schema()) != STATUS_OK)
            {
                pDisplay        = NULL;
                return res;
            }

            // Remember the display handle
            dpy->set_main_callback(main_task_handler, this);

            return STATUS_OK;
        }

        status_t Display::init_schema()
        {
            // Form the list of initializers
            status_t res = STATUS_OK;
            lltl::parray<IStyleFactory> init;
            for (BuiltinStyle *curr = BuiltinStyle::root(); curr != NULL; curr = curr->next())
            {
                if (!init.add(curr->init()))
                    return STATUS_NO_MEM;
            }

            // Initialize schema
            res = sSchema.init(&init);
            if (res != STATUS_OK)
                return res;

            // Set-up default language
            const LSPString *language = pEnv->get(LSP_TK_ENV_LANG);
            res = (language != NULL) ?
                    sSchema.set_lanugage(language) :
                    sSchema.set_lanugage(LSP_TK_PROP_DEFAULT_LANGUAGE);
            if (res != STATUS_OK)
                return res;

            // Load schema settings
            const char *schema_path = pEnv->get_utf8(LSP_TK_ENV_SCHEMA_PATH);
            if (schema_path == NULL)
                return STATUS_OK;

            // Load style sheet
            StyleSheet sheet;
            io::IInSequence *is = pResourceLoader->read_sequence(schema_path);
            if (is == NULL)
                return STATUS_NOT_FOUND;

            // Parse style
            res = sheet.parse_data(is, WRAP_CLOSE | WRAP_DELETE);
            if (res != STATUS_OK)
                return res;

            // Apply loaded schema
            return sSchema.apply(&sheet);
        }

        status_t Display::main()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->main();
        }

        status_t Display::wait_events(wssize_t millis)
        {
            return (pDisplay != NULL) ? pDisplay->wait_events(millis) : STATUS_BAD_STATE;
        }

        status_t Display::main_iteration()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            if (!sLock.lock())
                return STATUS_UNKNOWN_ERR;
            status_t res = pDisplay->main_iteration();
            if (!sLock.unlock())
                return STATUS_UNKNOWN_ERR;
            return res;
        }

        void Display::quit_main()
        {
            if (pDisplay != NULL)
                pDisplay->quit_main();
        }

        ws::ISurface *Display::create_surface(size_t width, size_t height)
        {
            return (pDisplay != NULL) ? pDisplay->create_surface(width, height) : NULL;
        }

        void Display::sync()
        {
            if (pDisplay != NULL)
                pDisplay->sync();
        }

        status_t Display::add(Widget *widget, const char *id)
        {
            Widget **w = add(id);
            if (w == NULL)
                return STATUS_NO_MEM;

            *w = widget;
            return STATUS_OK;
        }

        Widget **Display::add(const char *id)
        {
            // Prevent from duplicates
            if (id != NULL)
            {
                // Check that widget does not exist
                Widget *widget = get(id);
                if (widget != NULL)
                    return NULL;
            }

            // Allocate memory
            size_t slen     = (id != NULL) ? (::strlen(id) + 1) * sizeof(char) : 0;
            size_t to_alloc = align_size(sizeof(item_t) + slen, DEFAULT_ALIGN);

            // Append widget
            item_t *w   = reinterpret_cast<item_t *>(::malloc(to_alloc));
            if (w == NULL)
                return NULL;
            else if (!sWidgets.add(w))
            {
                ::free(w);
                return NULL;
            }

            // Initialize widget
            w->widget       = NULL;
            w->id           = NULL;
            if (id != NULL)
            {
                w->id           = reinterpret_cast<char *>(&w[1]);
                ::memcpy(w->id, id, slen);
            }

            return &w->widget;
        }

        Widget *Display::get(const char *id)
        {
            if (id == NULL)
                return NULL;

            for (size_t i=0, n=sWidgets.size(); i<n; ++i)
            {
                item_t *ptr     = sWidgets.uget(i);
                if (ptr->id == NULL)
                    continue;
                if (!strcmp(ptr->id, id))
                    return ptr->widget;
            }

            return NULL;
        }

        Widget *Display::remove(const char *id)
        {
            if (id == NULL)
                return NULL;

            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.uget(i);
                if (ptr->id == NULL)
                    continue;
                if (!strcmp(ptr->id, id))
                {
                    Widget *result = ptr->widget;
                    sWidgets.remove(i);
                    return result;
                }
            }

            return NULL;
        }

        bool Display::remove(Widget *widget)
        {
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.uget(i);
                if (ptr->widget == widget)
                {
                    sWidgets.remove(i);
                    return true;
                }
            }

            return false;
        }

        bool Display::exists(Widget *widget)
        {
            size_t n    = sWidgets.size();
            for (size_t i=0; i<n; ++i)
            {
                item_t *ptr     = sWidgets.uget(i);
                if (ptr->widget == widget)
                    return true;
            }

            return false;
        }

        status_t Display::get_clipboard(size_t id, ws::IDataSink *sink)
        {
            return pDisplay->get_clipboard(id, sink);
        }

        status_t Display::set_clipboard(size_t id, ws::IDataSource *src)
        {
            return pDisplay->set_clipboard(id, src);
        }

        status_t Display::reject_drag()
        {
            return pDisplay->reject_drag();
        }

        status_t Display::accept_drag(ws::IDataSink *sink, ws::drag_t action, const ws::rectangle_t *r)
        {
            return pDisplay->accept_drag(sink, action, r);
        }

        const char * const *Display::get_drag_mime_types()
        {
            return pDisplay->get_drag_ctypes();
        }

        status_t Display::queue_destroy(Widget *widget)
        {
            return vGarbage.add(widget) ? STATUS_OK : STATUS_NO_MEM;
        }

        const ws::MonitorInfo *Display::enum_monitors(size_t *count)
        {
            return pDisplay->enum_monitors(count);
        }

        status_t Display::work_area_geometry(ws::rectangle_t *r)
        {
            return pDisplay->work_area_geometry(r);
        }

        size_t Display::idle_interval()
        {
            return pDisplay->idle_interval();
        }

        size_t Display::set_idle_interval(size_t interval)
        {
            return pDisplay->set_idle_interval(interval);
        }

    } /* namespace tk */
} /* namespace lsp */
