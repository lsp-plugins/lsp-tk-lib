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

// System objects
#include <lsp-plug.in/tk/sys/LSPSlot.h>
#include <lsp-plug.in/tk/sys/LSPTimer.h>
#include <lsp-plug.in/tk/sys/LSPSlotSet.h>
#include <lsp-plug.in/tk/sys/LSPStyle.h>
#include <lsp-plug.in/tk/sys/LSPColor.h>
#include <lsp-plug.in/tk/sys/LSPLocalString.h>
#include <lsp-plug.in/tk/sys/LSPFloat.h>
#include <lsp-plug.in/tk/sys/LSPTheme.h>
#include <lsp-plug.in/tk/sys/LSPDisplay.h>

// Utilitary objects
#include <lsp-plug.in/tk/util/LSPSurface.h>
#include <lsp-plug.in/tk/util/LSPFont.h>
#include <lsp-plug.in/tk/util/LSPPadding.h>
#include <lsp-plug.in/tk/util/LSPTextLines.h>
#include <lsp-plug.in/tk/util/LSPItem.h>
#include <lsp-plug.in/tk/util/LSPItemList.h>
#include <lsp-plug.in/tk/util/LSPItemSelection.h>
#include <lsp-plug.in/tk/util/LSPTextSelection.h>
#include <lsp-plug.in/tk/util/LSPTextCursor.h>
#include <lsp-plug.in/tk/util/LSPFileMask.h>
#include <lsp-plug.in/tk/util/LSPKeyboardHandler.h>
#include <lsp-plug.in/tk/util/LSPSizeConstraints.h>
#include <lsp-plug.in/tk/util/LSPWindowActions.h>
#include <lsp-plug.in/tk/util/LSPTextDataSource.h>
#include <lsp-plug.in/tk/util/LSPTextDataSink.h>
#include <lsp-plug.in/tk/util/LSPUrlSink.h>
#include <lsp-plug.in/tk/util/LSPFileFilterItem.h>
#include <lsp-plug.in/tk/util/LSPFileFilter.h>
#include <lsp-plug.in/tk/util/LSPStyleTrigger.h>

// Widget basics
#include <lsp-plug.in/tk/basic/LSPWidget.h>
#include <lsp-plug.in/tk/basic/LSPComplexWidget.h>
#include <lsp-plug.in/tk/basic/LSPWidgetContainer.h>

// Basic widgets
#include <lsp-plug.in/tk/widgets/LSPVoid.h>
#include <lsp-plug.in/tk/widgets/LSPWindow.h>
#include <lsp-plug.in/tk/widgets/LSPBox.h>
#include <lsp-plug.in/tk/widgets/LSPGrid.h>
#include <lsp-plug.in/tk/widgets/LSPMenuItem.h>
#include <lsp-plug.in/tk/widgets/LSPMenu.h>
#include <lsp-plug.in/tk/widgets/LSPButton.h>
#include <lsp-plug.in/tk/widgets/LSPSwitch.h>
#include <lsp-plug.in/tk/widgets/LSPLabel.h>
#include <lsp-plug.in/tk/widgets/LSPHyperlink.h>
#include <lsp-plug.in/tk/widgets/LSPIndicator.h>
#include <lsp-plug.in/tk/widgets/LSPSeparator.h>
#include <lsp-plug.in/tk/widgets/LSPLed.h>
#include <lsp-plug.in/tk/widgets/LSPKnob.h>
#include <lsp-plug.in/tk/widgets/LSPMeter.h>
#include <lsp-plug.in/tk/widgets/LSPAlign.h>
#include <lsp-plug.in/tk/widgets/LSPGroup.h>
#include <lsp-plug.in/tk/widgets/LSPScrollBar.h>
#include <lsp-plug.in/tk/widgets/LSPFader.h>
#include <lsp-plug.in/tk/widgets/LSPListBox.h>
#include <lsp-plug.in/tk/widgets/LSPComboBox.h>
#include <lsp-plug.in/tk/widgets/LSPEdit.h>
#include <lsp-plug.in/tk/widgets/LSPGraph.h>
#include <lsp-plug.in/tk/widgets/LSPComboGroup.h>
#include <lsp-plug.in/tk/widgets/LSPProgressBar.h>
#include <lsp-plug.in/tk/widgets/LSPAudioSample.h>
#include <lsp-plug.in/tk/widgets/LSPScrollBox.h>

// Dialogs
#include <lsp-plug.in/tk/widgets/dialogs/LSPMessageBox.h>
#include <lsp-plug.in/tk/widgets/dialogs/LSPFileDialog.h>

// Advanced widgets
#include <lsp-plug.in/tk/widgets/LSPAudioFile.h>
#include <lsp-plug.in/tk/widgets/LSPMountStud.h>
#include <lsp-plug.in/tk/widgets/LSPSaveFile.h>
#include <lsp-plug.in/tk/widgets/LSPLoadFile.h>
#include <lsp-plug.in/tk/widgets/LSPFraction.h>

// Grapic widgets
#include <lsp-plug.in/tk/widgets/graph/LSPGraphItem.h>
#include <lsp-plug.in/tk/widgets/graph/LSPAxis.h>
#include <lsp-plug.in/tk/widgets/graph/LSPCenter.h>
#include <lsp-plug.in/tk/widgets/graph/LSPBasis.h>
#include <lsp-plug.in/tk/widgets/graph/LSPMarker.h>
#include <lsp-plug.in/tk/widgets/graph/LSPMesh.h>
#include <lsp-plug.in/tk/widgets/graph/LSPDot.h>
#include <lsp-plug.in/tk/widgets/graph/LSPText.h>
#include <lsp-plug.in/tk/widgets/graph/LSPFrameBuffer.h>

// 3D rendering
#include <lsp-plug.in/tk/widgets/LSPArea3D.h>
#include <lsp-plug.in/tk/widgets/3d/LSPObject3D.h>
#include <lsp-plug.in/tk/widgets/3d/LSPCapture3D.h>
#include <lsp-plug.in/tk/widgets/3d/LSPMesh3D.h>


#endif /* LSP_PLUG_IN_TK_TK_H_ */
