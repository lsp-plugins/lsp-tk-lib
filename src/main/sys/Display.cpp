/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

namespace lsp
{
    namespace tk
    {
        Display::Display(display_settings_t *settings):
            sSchema(this)
        {
            pDictionary     = NULL;
            pDisplay        = NULL;

            // Apply custom settings
            if (settings != NULL)
            {
                pResourceLoader     = settings->resources;
                if (settings->dictionary != NULL)
                    sDictBase.set_utf8(settings->dictionary);
            }
        }

        Display::~Display()
        {
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
                ws::lsp_ws_free_display(pDisplay);
                pDisplay = NULL;
            }

            // Destroy dictionary
            if (pDictionary != NULL)
            {
                delete pDictionary;
                pDictionary = NULL;
            }
        }

        status_t Display::main_task_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg)
        {
            Display *_this   = reinterpret_cast<Display *>(arg);
            if (_this == NULL)
                return STATUS_BAD_ARGUMENTS;

            for (size_t i=0, n=_this->vGarbage.size(); i<n; ++i)
            {
                // Get widget
                Widget *w = _this->vGarbage.uget(i);
                if (w == NULL)
                    continue;

                // Widget is registered?
                for (size_t j=0, m=_this->sWidgets.size(); j<m; )
                {
                    item_t *item = _this->sWidgets.uget(j);
                    if (item->widget == w)
                    {
                        // Free the binding
                        _this->sWidgets.qremove(j);
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
            _this->vGarbage.flush();

            return STATUS_OK;
        }

        status_t Display::init(int argc, const char **argv)
        {
            // Create display
            ws::IDisplay *dpy = ws::lsp_ws_create_display(argc, argv);
            if (dpy == NULL)
                return STATUS_NO_MEM;

            status_t res = init(dpy, argc, argv);

            if (res != STATUS_OK)
            {
                dpy->destroy();
                ws::lsp_ws_free_display(dpy);
            }

            return res;
        }

        status_t Display::init(ws::IDisplay *dpy, int argc, const char **argv)
        {
            // Should be non-null
            if (dpy == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Initialize dictionary
            i18n::Dictionary *dict = new i18n::Dictionary(pResourceLoader);
            if (dict == NULL)
                return STATUS_NO_MEM;
            status_t res = dict->init(&sDictBase);
            if (res != STATUS_OK)
            {
                delete dict;
                return res;
            }

            // Initialize display
            pDisplay        = dpy;
            pDisplay->set_main_callback(main_task_handler, this);

            // Create slots
            Slot *slot   = sSlots.add(SLOT_DESTROY);
            if (slot == NULL)
            {
                delete dict;
                return STATUS_NO_MEM;
            }
            slot = sSlots.add(SLOT_RESIZE);
            if (slot == NULL)
            {
                delete dict;
                return STATUS_NO_MEM;
            }

            // Initialize theme
//            sTheme.init(this); // TODO
            pDictionary     = dict;

            return STATUS_OK;
        }

        void Display::destroy()
        {
            do_destroy();
        }

        status_t Display::main()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->main();
        }

        status_t Display::main_iteration()
        {
            if (pDisplay == NULL)
                return STATUS_BAD_STATE;
            return pDisplay->main_iteration();
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

        status_t Display::accept_drag(ws::IDataSink *sink, ws::drag_t action, bool internal, const ws::rectangle_t *r)
        {
            return pDisplay->accept_drag(sink, action, internal, r);
        }

        const char * const *Display::get_drag_mime_types()
        {
            return pDisplay->get_drag_ctypes();
        }

        status_t Display::queue_destroy(Widget *widget)
        {
            return vGarbage.add(widget) ? STATUS_OK : STATUS_NO_MEM;
        }
    }

} /* namespace lsp */
