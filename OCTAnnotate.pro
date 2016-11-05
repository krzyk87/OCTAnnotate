#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T13:56:39
#
#-------------------------------------------------

QT       += core gui xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = OCTAnnotate
TEMPLATE = app


SOURCES += main.cpp\
        octannotate.cpp \
    patientdata.cpp \
    newpatientdialog.cpp \
    qcustomplot.cpp \
    autoasmanualdialog.cpp \
    settingsdialog.cpp \
    calculate.cpp \
    readwritedata.cpp \
    computeerrorsettingsdialog.cpp \
    dbmanager.cpp

HEADERS  += octannotate.h \
    patientdata.h \
    newpatientdialog.h \
    qcustomplot.h \
    autoasmanualdialog.h \
    settingsdialog.h \
    calculate.h \
    readwritedata.h \
    computeerrorsettingsdialog.h \
    dbmanager.h

FORMS    += octannotate.ui \
    newpatientdialog.ui \
    settingsdialog.ui \
    computeerrorsettingsdialog.ui

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
