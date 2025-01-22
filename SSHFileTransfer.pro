QT += core gui widgets network

# Name of the target
TARGET = SSHFileTransfer
TEMPLATE = app

# Source files
SOURCES += main.cpp \
           filetransferworker.cpp \
           mainwindow.cpp

# Header files
HEADERS += mainwindow.h \
    filetransferworker.h

# UI file
FORMS += \
    mainwindow.ui

# Include path for libssh2
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lssh2   # On Linux

# Deployment rules
RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    tiger_left.png \
    tiger_right.png
