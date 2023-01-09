QT       += core gui \
            xml \
            pdf \
            pdfwidgets \
            svgwidgets \
            webenginewidgets \
            core5compat \ #QTextCodec

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    #../LabQHome/layoutparts.cpp \
    ../LabQHome/tablewidget.cpp \
    #../LabQHome/utility.cpp \

HEADERS += \
    ../LabQHome/iofile.h \
    #../LabQHome/layoutparts.h \
    ../LabQHome/tablewidget.h \
    #../LabQHome/utility.h \
    plugin/pluginchecker/checker.h

INCLUDEPATH += E:/boost_1_76_0/boost_1_76_0 \        #boostLib for xml parser
               E:/repos/qt_project/LabQ/LabQHome \   #utility tool for Qt project


LIBS += "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.19041.0\\um\\x64\\kernel32.lib" \  #WinAPI
        "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.19041.0\\um\\x64\\user32.lib"      #WinAPI

include(src/src.pri)
include(plugin/plugin.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icon.qrc
