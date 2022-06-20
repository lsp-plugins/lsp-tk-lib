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

#ifndef LSP_PLUG_IN_TK_DISPLAY_H_
#define LSP_PLUG_IN_TK_DISPLAY_H_

#ifndef LSP_PLUG_IN_TK_IMPL
    #error "use <lsp-plug.in/tk/tk.h>"
#endif

#include <lsp-plug.in/ws/IDisplay.h>
#include <lsp-plug.in/ws/ISurface.h>
#include <lsp-plug.in/ws/IDataSource.h>
#include <lsp-plug.in/ws/IDataSink.h>
#include <lsp-plug.in/i18n/IDictionary.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/ipc/Mutex.h>

namespace lsp
{
    namespace tk
    {
        class Widget;
        class SlotSet;

        /** Main display
         *
         */
        class Display: public Atoms
        {
            private:
                Display & operator = (const Display &);
                Display(const Display &);

                friend class Schema;

            protected:
                typedef struct item_t
                {
                    Widget         *widget;
                    char           *id;
                } item_t;

            protected:
                lltl::parray<item_t>    sWidgets;
                lltl::parray<Widget>    vGarbage;
                ipc::Mutex              sLock;

                SlotSet                 sSlots;
                Schema                  sSchema;

                i18n::IDictionary      *pDictionary;
                ws::IDisplay           *pDisplay;

                resource::ILoader      *pResourceLoader;
                resource::Environment  *pEnv;

            protected:
                void                do_destroy();
                void                garbage_collect();
                status_t            init_schema();

            protected:
                static status_t     main_task_handler(ws::timestamp_t sched, ws::timestamp_t time, void *arg);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                /** Constructor
                 * @param settings additional display settings
                 */
                explicit Display(display_settings_t *settings = NULL);

                /** Destructor
                 *
                 */
                virtual ~Display();

                /** Initialize display
                 *
                 * @param argc number of additional arguments
                 * @param argv list of additional arguments
                 * @return status of operation
                 */
                status_t init(int argc, const char **argv);

                /** Initialize display
                 *
                 * @param dpy underlying display object
                 * @param argc number of additional arguments
                 * @param argv list of additional arguments
                 * @return status of operation
                 */
                status_t init(ws::IDisplay *dpy, int argc, const char **argv);

                /** Destroy display
                 *
                 */
                void destroy();

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Execute main loop function
                 *
                 * @return status of operation
                 */
                status_t main();

                /** Execute one main loop iteration
                 *
                 * @return status of operation
                 */
                status_t main_iteration();

                /**
                 * Wait for new events
                 *
                 * @param millis maximum amount of time to wait for new event
                 */
                status_t wait_events(wssize_t millis);

                /** Interrupt main loop function
                 *
                 */
                void quit_main();

                /**
                 * Synchronize display
                 */
                void sync();

                /** Register widget, it will be automatically destroyed
                 *
                 * @param widget widget to queue
                 * @param id widget identifier
                 * @return status of operation
                 */
                status_t add(Widget *widget, const char *id = NULL);

                /** Add widget and return pointer to write to
                 *
                 * @param id widget identifier
                 * @return pointer or NULL
                 */
                Widget **add(const char *id);

                /** Get widget by it's identifier
                 *
                 * @param id widget identifier
                 * @return pointer to widget
                 */
                Widget *get(const char *id);

                /** Deregister widget by it's identifier
                 *
                 * @param id widget identifier
                 * @return pointer to widget or NULL
                 */
                Widget *remove(const char *id);

                /** Deregister widget by it's pointer
                 *
                 * @param widget widget to remove from queue
                 * @return status of operation
                 */
                bool remove(Widget *widget);

                /** Check that widget exists
                 *
                 * @param id widget pointer
                 * @return true on success
                 */
                bool exists(Widget *widget);

                /** Check that widget exists
                 *
                 * @param id widget identifier
                 * @return true on success
                 */
                inline bool exists(const char *id)          { return get(id) != NULL; }

                /**
                 * Lock the main event loop until unlock() is called
                 * @return true if main event loop has been locked
                 */
                inline bool lock()                          { return sLock.lock();      }

