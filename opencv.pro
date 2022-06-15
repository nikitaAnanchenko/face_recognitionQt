TEMPLATE = app
TARGET = OpenCV
INCLUDEPATH += .

QT += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_video -lopencv_videoio -lopencv_objdetect -lopencv_dnn -lopencv_face

HEADERS += \
    include/algorithms.h \
    include/capture.h \
    include/mainwindow.h

SOURCES += \
    src/algorithms.cpp \
    src/capture.cpp \
    src/main.cpp \
    gui/mainwindow.cpp

FORMS += \
    gui/mainwindow.ui

RESOURCES += \
    resources/resources.qrc
