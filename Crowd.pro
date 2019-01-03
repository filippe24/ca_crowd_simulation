
TEMPLATE = app
TARGET = Crowd

QT += gui opengl

CONFIG += c++11

INCLUDEPATH += .

# Input
HEADERS += glwidget.h mainwindow.h \
    trianglemesh.h \
    plyreader.h \
    geometry.h \
    person.h \
    prsanimation.h \
    groundgrid.h \
#    cal3Ddemo/demo.h \
    cal3Ddemo/global.h \
#    cal3Ddemo/menu.h \
    cal3Ddemo/tga.h \
    cal3Ddemo/modelloader.h
#    cal3Ddemo/tick.h
FORMS += mainwindow.ui
SOURCES += glwidget.cpp main.cpp mainwindow.cpp \
    trianglemesh.cpp \
    plyreader.cpp \
    geometry.cpp \
    person.cpp \
    prsanimation.cpp \
    groundgrid.cpp \
#    cal3Ddemo/demo.cpp \
#    cal3Ddemo/main.cpp \
#    cal3Ddemo/menu.cpp \
    cal3Ddemo/tga.cpp \
    cal3Ddemo/modelloader.cpp
#    cal3Ddemo/tick.cpp
DISTFILES += \
#    cal3Dlibrary/cal3d.dsp \
#    cal3Dlibrary/SConscript \
    shaders/simpleshader.vert \
    shaders/simpleshader.frag \
    shaders/particle.frag \
    shaders/particle.vert \
    shaders/basic.frag \
    shaders/basic.vert \
    shaders/person.frag \
    shaders/person.vert

RESOURCES += \
    resources.qrc

#libraries
#unix:!macx: LIBS += -lcal3d

#unix:!macx: LIBS += -L$$PWD/cal3Dlib/lib/ -lcal3d

#INCLUDEPATH += $$PWD/cal3Dlib/include
#DEPENDPATH += $$PWD/cal3Dlib/include



unix|win32: LIBS += -lcal3d
