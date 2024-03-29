TEMPLATE = app

QT += widgets svg x11extras

TARGET = CTkTest

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

CONFIG += debug
CONFIG += silent

# Input
SOURCES += \
CTkAppTest.cpp \

HEADERS += \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

PRE_TARGETDEPS = \
$$LIB_DIR/libCTkApp.a \

INCLUDEPATH += \
. \
../include \
../../../CQApp/include \
../../../CQUtil/include \
../../../CFont/include \
../../../CImageLib/include \
../../../CReadLine/include \
../../../CFileUtil/include \
../../../CFile/include \
../../../CArgs/include \
../../../COS/include \
../../../CUtil/include \
../../../CMath/include \
../../../CStrUtil/include \
../../../CRegExp/include \
../../../CRGBName/include \
../../../CCSS/include \
../../../CXML/include \
/usr/include/tcl \

unix:LIBS += \
-L$$LIB_DIR \
-L../../../CQPropertyView/lib \
-L../../../CQUtil/lib \
-L../../../CCommand/lib \
-L../../../CArgs/lib \
-L../../../CReadLine/lib \
-L../../../CFont/lib \
-L../../../CUtil/lib \
-L../../../CImageLib/lib \
-L../../../CConfig/lib \
-L../../../CMath/lib \
-L../../../CFileUtil/lib \
-L../../../CFile/lib \
-L../../../CTimer/lib \
-L../../../CGlob/lib \
-L../../../COS/lib \
-L../../../CStrUtil/lib \
-L../../../CRegExp/lib \
-L../../../CRGBName/lib \
-lCTkApp -lCQPropertyView -lCQUtil -lCReadLine -lCConfig -lCUtil -lCCommand \
-lCArgs -lCImageLib -lCFont -lCTimer -lCGlob -lCMath \
-lCRGBName -lCFileUtil -lCFile -lCOS -lCStrUtil -lCRegExp \
-lpng -ljpeg -ltre -lreadline -lncurses \
-ltk -ltcl -lX11
