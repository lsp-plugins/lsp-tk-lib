/*
 * tk.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TK_H_
#define LSP_PLUG_IN_TK_TK_H_

#include <lsp-plug.in/tk/version.h>

#include <lsp-plug.in/tk/types.h>

#include <core/types.h>
#include <core/status.h>
#include <data/cvector.h>
#include <data/cstorage.h>
#include <core/LSPString.h>
#include <core/i18n/IDictionary.h>

#include <ui/ws/ws.h>


// System objects
#include <ui/tk/sys/LSPSlot.h>
#include <ui/tk/sys/LSPTimer.h>
#include <ui/tk/sys/LSPSlotSet.h>
#include <ui/tk/sys/LSPStyle.h>
#include <ui/tk/sys/LSPColor.h>
#include <ui/tk/sys/LSPLocalString.h>
#include <ui/tk/sys/LSPFloat.h>
#include <ui/tk/sys/LSPTheme.h>
#include <ui/tk/sys/LSPDisplay.h>

// Utilitary objects
#include <ui/tk/util/LSPSurface.h>
#include <ui/tk/util/LSPFont.h>
#include <ui/tk/util/LSPPadding.h>
#include <ui/tk/util/LSPTextLines.h>
#include <ui/tk/util/LSPItem.h>
#include <ui/tk/util/LSPItemList.h>
#include <ui/tk/util/LSPItemSelection.h>
#include <ui/tk/util/LSPTextSelection.h>
#include <ui/tk/util/LSPTextCursor.h>
#include <ui/tk/util/LSPFileMask.h>
#include <ui/tk/util/LSPKeyboardHandler.h>
#include <ui/tk/util/LSPSizeConstraints.h>
#include <ui/tk/util/LSPWindowActions.h>
#include <ui/tk/util/LSPTextDataSource.h>
#include <ui/tk/util/LSPTextDataSink.h>
#include <ui/tk/util/LSPUrlSink.h>
#include <ui/tk/util/LSPFileFilterItem.h>
#include <ui/tk/util/LSPFileFilter.h>
#include <ui/tk/util/LSPStyleTrigger.h>

// Widget basics
#include <ui/tk/basic/LSPWidget.h>
#include <ui/tk/basic/LSPComplexWidget.h>
#include <ui/tk/basic/LSPWidgetContainer.h>

// Basic widgets
#include <ui/tk/widgets/LSPVoid.h>
#include <ui/tk/widgets/LSPWindow.h>
#include <ui/tk/widgets/LSPBox.h>
#include <ui/tk/widgets/LSPGrid.h>
#include <ui/tk/widgets/LSPMenuItem.h>
#include <ui/tk/widgets/LSPMenu.h>
#include <ui/tk/widgets/LSPButton.h>
#include <ui/tk/widgets/LSPSwitch.h>
#include <ui/tk/widgets/LSPLabel.h>
#include <ui/tk/widgets/LSPHyperlink.h>
#include <ui/tk/widgets/LSPIndicator.h>
#include <ui/tk/widgets/LSPSeparator.h>
#include <ui/tk/widgets/LSPLed.h>
#include <ui/tk/widgets/LSPKnob.h>
#include <ui/tk/widgets/LSPMeter.h>
#include <ui/tk/widgets/LSPAlign.h>
#include <ui/tk/widgets/LSPGroup.h>
#include <ui/tk/widgets/LSPScrollBar.h>
#include <ui/tk/widgets/LSPFader.h>
#include <ui/tk/widgets/LSPListBox.h>
#include <ui/tk/widgets/LSPComboBox.h>
#include <ui/tk/widgets/LSPEdit.h>
#include <ui/tk/widgets/LSPGraph.h>
#include <ui/tk/widgets/LSPComboGroup.h>
#include <ui/tk/widgets/LSPProgressBar.h>
#include <ui/tk/widgets/LSPAudioSample.h>
#include <ui/tk/widgets/LSPScrollBox.h>

// Dialogs
#include <ui/tk/widgets/dialogs/LSPMessageBox.h>
#include <ui/tk/widgets/dialogs/LSPFileDialog.h>

// Advanced widgets
#include <ui/tk/widgets/LSPAudioFile.h>
#include <ui/tk/widgets/LSPMountStud.h>
#include <ui/tk/widgets/LSPSaveFile.h>
#include <ui/tk/widgets/LSPLoadFile.h>
#include <ui/tk/widgets/LSPFraction.h>

// Grapic widgets
#include <ui/tk/widgets/graph/LSPGraphItem.h>
#include <ui/tk/widgets/graph/LSPAxis.h>
#include <ui/tk/widgets/graph/LSPCenter.h>
#include <ui/tk/widgets/graph/LSPBasis.h>
#include <ui/tk/widgets/graph/LSPMarker.h>
#include <ui/tk/widgets/graph/LSPMesh.h>
#include <ui/tk/widgets/graph/LSPDot.h>
#include <ui/tk/widgets/graph/LSPText.h>
#include <ui/tk/widgets/graph/LSPFrameBuffer.h>

// 3D rendering
#include <ui/tk/widgets/LSPArea3D.h>
#include <ui/tk/widgets/3d/LSPObject3D.h>
#include <ui/tk/widgets/3d/LSPCapture3D.h>
#include <ui/tk/widgets/3d/LSPMesh3D.h>


#endif /* LSP_PLUG_IN_TK_TK_H_ */
