APPNAME = CQTkWidgetTest

include($$(MAKE_DIR)/qt_app.mk)

SOURCES += \
CQTkWidgetTest.cpp \
CQTkWidget.cpp \
\
CQTclUtil.cpp \
CTclUtil.cpp

HEADERS += \
CQTkWidgetTest.h \
CQTkWidget.h \
\
CQTclUtil.h \
CTclUtil.h

CONFIG += debug

INCLUDEPATH += \
$(INC_DIR)/CQTkWidget \
$(INC_DIR)/CStrParse \
$(INC_DIR)/CStrUtil \
/work/colinw/packages/tcl_test/include

PRE_TARGETDEPS = \
$(LIB_DIR)/libCQTkWidget.a

unix:LIBS += \
-lCStrParse \
-lCStrUtil \
-lCPrintF \
-L/work/colinw/packages/tcl_test/lib \
-ltcl9tk9.0 -ltkstub -ltcl9.0 -ltclstub \
-L/work/colinw/packages/libxft/lib \
"-Wl,-rpath,/work/colinw/packages/libxft/lib" \
-lXft-dbg \
-lfontconfig -lfreetype -lfontconfig \
-lX11 -lXss -lXext \
-lz
