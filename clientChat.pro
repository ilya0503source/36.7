QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    loginform.cpp \
    main.cpp \
    mainwindow.cpp \
    registrationform.cpp \
    startscreen.cpp \
    users.cpp

HEADERS += \
    client.h \
    loginform.h \
    mainwindow.h \
    registrationform.h \
    startscreen.h \
    users.h

FORMS += \
    loginform.ui \
    mainwindow.ui \
    registrationform.ui \
    startscreen.ui \
    users.ui
TRANSLATIONS = \
    translations/app_ru.ts

win32::RC_FILE = file.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
