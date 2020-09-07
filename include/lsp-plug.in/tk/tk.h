/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-tk-lib
 * Created on: 6 мая 2020 г.
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
#include <lsp-plug.in/tk/prop/collection/CollectionListener.h>
#include <lsp-plug.in/tk/style/IStyleListener.h>
#include <lsp-plug.in/tk/style/IStyleInitializer.h>

// Base property definition
#include <lsp-plug.in/tk/prop/base/Property.h>

// Generic properties
#include <lsp-plug.in/tk/prop/base/Ptr.h>
#include <lsp-plug.in/tk/prop/base/SimpleProperty.h>
#include <lsp-plug.in/tk/prop/base/MultiProperty.h>
#include <lsp-plug.in/tk/prop/base/Enum.h>
#include <lsp-plug.in/tk/prop/base/BitEnum.h>
#include <lsp-plug.in/tk/prop/base/Flags.h>

// Enumerations
#include <lsp-plug.in/tk/prop/enum/ButtonMode.h>
#include <lsp-plug.in/tk/prop/enum/BorderStyle.h>
#include <lsp-plug.in/tk/prop/enum/GraphFrameFunction.h>
#include <lsp-plug.in/tk/prop/enum/MenuItemType.h>
#include <lsp-plug.in/tk/prop/enum/WindowPolicy.h>
#include <lsp-plug.in/tk/prop/enum/Pointer.h>
#include <lsp-plug.in/tk/prop/enum/Orientation.h>
#include <lsp-plug.in/tk/prop/enum/Scrolling.h>

// Flags
#include <lsp-plug.in/tk/prop/flags/Allocation.h>
#include <lsp-plug.in/tk/prop/flags/WindowActions.h>

// Single-value properties
#include <lsp-plug.in/tk/prop/simple/Boolean.h>
#include <lsp-plug.in/tk/prop/simple/Integer.h>
#include <lsp-plug.in/tk/prop/simple/Float.h>
#include <lsp-plug.in/tk/prop/simple/String.h>

// Collections
#include <lsp-plug.in/tk/prop/collection/StringList.h>
#include <lsp-plug.in/tk/prop/collection/WidgetList.h>
#include <lsp-plug.in/tk/prop/collection/WidgetPtr.h>
#include <lsp-plug.in/tk/prop/collection/WidgetSet.h>

// Compound properties
#include <lsp-plug.in/tk/prop/multi/Color.h>
#include <lsp-plug.in/tk/prop/multi/Embedding.h>
#include <lsp-plug.in/tk/prop/multi/Font.h>
#include <lsp-plug.in/tk/prop/multi/Layout.h>
#include <lsp-plug.in/tk/prop/multi/Padding.h>
#include <lsp-plug.in/tk/prop/multi/Position.h>
#include <lsp-plug.in/tk/prop/multi/RangeFloat.h>
#include <lsp-plug.in/tk/prop/multi/Rectangle.h>
#include <lsp-plug.in/tk/prop/multi/Shortcut.h>
#include <lsp-plug.in/tk/prop/multi/Size.h>
#include <lsp-plug.in/tk/prop/multi/SizeConstraints.h>
#include <lsp-plug.in/tk/prop/multi/SizeRange.h>
#include <lsp-plug.in/tk/prop/multi/StepFloat.h>
#include <lsp-plug.in/tk/prop/multi/TextFitness.h>
#include <lsp-plug.in/tk/prop/multi/TextLayout.h>
#include <lsp-plug.in/tk/prop/multi/TextSelection.h>
#include <lsp-plug.in/tk/prop/multi/Vector2D.h>

// Specific properties
#include <lsp-plug.in/tk/prop/specific/GraphMeshData.h>
#include <lsp-plug.in/tk/prop/specific/GraphFrameData.h>

// Styles and schemas
#include <lsp-plug.in/tk/style/Style.h>
#include <lsp-plug.in/tk/style/Schema.h>

// System objects
#include <lsp-plug.in/tk/sys/Atoms.h>
#include <lsp-plug.in/tk/sys/Slot.h>
#include <lsp-plug.in/tk/sys/SlotSet.h>
#include <lsp-plug.in/tk/sys/Timer.h>
#include <lsp-plug.in/tk/sys/Display.h>

// Utilitary objects
#include <lsp-plug.in/tk/util/KeyboardHandler.h>
#include <lsp-plug.in/tk/util/TextCursor.h>
#include <lsp-plug.in/tk/util/TextDataSink.h>
#include <lsp-plug.in/tk/util/TextDataSource.h>

// Base widgets
#include <lsp-plug.in/tk/base/Widget.h>
#include <lsp-plug.in/tk/base/WidgetContainer.h>
#include <lsp-plug.in/tk/base/cast.h>

// Simple widgets
#include <lsp-plug.in/tk/widgets/simple/Button.h>
#include <lsp-plug.in/tk/widgets/simple/Edit.h>
#include <lsp-plug.in/tk/widgets/simple/Fader.h>
#include <lsp-plug.in/tk/widgets/simple/Hyperlink.h>
#include <lsp-plug.in/tk/widgets/simple/Indicator.h>
#include <lsp-plug.in/tk/widgets/simple/Knob.h>
#include <lsp-plug.in/tk/widgets/simple/Label.h>
#include <lsp-plug.in/tk/widgets/simple/Led.h>
#include <lsp-plug.in/tk/widgets/simple/ListBoxItem.h>
#include <lsp-plug.in/tk/widgets/simple/MenuItem.h>
#include <lsp-plug.in/tk/widgets/simple/ProgressBar.h>
#include <lsp-plug.in/tk/widgets/simple/ScrollBar.h>
#include <lsp-plug.in/tk/widgets/simple/Separator.h>
#include <lsp-plug.in/tk/widgets/simple/Switch.h>
#include <lsp-plug.in/tk/widgets/simple/Void.h>

// Container widgets
#include <lsp-plug.in/tk/widgets/containers/Window.h>
#include <lsp-plug.in/tk/widgets/containers/PopupWindow.h>
#include <lsp-plug.in/tk/widgets/containers/Align.h>
#include <lsp-plug.in/tk/widgets/containers/Box.h>
#include <lsp-plug.in/tk/widgets/containers/Grid.h>
#include <lsp-plug.in/tk/widgets/containers/Group.h>
#include <lsp-plug.in/tk/widgets/containers/ScrollArea.h>

// Compound widgets
#include <lsp-plug.in/tk/widgets/compound/Menu.h>
#include <lsp-plug.in/tk/widgets/compound/ListBox.h>
#include <lsp-plug.in/tk/widgets/compound/ComboBox.h>
#include <lsp-plug.in/tk/widgets/compound/ComboGroup.h>

// Specific widgets
#include <lsp-plug.in/tk/widgets/specific/FileButton.h>
#include <lsp-plug.in/tk/widgets/specific/Fraction.h>

// Graph widgets
#include <lsp-plug.in/tk/widgets/graph/GraphItem.h>
#include <lsp-plug.in/tk/widgets/graph/Graph.h>
#include <lsp-plug.in/tk/widgets/graph/GraphAxis.h>
#include <lsp-plug.in/tk/widgets/graph/GraphDot.h>
#include <lsp-plug.in/tk/widgets/graph/GraphFrameBuffer.h>
#include <lsp-plug.in/tk/widgets/graph/GraphMarker.h>
#include <lsp-plug.in/tk/widgets/graph/GraphMesh.h>
#include <lsp-plug.in/tk/widgets/graph/GraphOrigin.h>
#include <lsp-plug.in/tk/widgets/graph/GraphText.h>

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
