TEMPLATE = lib

TARGET = CTkApp

DEPENDPATH += .

QT += widgets svg

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
CTkApp.cpp \
CTkAppCommands.cpp \
CTkAppImage.cpp \
CTkAppFont.cpp \
CTkAppOptData.cpp \
CTkAppWidget.cpp \
\
CTkAppGridLayout.cpp \
CTkAppPackLayout.cpp \
CTkAppPlaceLayout.cpp \
CTkAppLayout.cpp \
CTkAppLayoutWidget.cpp \
\
CTkAppUtil.cpp \
\
CTclApp.cpp \
CTclAppCommand.cpp \
\
CTkWidget.cpp \
CTkAppDebug.cpp \
\
CQLabelImage.cpp \
CQButtonImage.cpp \
CQTextWidget.cpp \
CQSpinList.cpp \
CQSlider.cpp \
CSVGUtil.cpp \
CQRotatedText.cpp \
CQOptEdit.cpp \
CXBMImage.cpp \

HEADERS += \
../include/CTkApp.h \
../include/CTkAppCommands.h \
../include/CTkAppEventData.h \
../include/CTkAppImage.h \
../include/CTkAppFont.h \
../include/CTkAppOptData.h \
../include/CTkAppOptionValue.h \
../include/CTkAppWidget.h \
\
../include/CTkAppGridLayout.h \
../include/CTkAppPackLayout.h \
../include/CTkAppPlaceLayout.h \
../include/CTkAppLayout.h \
../include/CTkAppLayoutWidget.h \
\
../include/CTkAppUtil.h \
\
../include/CTclApp.h \
../include/CTclAppCommand.h \
\
../include/CTkWidget.h \
../include/CTkAppDebug.h \
\
../include/CQLabelImage.h \
../include/CQButtonImage.h \
../include/CQTextWidget.h \
../include/CQSpinList.h \
../include/CQSlider.h \
../include/CSVGUtil.h \
../include/CQRotatedText.h \
\
CQOptEdit.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../include \
../../../CQUtil/include \
../../../CTclApp/include \
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
