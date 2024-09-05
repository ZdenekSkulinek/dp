QT += qml quick opengl

CONFIG += c++11

SOURCES += main.cpp \
    capplication.cpp \
    cbargraph.cpp \
    cbargraphrenderer.cpp \
    cinterthreadqueue.cpp \
    copenclgoertzel.cpp \
    crecorder.cpp \
    csettings.cpp \
    cfileio.cpp \
    cstats.cpp

RESOURCES += qml.qrc \
    gpulang.qrc

LIBS += -lopenal -lalut -L/usr/local/cuda/lib64 -lOpenCL

INCLUDEPATH += /opt/AMDAPPSDK-2.9-1/include "/home/zdenek/NVIDIA GPU Computing SDK/OpenCL/common/inc"


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = .#/tmp/$${TARGET}/bin
else: unix:!android: target.path = .#/opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:QMAKE_RPATHDIR = .

unix:!macx {
    # suppress the default RPATH if you wish
    #QMAKE_LFLAGS_RPATH="."
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    #QMAKE_LFLAGS += "-rpath=."
}

HEADERS += \
    capplication.h \
    cbargraph.h \
    cbargraphrenderer.h \
    cinterthreadqueue.h \
    crecorder.h \
    csettings.h \
    cfileio.h \
    copenclgoertzel.h \
    copenclgoertzeldata.h \
    cstats.h

DISTFILES +=
