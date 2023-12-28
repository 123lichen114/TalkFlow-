QT       += core gui
QT += network
QT += core
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    changeavatar.cpp \
    changename.cpp \
    home.cpp \
    login.cpp \
    main.cpp \
    registerdialog.cpp \
    userinfo.cpp \
    addfriends.cpp\
    chat_1v1.cpp\
    client.cpp\
    server.cpp\
    grouphome.cpp\
    receivefiledialog.cpp\
    sendfiledialog.cpp\

HEADERS += \
    changeavatar.h \
    changename.h \
    home.h \
    login.h \
    registerdialog.h \
    userinfo.h\
    addfriends.h\
    chat_1v1.h\
    signal.h\
    client.h\
    server.h\
    grouphome.h\
    receivefiledialog.h\
    sendfiledialog.h

FORMS += \
    changeavatar.ui \
    changename.ui \
    home.ui \
    login.ui \
    registerdialog.ui\
    addfriends.ui\
    chat_1v1.ui\
    client.ui\
    server.ui\
    grouphome.ui\
    receivefiledialog.ui\
    sendfiledialog.ui\

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
