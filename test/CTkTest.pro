TEMPLATE = app

QT += widgets

TARGET = CTkTest

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++14

MOC_DIR = .moc

CONFIG += debug
CONFIG += silent

# Input
SOURCES += \
CTkTest.cpp \

HEADERS += \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CTk/include \
../../CTcl/include \
../../CQApp/include \
../../CQUtil/include \
../../CFont/include \
../../CImageLib/include \
../../CReadLine/include \
../../CFileUtil/include \
../../CFile/include \
../../CArgs/include \
../../COS/include \
../../CUtil/include \
../../CMath/include \
../../CStrUtil/include \
../../CRegExp/include \
../../CRGBName/include \
../../CCSS/include \
../../CXML/include \

unix:LIBS += \
-L$$LIB_DIR \
-L../../CTk/lib \
-L../../CTcl/lib \
-L../../CQUtil/lib \
-L../../CCommand/lib \
-L../../CArgs/lib \
-L../../CReadLine/lib \
-L../../CFont/lib \
-L../../CUtil/lib \
-L../../CImageLib/lib \
-L../../CConfig/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CTimer/lib \
-L../../CMath/lib \
-L../../CGlob/lib \
-L../../COS/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../CRGBName/lib \
-lCTk -lCTcl -lCQUtil -lCReadLine -lCConfig -lCUtil -lCCommand \
-lCArgs -lCImageLib -lCFont -lCTimer -lCMath -lCGlob \
-lCRGBName -lCFileUtil -lCFile -lCOS -lCStrUtil -lCRegExp \
-lpng -ljpeg -ltre -lreadline -lncurses
