LIBNAME = CQTkWidget

CAST_WARN =

include($$(MAKE_DIR)/qt_lib.mk)

HEADERS += \
CQTkWidget.h \

INCLUDEPATH += \
$(INC_DIR)/CQTclUtil \
$(INC_DIR)/CTclUtil \
/usr/include/tcl \

TCL_DIR = \
/usr/include/tcl \

SOURCES += \
CQTkWidget.cpp \
