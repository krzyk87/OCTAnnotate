#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T13:56:39
#
#-------------------------------------------------

QT       += core gui xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
greaterThan(QT_MAJOR_VERSION, 6): QT += core5compat

TARGET = OCTAnnotate
TEMPLATE = app


SOURCES += main.cpp\
    functions.cpp \
    layer.cpp \
        octannotate.cpp \
    octdata.cpp \
    patientdata.cpp \
    qcustomplot.cpp \
    calculate.cpp \
    readwritedata.cpp \
    scan.cpp \
    settingsdialog.cpp

HEADERS  += octannotate.h \
    enums.h \
    functions.h \
    layer.h \
    octdata.h \
    patientdata.h \
    qcustomplot.h \
    calculate.h \
    readwritedata.h \
    scan.h \
    settingsdialog.h

FORMS    += octannotate.ui \
    settingsdialog.ui

#INCLUDEPATH += C:\\opencv\\mybuild\\install\\include

#LIBS += -LC:\\opencv\\mybuild\\install\\x86\\mingw\\lib \
#    -lopencv_core249.dll \
#    -lopencv_highgui249.dll \
#    -lopencv_imgproc249.dll \
#    -lopencv_features2d249.dll \
#    -lopencv_calib3d249.dll \
#    -lopencv_core249d.dll \
#    -lopencv_highgui249d.dll \
#    -lopencv_imgproc249d.dll \
#    -lopencv_features2d249d.dll \
#    -lopencv_calib3d249d.dll

OTHER_FILES += \
    circ_profile_legend.png
    logoPUEPS.png
    logoPUT.png
