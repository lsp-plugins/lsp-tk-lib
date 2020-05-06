/*
 * tk.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_TK_TK_H_
#define LSP_PLUG_IN_TK_TK_H_

#include <lsp-plug.in/old-tk/basic/LSPComplexWidget.h>
#include <lsp-plug.in/old-tk/basic/LSPWidget.h>
#include <lsp-plug.in/old-tk/basic/LSPWidgetContainer.h>
#include <lsp-plug.in/old-tk/sys/LSPColor.h>
#include <lsp-plug.in/old-tk/sys/LSPDisplay.h>
#include <lsp-plug.in/old-tk/sys/LSPFloat.h>
#include <lsp-plug.in/old-tk/sys/LSPLocalString.h>
#include <lsp-plug.in/old-tk/sys/LSPSlot.h>
#include <lsp-plug.in/old-tk/sys/LSPSlotSet.h>
#include <lsp-plug.in/old-tk/sys/LSPStyle.h>
#include <lsp-plug.in/old-tk/sys/LSPTheme.h>
#include <lsp-plug.in/old-tk/sys/LSPTimer.h>
#include <lsp-plug.in/old-tk/util/LSPFileFilter.h>
#include <lsp-plug.in/old-tk/util/LSPFileFilterItem.h>
#include <lsp-plug.in/old-tk/util/LSPFileMask.h>
#include <lsp-plug.in/old-tk/util/LSPFont.h>
#include <lsp-plug.in/old-tk/util/LSPItem.h>
#include <lsp-plug.in/old-tk/util/LSPItemList.h>
#include <lsp-plug.in/old-tk/util/LSPItemSelection.h>
#include <lsp-plug.in/old-tk/util/LSPKeyboardHandler.h>
#include <lsp-plug.in/old-tk/util/LSPPadding.h>
#include <lsp-plug.in/old-tk/util/LSPSizeConstraints.h>
#include <lsp-plug.in/old-tk/util/LSPStyleTrigger.h>
#include <lsp-plug.in/old-tk/util/LSPSurface.h>
#include <lsp-plug.in/old-tk/util/LSPTextCursor.h>
#include <lsp-plug.in/old-tk/util/LSPTextDataSink.h>
#include <lsp-plug.in/old-tk/util/LSPTextDataSource.h>
#include <lsp-plug.in/old-tk/util/LSPTextLines.h>
#include <lsp-plug.in/old-tk/util/LSPTextSelection.h>
#include <lsp-plug.in/old-tk/util/LSPUrlSink.h>
#include <lsp-plug.in/old-tk/util/LSPWindowActions.h>
#include <lsp-plug.in/old-tk/widgets/3d/LSPCapture3D.h>
#include <lsp-plug.in/old-tk/widgets/3d/LSPMesh3D.h>
#include <lsp-plug.in/old-tk/widgets/3d/LSPObject3D.h>
#include <lsp-plug.in/old-tk/widgets/dialogs/LSPFileDialog.h>
#include <lsp-plug.in/old-tk/widgets/dialogs/LSPMessageBox.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPAxis.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPBasis.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPCenter.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPDot.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPFrameBuffer.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPGraphItem.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPMarker.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPMesh.h>
#include <lsp-plug.in/old-tk/widgets/graph/LSPText.h>
#include <lsp-plug.in/old-tk/widgets/LSPAlign.h>
#include <lsp-plug.in/old-tk/widgets/LSPArea3D.h>
#include <lsp-plug.in/old-tk/widgets/LSPAudioFile.h>
#include <lsp-plug.in/old-tk/widgets/LSPAudioSample.h>
#include <lsp-plug.in/old-tk/widgets/LSPBox.h>
#include <lsp-plug.in/old-tk/widgets/LSPButton.h>
#include <lsp-plug.in/old-tk/widgets/LSPComboBox.h>
#include <lsp-plug.in/old-tk/widgets/LSPComboGroup.h>
#include <lsp-plug.in/old-tk/widgets/LSPEdit.h>
#include <lsp-plug.in/old-tk/widgets/LSPFader.h>
#include <lsp-plug.in/old-tk/widgets/LSPFraction.h>
#include <lsp-plug.in/old-tk/widgets/LSPGraph.h>
#include <lsp-plug.in/old-tk/widgets/LSPGrid.h>
#include <lsp-plug.in/old-tk/widgets/LSPGroup.h>
#include <lsp-plug.in/old-tk/widgets/LSPHyperlink.h>
#include <lsp-plug.in/old-tk/widgets/LSPIndicator.h>
#include <lsp-plug.in/old-tk/widgets/LSPKnob.h>
#include <lsp-plug.in/old-tk/widgets/LSPLabel.h>
#include <lsp-plug.in/old-tk/widgets/LSPLed.h>
#include <lsp-plug.in/old-tk/widgets/LSPListBox.h>
#include <lsp-plug.in/old-tk/widgets/LSPLoadFile.h>
#include <lsp-plug.in/old-tk/widgets/LSPMenu.h>
#include <lsp-plug.in/old-tk/widgets/LSPMenuItem.h>
#include <lsp-plug.in/old-tk/widgets/LSPMeter.h>
#include <lsp-plug.in/old-tk/widgets/LSPMountStud.h>
#include <lsp-plug.in/old-tk/widgets/LSPProgressBar.h>
#include <lsp-plug.in/old-tk/widgets/LSPSaveFile.h>
#include <lsp-plug.in/old-tk/widgets/LSPScrollBar.h>
#include <lsp-plug.in/old-tk/widgets/LSPScrollBox.h>
#include <lsp-plug.in/old-tk/widgets/LSPSeparator.h>
#include <lsp-plug.in/old-tk/widgets/LSPSwitch.h>
#include <lsp-plug.in/old-tk/widgets/LSPVoid.h>
#include <lsp-plug.in/old-tk/widgets/LSPWindow.h>
#include <lsp-plug.in/tk-old/types.h>
#include <lsp-plug.in/tk-old/version.h>


#endif /* LSP_PLUG_IN_TK_TK_H_ */
