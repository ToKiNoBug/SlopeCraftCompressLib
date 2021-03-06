QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
INCLUDEPATH += D:/eigen-3.3.9 \
                               D:/Git/SlopeCraft/Kernel
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HeightLine.cpp \
    OptiChain.cpp \
    getBHL.cpp \
    main.cpp \
    CompressWind.cpp

HEADERS += \
    CompressWind.h \
    HeightLine.h \
    OptiChain.h

FORMS += \
    CompressWind.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
