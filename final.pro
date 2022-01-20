# -------------------------------------------------
# Project created by QtCreator 2010-08-22T14:12:19
# -------------------------------------------------
QT += opengl xml
TARGET = final
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14

unix:!macx {
    LIBS += -lGLU
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -lglu32
}

SOURCES += ui/mainwindow.cpp \
    lib/CS123XmlSceneParser.cpp \
    lib/RGBA.cpp \
    lib/errorchecker.cpp \
    lib/resourceloader.cpp \
    main.cpp \
    glew-1.10.0/src/glew.c \
    src/Scene.cpp \
    src/gl/datatype/FBO.cpp \
    src/gl/GLDebug.cpp \
    src/gl/datatype/IBO.cpp \
    src/gl/datatype/VAO.cpp \
    src/gl/datatype/VBO.cpp \
    src/gl/datatype/VBOAttribMarker.cpp \
    src/gl/shaders/CS123Shader.cpp \
    src/gl/shaders/Shader.cpp \
    src/gl/textures/DepthBuffer.cpp \
    src/gl/textures/RenderBuffer.cpp \
    src/gl/textures/Texture.cpp \
    src/gl/textures/Texture2D.cpp \
    src/gl/textures/TextureParameters.cpp \
    src/gl/textures/TextureParametersBuilder.cpp \
    src/openglshape.cpp \
    src/settings.cpp \
    src/shapes/BaseShape.cpp \
    src/shapes/ConeShape.cpp \
    src/shapes/CubeShape.cpp \
    src/shapes/CylinderShape.cpp \
    src/shapes/MeshShape.cpp \
    src/shapes/MobiusShape.cpp \
    src/shapes/PlainShape.cpp \
    src/shapes/ShapeUtils.cpp \
    src/shapes/SphereShape.cpp \
    src/shapes/SpikeShape.cpp \
    src/shapes/TankShape.cpp \
    src/shapes/TerrainShape.cpp \
    src/shapes/TorusShape.cpp \
    src/shapes/WaveShape.cpp \
    ui/view.cpp \
    ui/viewformat.cpp

HEADERS += ui/mainwindow.h \
    lib/CS123ISceneParser.h \
    lib/CS123SceneData.h \
    lib/CS123XmlSceneParser.h \
    lib/RGBA.h \
    lib/errorchecker.h \
    lib/resourceloader.h \
    lib/sphere.h \
    src/Scene.h \
    src/gl/GLDebug.h \
    src/gl/datatype/FBO.h \
    src/gl/datatype/IBO.h \
    src/gl/datatype/VAO.h \
    src/gl/datatype/VBO.h \
    src/gl/datatype/VBOAttribMarker.h \
    src/gl/shaders/CS123Shader.h \
    src/gl/shaders/Shader.h \
    src/gl/shaders/ShaderAttribLocations.h \
    src/gl/textures/DepthBuffer.h \
    src/gl/textures/RenderBuffer.h \
    src/gl/textures/Texture.h \
    src/gl/textures/Texture2D.h \
    src/gl/textures/TextureParameters.h \
    src/gl/textures/TextureParametersBuilder.h \
    src/openglshape.h \
    src/settings.h \
    src/shapes/BaseShape.h \
    src/shapes/ConeShape.h \
    src/shapes/CubeShape.h \
    src/shapes/CylinderShape.h \
    src/shapes/MeshShape.h \
    src/shapes/MobiusShape.h \
    src/shapes/PlainShape.h \
    src/shapes/ShapeResources.h \
    src/shapes/ShapeUtils.h \
    src/shapes/SphereShape.h \
    src/shapes/SpikeShape.h \
    src/shapes/TankShape.h \
    src/shapes/TerrainShape.h \
    src/shapes/TorusShape.h \
    src/shapes/WaveShape.h \
    src/shapes/WaveUtils.h \
    ui/view_param.h \
    ui_mainwindow.h \
    glew-1.10.0/include/GL/glew.h \
    ui/view.h \
    ui/viewformat.h

FORMS += ui/mainwindow.ui
INCLUDEPATH += src glm ui lib glew-1.10.0/include
DEPENDPATH += src glm ui lib glew-1.10.0/include


DEFINES += _USE_MATH_DEFINES
DEFINES += TIXML_USE_STL
DEFINES += GLM_SWIZZLE GLM_FORCE_RADIANS
OTHER_FILES += shaders/shader.frag \
    shaders/shader.vert \
    shaders/phong.frag \
    shaders/phong.vert

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

QMAKE_CXXFLAGS += -g

# QMAKE_CXX_FLAGS_WARN_ON += -Wunknown-pragmas -Wunused-function -Wmain

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}

RESOURCES += \
    resources.qrc

DISTFILES += \
    scenes/undersea.xml \
    shaders/foggy.frag \
    shaders/godray.frag \
    shaders/godray.vert \
    shaders/hdr.frag \
    shaders/identity.frag \
    shaders/normals/normals.vert \
    shaders/normals/normals.frag \
    shaders/normals/normals.gsh \
    shaders/normals/normalsArrow.gsh \
    shaders/normals/normalsArrow.frag \
    shaders/normals/normalsArrow.vert \
    shaders/pattern.frag \
    shaders/pattern.vert \
    shaders/phong.frag \
    shaders/phong.vert \
    shaders/shadow.frag \
    shaders/shadow.vert \
    shaders/shadowMap.frag \
    shaders/shadowMap.vert \
    shaders/quad.vert \
    shaders/shader.frag \
    shaders/shader.vert \
    shaders/water.frag \
    shaders/water.vert \
    shaders/water_backup.vert \
    src/shapes/README_shapes.txt
