#-------------------------------------------------
#
# Project created by QtCreator 2017-10-05T12:04:51
#
#-------------------------------------------------

QT       += core gui widgets

QMAKE_CXXFLAGS += -g -Wall

macx {
	CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS += -fsanitize=address -shared-libasan
        QMAKE_LFLAGS += -fsanitize=address -shared-libasan
	}
}

CONFIG += c++14

#CONFIG += static

TARGET = FQGL
TEMPLATE = lib

DEFINES += FQGL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
        ../src/fqgl_global.h \
        ../src/FQGLTypes.h \

SOURCES += \
        ../src/FQGLCamera.cpp \
        ../src/FQGLController.cpp \
        ../src/FQGLFrustumCompute.cpp \
        ../src/FQGLFrustum.cpp \
        ../src/FQGLPlane.cpp \
        ../src/FQGLPrim.cpp \
        ../src/FQGLResponder.cpp \
        ../src/FQGLScene.cpp \
        ../src/FQGLWidget.cpp \

HEADERS += \
        ../src/FQGLCamera.h \
        ../src/FQGLController.h \
        ../src/FQGLFrustumCompute.h \
        ../src/FQGLFrustum.h \
        ../src/FQGLPlane.h \
        ../src/FQGLPrim.h \
        ../src/FQGLResponder.h \
        ../src/FQGLScene.h \
        ../src/FQGLWidget.h \

ios {
    CONFIG += staticlib
    #CONFIG += shared
    CONFIG += framework

    QMAKE_IOS_DEPLOYMENT_TARGET = 10
    QMAKE_FRAMEWORK_BUNDLE_NAME = $$LIBRARY_NAME
}

macx|ios {
    CONFIG += lib_bundle
    # Link to fftreal framework
    # LIBS += -F$${fftreal_dir}
    # LIBS += -framework fftreal 

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${HEADERS}
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

	SRCDIR = $$PWD/../src

    CONFIG(staticlib, shared|staticlib){
        message("Adding copy for static framework")
        # (using QMAKE_EXTRA_TARGET will be executed before linking, which is
        # too early).
        QMAKE_POST_LINK += mkdir -p $${TARGET}.framework/Headers && \
            $$QMAKE_COPY $$SRCDIR/*.h $${TARGET}.framework/Headers && \
            $$QMAKE_COPY $$OUT_PWD/lib$${TARGET}.a $${TARGET}.framework/$${TARGET} && \
            $$QMAKE_RANLIB -s $${TARGET}.framework/$${TARGET}
    }

} else {

    # LIBS += -F$${fftreal_dir}
    # LIBS += -lfftreal

    linux-g++*: {
        # Provide relative path from application to fftreal library
        QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    }
}


unix {
    target.path = /usr/lib
    INSTALLS += target
}
