#-------------------------------------------------
#
# Project created by QtCreator 2020-02-26T10:46:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imx6ull_flash_tool
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


SOURCES += \
        main.cpp \
        widget.cpp \
    CDetachDeviceThread.cpp

HEADERS += \
        widget.h \
    CDetachDeviceThread.h

FORMS += \
        widget.ui

RC_ICONS = cpu.ico

RESOURCES += \
    image.qrc


INCLUDEPATH += 'E:/mfgtools/libusb/libusb'
#INCLUDEPATH += 'E:/Project Development/imx6ull/develop_tools/code/mfgtools/libusb/libusb'
#DEPENDPATH += 'E:/Project Development/imx6ull/develop_tools/code/mfgtools/libusb/libusb'

#LIBS += -L'E:/Project Development/imx6ull/develop_tools/code/mfgtools/libusb/x64/Debug/dll/' -llibusb-1.0
LIBS += -L'E:/mfgtools/libusb/Win32/Debug/dll' -llibusb-1.0

