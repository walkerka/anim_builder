#-------------------------------------------------
#
# Project created by QtCreator 2014-06-03T22:10:10
#
#-------------------------------------------------

QT       += core gui xml opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AnimBuilder
TEMPLATE = app

DEFINES += GLEW_STATIC

SOURCES += main.cpp\
        mainwindow.cpp \
    rasterimageeditor.cpp \
    timeline.cpp \
    rasterlayer.cpp \
    timelinenavbar.cpp \
    wavfile.cpp \
    soundlayer.cpp \
    animationfile.cpp \
    command.cpp \
    brushpropertywindow.cpp \
    cachedimage.cpp \
    brushtool.cpp \
    layer.cpp \
    strokepoint.cpp \
    timelinewindow.cpp \
    pantool.cpp \
    zoomtool.cpp \
    rotatetool.cpp \
    ringcolorpicker.cpp \
    colorpicker.cpp \
    filltool.cpp \
    regiontool.cpp \
    colortool.cpp \
    newprojectdialog.cpp \
    splinetool.cpp \
    tracelayer.cpp \
    tracetool.cpp \
    openglwindow.cpp \
    renderwindow.cpp \
    openglrenderer.cpp \
    glew.c

HEADERS  += mainwindow.h \
    rasterimageeditor.h \
    timeline.h \
    rasterlayer.h \
    timelinenavbar.h \
    wavfile.h \
    soundlayer.h \
    animationfile.h \
    command.h \
    brushpropertywindow.h \
    canvastool.h \
    cachedimage.h \
    brushtool.h \
    layer.h \
    strokepoint.h \
    timelinewindow.h \
    pantool.h \
    zoomtool.h \
    rotatetool.h \
    ringcolorpicker.h \
    filltool.h \
    regiontool.h \
    colorpicker.h \
    colortool.h \
    newprojectdialog.h \
    splinetool.h \
    strokepoint.h \
    tracelayer.h \
    tracetool.h \
    openglwindow.h \
    renderwindow.h \
    openglrenderer.h

FORMS    += mainwindow.ui \
    brushpropertywindow.ui \
    timelinewindow.ui \
    colorpicker.ui \
    newprojectdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES +=
