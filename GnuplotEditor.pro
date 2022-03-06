QT       += core gui \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../LabQHome/layoutparts.cpp \
    ../LabQHome/utility.cpp \
    browserwidget.cpp \
    consolewidget.cpp \
    editorsettingwidget.cpp \
    editorsyntaxhighlighter.cpp \
    filetree.cpp \
    gnuplot.cpp \
    gnuplotcpl.cpp \
    gnuploteditor.cpp \
    main_ge.cpp \
    menubar.cpp \
    tablewidget.cpp \
    texteditor.cpp \
    windowmenubar.cpp

HEADERS += \
    ../LabQHome/iofile.h \
    ../LabQHome/layoutparts.h \
    ../LabQHome/utility.h \
    browserwidget.h \
    consolewidget.h \
    editorsettingwidget.h \
    editorsyntaxhighlighter.h \
    filetree.h \
    gnuplot.h \
    gnuplotcpl.h \
    gnuploteditor.h \
    menubar.h \
    tablewidget.h \
    texteditor.h \
    windowmenubar.h

INCLUDEPATH += E:/boost_1_76_0/boost_1_76_0 \
               E:/repos/qt_project/LabQ/LabQHome \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
