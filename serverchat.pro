QT       += core gui sql network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatDB.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp \
    serverChat.cpp

HEADERS += \
    chatDB.h \
    logger.h \
    mainwindow.h \
    message.h \
    serverChat.h \
    user.h

FORMS += \
    mainwindow.ui
win32::RC_FILE = file.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
