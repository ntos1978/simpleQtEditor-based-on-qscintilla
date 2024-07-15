QT       += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

CONFIG += c++17 qscintilla2

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.h

FORMS += \
    MainWindow.ui

#INCLUDEPATH += /usr/include/x86_64-linux-gnu/qt6/Qsci/ /usr/include/x86_64-linux-gnu/qt6/
# or
#/home/nam/Qt/6.5.3/gcc_64/include/Qsci

# Note that we have to compile qscintilla for this qt creator from qt.
# The library is installed to the folder where Qt is located.

#LIBS += -L/home/nam/Qt/6.5.3/gcc_64/lib/ -lqscintilla2_qt6

# But now I have commented them out, and the code still works while it wasn't before without the include and libs!!

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