                /**
                 * Perform single try to lock the main event loop until unlock() is called
                 * @return true if main event loop has been locked
                 */
                inline bool try_lock()                      { return sLock.try_lock();  }

                /**
                 * Unlock previously locked main event loop
                 * @return true if main event loop has been unlocked
                 */
                inline bool unlock()                        { return sLock.unlock();    }

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /**
                 * Get style
                 * @return style
                 */
                inline Schema  *schema()                    { return &sSchema; }

                /** Get slots
                 *
                 * @return slots
                 */
                inline SlotSet *slots()                     { return &sSlots; }

                /** Get slot
                 *
                 * @param id slot identifier
                 * @return pointer to slot or NULL
                 */
                inline Slot *slot(slot_t id)                { return sSlots.slot(id); }

                /** Return native display handle
                 *
                 * @return native display handle
                 */
                inline ws::IDisplay *display()              { return pDisplay; }

                /**
                 * Obtain number of screens
                 * @return number of screens
                 */
                inline size_t           screens()           { return pDisplay->screens();       }

                /**
                 * Get the dictionary that contains localization data
                 * @return dictionary object
                 */
                inline i18n::IDictionary *dictionary()      { return pDictionary;               }

                /**
                 * Get environment
                 * @return environment
                 */
                inline resource::Environment *environment() { return pEnv;                      }

                /**
                 * Get clipboard data
                 * @param id clipboard identifier
                 * @param sink data sink
                 * @return status of operation
                 */
                status_t get_clipboard(size_t id, ws::IDataSink *sink);

                /**
                 * Set clipboard data
                 * @param id clipboard identifier
                 * @param src data source
                 * @return status of operation
                 */
                status_t set_clipboard(size_t id, ws::IDataSource *src);

                /**
                 * Reject drag event
                 * @return status of operation
                 */
                status_t reject_drag();

                /**
                 * Accept drag request
                 * @param sink the sink that will handle data transfer
                 * @param action drag action
                 * @param internal true if we want to receive notifications inside of the drag rectangle
                 * @param r parameters of the drag rectangle, can be NULL
                 * @return status of operation
                 */
                status_t accept_drag(ws::IDataSink *sink, ws::drag_t action, bool internal, const ws::rectangle_t *r);

                /**
                 * Get NULL-terminated list of provided MIME types for a drag
                 * @return NULL-terminated list of strings
                 */
                const char * const *get_drag_mime_types();

                /** Get screen size
                 *
                 * @param screen screen identifier
                 * @param w screen width
                 * @param h screen height
                 * @return status of operation
                 */
                inline status_t screen_size(size_t screen, ssize_t *w, ssize_t *h) { return pDisplay->screen_size(screen, w, h); }

                /**
                 * Get current pointer location
                 * @param screen current screen where the pointer is located
                 * @param left pointer to store X position
                 * @param top pointer to store Y position
                 * @return status of operation
                 */
                inline status_t get_pointer_location(size_t *screen, ssize_t *left, ssize_t *top) { return pDisplay->get_pointer_location(screen, left, top); }

                /** Create surface
                 *
                 * @param width surface width
                 * @param height surface height
                 * @return pointer to created surface or NULL
                 */
                ws::ISurface *create_surface(size_t width, size_t height);

                /**
                 * Queue widget for removal. Because all widget operations are done in the
                 * main event loop, it's unsafe to destroy widget immediately in callback
                 * handlers. This method allows to put the widget to the garbage queue that
                 * will be recycled at the end of the event loop iteration.
                 *
                 * @param widget widget to be queued for destroy
                 * @return status of operation
                 */
                status_t queue_destroy(Widget *widget);

                /** Enumerate all monitors in the system for the display,
                 * the resultint pointer is valid until the next enum_monitors() call.
                 *
                 * @param count pointer to store number of enumerated monitors
                 * @return pointer to the array of enumerated monitors
                 */
                const ws::MonitorInfo *enum_monitors(size_t *count);
        };
    }

} /* namespace lsp */

#endif /* LSP_PLUG_IN_TK_DISPLAY_H_ */
