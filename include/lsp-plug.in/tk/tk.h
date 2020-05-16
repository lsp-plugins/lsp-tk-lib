/*
 * tk.h
 *
 *  Created on: 6 мая 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TK_H_
#define LSP_PLUG_IN_TK_TK_H_

#define LSP_PLUG_IN_TK_IMPL

// Version
#include <lsp-plug.in/tk/version.h>

// Basic types
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/tk/types.h>
#include <lsp-plug.in/tk/sys/types.h>
#include <lsp-plug.in/tk/style/types.h>
#include <lsp-plug.in/tk/prop/types.h>

// Basic dependencies
#include <lsp-plug.in/ws/ws.h>

// Interfaces
#include <lsp-plug.in/tk/prop/Listener.h>
#include <lsp-plug.in/tk/style/IStyleListener.h>
#include <lsp-plug.in/tk/style/IStyleInitializer.h>

// Properties
#include <lsp-plug.in/tk/prop/Property.h>
#include <lsp-plug.in/tk/prop/SimpleProperty.h>
#include <lsp-plug.in/tk/prop/MultiProperty.h>
#include <lsp-plug.in/tk/prop/Enum.h>
#include <lsp-plug.in/tk/prop/BitEnum.h>
#include <lsp-plug.in/tk/prop/Boolean.h>
#include <lsp-plug.in/tk/prop/Integer.h>
#include <lsp-plug.in/tk/prop/Float.h>
#include <lsp-plug.in/tk/prop/String.h>
#include <lsp-plug.in/tk/prop/BorderStyle.h>
#include <lsp-plug.in/tk/prop/Color.h>
#include <lsp-plug.in/tk/prop/Padding.h>
#include <lsp-plug.in/tk/prop/Layout.h>
#include <lsp-plug.in/tk/prop/Position.h>
#include <lsp-plug.in/tk/prop/Size.h>
#include <lsp-plug.in/tk/prop/SizeConstraints.h>
#include <lsp-plug.in/tk/prop/WindowActions.h>
#include <lsp-plug.in/tk/prop/WindowPolicy.h>

// Styles and schemas
#include <lsp-plug.in/tk/style/Style.h>
#include <lsp-plug.in/tk/style/Schema.h>

// System objects
#include <lsp-plug.in/tk/sys/Atoms.h>
#include <lsp-plug.in/tk/sys/Slot.h>
#include <lsp-plug.in/tk/sys/SlotSet.h>
#include <lsp-plug.in/tk/sys/Timer.h>
#include <lsp-plug.in/tk/sys/Display.h>

// Base widgets
#include <lsp-plug.in/tk/base/Widget.h>
#include <lsp-plug.in/tk/base/ComplexWidget.h>
#include <lsp-plug.in/tk/base/WidgetContainer.h>
#include <lsp-plug.in/tk/base/cast.h>

// Widgets
#include <lsp-plug.in/tk/widgets/Window.h>


namespace lsp
{
    namespace tk
    {
        /**
         * Initialize toolkit
         * @param argc number of arguments
         * @param argv list of arguments
         */
        void init(int argc, const char **argv);
    }
}

#undef LSP_PLUG_IN_TK_IMPL

#endif /* LSP_PLUG_IN_TK_TK_H_ */
