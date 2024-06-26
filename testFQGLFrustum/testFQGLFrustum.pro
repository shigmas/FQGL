QT       += opengl testlib

QT       -= gui

QMAKE_CXXFLAGS += -g

TARGET = testFQGLFrustum

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

SOURCES += \

SOURCES += \
        FQGLFrustumTest.cpp \

HEADERS += \
        FQGLFrustumTest.h \


# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
INSTALLS += target

macx {
    fqgl_dir = $$OUT_PWD/../lib
    fqgl_framework_name = FQGL.framework
    fqgl_lib_name = $${fqgl_framework_name}/Versions/1/FQGL


    LIBS += -F$${fqgl_dir} -framework FQGL

    INCLUDEPATH += $${fqgl_dir}/$${fqgl_framework_name}/Headers

    QMAKE_POST_LINK = \
	install_name_tool -change $${fqgl_lib_name} \
		@rpath/$${fqgl_lib_name} \
		$${TARGET} && \
	install_name_tool -add_rpath $${fqgl_dir}/ \
		$${TARGET}

}        

ios {
    # We build the library as a static library, so we link it as static,
    # pulling in the headers from the src location, and link the lib from the
    # lib directory
    fqgl_dir = $$OUT_PWD/../lib
    fqgl_src_dir = $$PWD/../src
    fqgl_framework_name = FQGL.framework
    fqgl_lib_name = $${fqgl_framework_name}/Versions/1/FQGL


    LIBS += -L$${fqgl_dir} -l FQGL

    INCLUDEPATH += $${fqgl_src_dir}/
}        
