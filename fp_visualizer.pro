#-------------------------------------------------
#
# Project created by QtCreator 2018-08-23T01:22:19
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = fp_visualizer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        fp.cpp \
    fpga.cpp \
    csv_data_manipulator.cpp \
    model_zynq.cpp \
    model_virtex_5.cpp \
    model_virtex.cpp\
    model_pynq.cpp \
    fine_grained.cpp

LIBS += \
       -lgurobi_g++5.2\
       -lgurobi_c++\
       -lgurobi70

HEADERS += \
        fp.h \
        fpga.h \
    ../../engine/include/gurobi_c++.h \
    csv_data_manipulator.hpp \
    zynq_wrapper.hpp \
    paint.h \
    zynq_model.h \
    fine_grained.h \
    generate_xdc.h

FORMS += \
        fp.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
