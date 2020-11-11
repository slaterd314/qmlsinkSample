TEMPLATE = app

QT += qml quick widgets

QT_CONFIG -= no-pkg-config
CONFIG += qmltypes
QML_IMPORT_NAME = com.thermo.myapp
QML_IMPORT_MAJOR_VERSION = 1

MKSPEC_PIECES=$$split(QMAKESPEC,"/")

contains(MKSPEC_PIECES,"linux-maelstrom-g++"){
CONFIG += device
SYSROOT=/opt/thermo/maelstrom-sysroot
}

# isEmpty(SYSROOT): SYSROOT=/opt/thermo/maelstrom-sysroot
QMAKE_CXXFLAGS += -std=c++17

!device {

message(not_device)

QT_CONFIG -= no-pkg-config
PKGCONFIG = \
     gstreamer-1.0 \
     gstreamer-video-1.0 \
     glib-2.0


INCLUDEPATH += /usr/include/gstreamer-1.0 /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include/ /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include

LIBS += -lglib-2.0 -lQt5GStreamer-1.0 -lgstvideo-1.0 -lgstreamer-1.0 -lgobject-2.0 -lgmodule-2.0
}

device {

message(device)
message(SYSROOT $$SYSROOT)
INCLUDEPATH += $$SYSROOT/usr/include/gstreamer-1.0 $$SYSROOT/usr/lib/arm-linux-gnueabihf/gstreamer-1.0/include/ $$SYSROOT/usr/include/glib-2.0 $$SYSROOT/usr/lib/arm-linux-gnueabihf/glib-2.0/include

LIBS += -L$$SYSROOT/usr/local/lib/arm-linux-gnueabihf/gstreamer-1.0 \
-L$$SYSROOT/usr/local/lib/arm-linux-gnueabihf \
-L$$SYSROOT/usr/lib/arm-linux-gnueabihf/glib-2.0

LIBS += -L$$SYSROOT/usr/local/lib/arm-linux-gnueabihf -lglib-2.0 -lQt5GStreamer-1.0 -lgstvideo-1.0 -lgstreamer-1.0 -lgobject-2.0 -lgmodule-2.0

}

CONFIG += debug

DEFINES += GST_USE_UNSTABLE_API

INCLUDEPATH += ../lib

SOURCES += main.cpp \
    MyApp.cpp

HEADERS += gstLibrary.h \
    MyApp.h

RESOURCES += qmlsink.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
