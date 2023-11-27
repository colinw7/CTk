TEMPLATE = lib

TARGET = CTkApp

DEPENDPATH += .

QT += widgets svg x11extras

CONFIG += staticlib
CONFIG += debug

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQUTIL_PEN \
-DCQUTIL_BRUSH \
-DCQUTIL_FONT \
-DCQUTIL_IMAGE \
-DCBRUSH_IMAGE \

MOC_DIR = .moc

# Input
SOURCES += \
CTkAppBitmaps.cpp \
CTkAppButton.cpp \
CTkAppButtonWidget.cpp \
CTkAppCanvas.cpp \
CTkAppCanvasShape.cpp \
CTkAppCanvasWidget.cpp \
CTkAppCheckButton.cpp \
CTkAppComboBox.cpp \
CTkAppCommands.cpp \
CTkApp.cpp \
CTkAppCursor.cpp \
CTkAppDebug.cpp \
CTkAppEntry.cpp \
CTkAppFont.cpp \
CTkAppFrame.cpp \
CTkAppGridLayout.cpp \
CTkAppImageCommand.cpp \
CTkAppImage.cpp \
CTkAppLabel.cpp \
CTkAppLabelFrame.cpp \
CTkAppLayout.cpp \
CTkAppLayoutWidget.cpp \
CTkAppListBox.cpp \
CTkAppMenuButton.cpp \
CTkAppMenu.cpp \
CTkAppMessage.cpp \
CTkAppNoteBook.cpp \
CTkAppOptData.cpp \
CTkAppPackLayout.cpp \
CTkAppPanedWindow.cpp \
CTkAppPlaceLayout.cpp \
CTkAppRadioButton.cpp \
CTkAppRoot.cpp \
CTkAppScale.cpp \
CTkAppScrollBar.cpp \
CTkAppSpinBox.cpp \
CTkAppText.cpp \
CTkAppTopLevel.cpp \
CTkAppTreeView.cpp \
CTkAppUtil.cpp \
CTkAppWidget.cpp \
CTkAppX11.cpp \
\
CTclApp.cpp \
CTclAppCommand.cpp \
\
CTkWidget.cpp \
\
CQLabelImage.cpp \
CQButtonImage.cpp \
CQTextWidget.cpp \
CQSpinList.cpp \
CSVGUtil.cpp \
CQRotatedText.cpp \
CQOptEdit.cpp \
CQSlider.cpp \
\
CXBMImage.cpp \
CGIFImage.cpp \

HEADERS += \
../include/CTkAppBitmaps.h \
../include/CTkAppButton.h \
../include/CTkAppButtonWidget.h \
../include/CTkAppCanvas.h \
../include/CTkAppCanvasShape.h \
../include/CTkAppCanvasWidget.h \
../include/CTkAppCheckButton.h \
../include/CTkAppComboBox.h \
../include/CTkAppCommand.h \
../include/CTkAppCommands.h \
../include/CTkAppCursor.h \
../include/CTkAppDebug.h \
../include/CTkAppEntry.h \
../include/CTkAppEventData.h \
../include/CTkAppFont.h \
../include/CTkAppFrame.h \
../include/CTkAppGridLayout.h \
../include/CTkApp.h \
../include/CTkAppImageCommand.h \
../include/CTkAppImage.h \
../include/CTkAppLabelFrame.h \
../include/CTkAppLabel.h \
../include/CTkAppLayout.h \
../include/CTkAppLayoutWidget.h \
../include/CTkAppListBox.h \
../include/CTkAppMenuButton.h \
../include/CTkAppMenu.h \
../include/CTkAppMessage.h \
../include/CTkAppNoteBook.h \
../include/CTkAppOptData.h \
../include/CTkAppOptionValue.h \
../include/CTkAppPackLayout.h \
../include/CTkAppPanedWindow.h \
../include/CTkAppPlaceLayout.h \
../include/CTkAppPoint.h \
../include/CTkAppRadioButton.h \
../include/CTkAppRoot.h \
../include/CTkAppScale.h \
../include/CTkAppScrollBar.h \
../include/CTkAppSpinBox.h \
../include/CTkAppSpinBoxWidget.h \
../include/CTkAppText.h \
../include/CTkAppTextWidget.h \
../include/CTkAppTopLevel.h \
../include/CTkAppTreeView.h \
../include/CTkAppTypes.h \
../include/CTkAppUtil.h \
../include/CTkAppWidget.h \
../include/CTkAppX11.h \
\
../include/CTclApp.h \
../include/CTclAppCommand.h \
\
../include/CTkWidget.h \
\
../include/CQLabelImage.h \
../include/CQButtonImage.h \
../include/CQTextWidget.h \
../include/CQSpinList.h \
../include/CSVGUtil.h \
../include/CQRotatedText.h \
../include/CQSlider.h \
\
CQOptEdit.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../../CQUtil/include \
../../../CTclApp/include \
../../../CQPropertyView/include \
../../../CXLib/include \
../../../CReadLine/include \
../../../CImageLib/include \
../../../CRGBName/include \
../../../CFont/include \
../../../CFile/include \
../../../CMath/include \
../../../CStrUtil/include \
../../../CGlob/include \
../../../CUtil/include \
../../../COS/include \
/usr/include/tcl \
