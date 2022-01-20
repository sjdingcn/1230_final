#include "view.h"

#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>
#include "src/gl/shaders/ShaderAttribLocations.h"
#include "src/openglshape.h"
//#include "errorchecker.h"

#include "lib/sphere.h"
#include "lib/resourceloader.h"
#include "lib/errorchecker.h"
#include "src/settings.h"

#include "src/gl/shaders/CS123Shader.h"
#include "src/gl/datatype/FBO.h"
#include "CS123XmlSceneParser.h"
#include "src/gl/textures/Texture2D.h"
#include "view_param.h"

using namespace CS123::GL;

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_time(), m_timer(), m_captureMouse(false),
    m_angleX(-0.5f), m_angleY(0.5f), m_zoom(10.f)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor
    if (m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // load scene from a fixed scene file
    QString file = ":/scenes/undersea.xml";
    m_sceneParser = std::unique_ptr<CS123XmlSceneParser>(new CS123XmlSceneParser(file.toLatin1().data()));
    m_sceneParser->parse();
    parse(this, m_sceneParser.get());

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
    int len = m_shapes.size();
    for(int i=len-1;i>-1;i--)
    {
        BaseShape* shape_ptr = m_shapes[i];
        delete shape_ptr;
    }
    m_shapes.clear();
}


void applyTexParametri(Texture2D& tex)
{
    TextureParametersBuilder builder;
    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
    TextureParameters parameters = builder.build();
    parameters.applyTo(tex);
}

void View::initializeGL() {
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    else fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    resizeGL(width(), height());

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    m_time.start();
    m_timer.start(1000 / 60);

    initializeObjects();
    
}

void View::initializeObjects() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Creates the shader program that will be used for drawing.
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/phong.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

//    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
//    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/identity.frag");
//    m_identityShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/godray.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/godray.frag");
    m_godrayShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shadowMap.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shadowMap.frag");
    m_shadowMapShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shadow.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shadow.frag");
    m_shadowShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/water.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/water.frag");
    m_waterShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/pattern.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/pattern.frag");
    m_patternShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

//    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
//    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/foggy.frag");
//    m_foggyGodrayShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/hdr.frag");
    m_hdrShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    std::vector<GLfloat> quadData{
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        0.0f, 0.0f,
    };
    m_quad = std::make_unique<OpenGLShape>();
    m_quad->setVertexData(&quadData[0], quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3 * sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_quad->buildVAO();

    // Initialize sphere with radius 0.5 centered at origin.
    for (CS123ScenePrimitive prim: m_primitives)
    {
        BaseShape* shape_ptr;
        if (prim.type == PrimitiveType::PRIMITIVE_CUBE)
            shape_ptr = new CubeShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_CONE)
            shape_ptr = new ConeShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_SPHERE)
            shape_ptr = new SphereShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_CYLINDER)
            shape_ptr = new CylinderShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_TORUS)
            shape_ptr = new TorusShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_MOBIUS)
            shape_ptr = new MobiusShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_SPIKE)
            shape_ptr = new SpikeShape(m_param1, 3);
        else if (prim.type == PrimitiveType::PRIMITIVE_TANK)
            shape_ptr = new TankShape(m_param1, m_param2);
        else if (prim.type == PrimitiveType::PRIMITIVE_TERRAIN)
            shape_ptr = new TerrainShape(m_param1*3, m_param2*3);
        else if (prim.type == PrimitiveType::PRIMITIVE_MESH)
            shape_ptr = new MeshShape(prim.meshfile);
        else
            shape_ptr = new BaseShape();
        m_shapes.push_back(shape_ptr);
    }

    m_godray_mask = std::make_unique<PlainShape>(m_godray_grain, m_godray_grain);
    m_sea_surface = std::make_unique<WaveShape>(32, 32, 2, 8, 0.5, glm::vec2(1,-2));

}

void View::setOpenGLTone()
{
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    glEnable(GL_MULTISAMPLE);
}

void View::paintGL() {
    rebuildMatrices();

//    if (settings.hdr) {
//        m_hdrFBO->bind();
//    }
//    else {
//        glViewport(0, 0, m_width, m_height);
//    }
    setOpenGLTone();



    if (settings.shadowMapping) drawShadowMap();

//    drawObjectsWrapper();

    if (settings.crepuscularRays)
    {
        // draw and save the previous rendered scene
//        m_saveFBO->bind();
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        drawObjectsWrapper();
//        m_saveFBO->unbind();
        if (settings.hdr) m_hdrFBO->bind();
        drawObjectsWrapper();
//        // draw the original scene
        std::vector<glm::vec3>* mesh = m_sea_surface->get_vertex();
        addGodray(mesh);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, m_width, m_height);
        drawObjectsWrapper();
    }


    if (settings.hdr) {
        m_hdrFBO->unbind();
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, m_width, m_height);
        m_hdrShader->bind();
        m_hdrShader->setUniform("exposure", settings.hdrExposure / 20.0f);
        m_hdrFBO->getColorAttachment(0).bind();
        m_quad->draw();
        m_hdrFBO->getColorAttachment(0).unbind();
        m_hdrShader->unbind();
    }


}

void View::drawObjectsWrapper()
{
    if (settings.hdr && (!settings.crepuscularRays)) m_hdrFBO->bind();

    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (settings.shadowMapping) {
        drawObjectsWShadow();
    } else {
        drawObjects();
    }
}

void View::drawObjects()
{
    // draw the objects in the water
    m_phongShader->bind();
    m_phongShader->setUniform("ka", m_global.ka);
    m_phongShader->setUniform("kd", m_global.kd);
    m_phongShader->setUniform("ks", m_global.ks);

    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("isShapeScene", false);
    m_phongShader->setUniform("p" , m_projection);
    m_phongShader->setUniform("v", m_view);

    m_phongShader->setUniform("extinc", 0.07f);
    m_phongShader->setUniform("sea_level", 5.f);

    changeDirectionalLightsColors();

    for (CS123SceneLightData light:m_lights) m_phongShader->setLight(light);

    int size_shape = m_shapes.size();
    for (int i=0;i<size_shape-1;i++)
    {
        m_phongShader->setUniform("m", m_transforms[i]);
        m_phongShader->applyMaterial(m_primitives[i].material);

        m_shapes[i]->draw();
    }

    m_phongShader->unbind();
    //draw the water surface
    drawWaterSurface();
    // draw light pattern by the water
    m_patternShader->bind();

    int h = m_sea_surface->m_param1+1;
    int w = m_sea_surface->m_param2+1;
    unsigned char norms[h][w][4];
    memset(norms, 0, h*w*4*sizeof(unsigned char));
    std::vector<glm::vec3>* mesh = m_sea_surface->get_vertex();
    loadTexture((unsigned char*)norms, TexSource::NORMAL_FIELD, mesh);
    Texture2D tex((unsigned char*)norms, w, h);
    applyTexParametri(tex);

    m_patternShader->setUniform("p" , m_projection);
    m_patternShader->setUniform("v", m_view);
    m_patternShader->setUniform("m", m_transforms[1]);
    m_patternShader->setUniform("lightColor", glm::vec3(
                                  settings.directionalLightR / 255.0f,
                                  settings.directionalLightG / 255.0f,
                                  settings.directionalLightB / 255.0f
                                                    ));
    m_patternShader->applyMaterial(m_primitives[1].material);
    m_patternShader->setTexture("wave_surface", tex);
    m_shapes[1]->draw();

    m_patternShader->unbind();



    // draw the outer tank
    m_phongShader->bind();
    m_phongShader->setUniform("m", m_transforms[size_shape-1]);
    m_phongShader->applyMaterial(m_primitives[size_shape-1].material);
    m_shapes[size_shape-1]->draw();
    m_phongShader->unbind();

    // Render the normals. Mainly for bug detection
//    m_normalsShader->bind();
//    m_normalsShader->setUniform("p", m_projection);
//    m_normalsShader->setUniform("v", m_view);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//    m_normalsShader->setUniform("m", glm::translate(glm::vec3(0,0,0))*glm::scale(glm::vec3(10,5,10)) );
//    m_sea_surface->draw();
////    m_normalsShader->setUniform("m", m_transforms[size_shape-1]);
////    m_shapes[size_shape-1]->draw();

//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    m_normalsShader->unbind();

}

void View::drawWaterSurface()
{
    m_waterShader->bind();

    m_waterShader->setUniform("ka", m_global.ka);
    m_waterShader->setUniform("kd", m_global.kd);
    m_waterShader->setUniform("ks", m_global.ks);

    m_waterShader->setUniform("p" , m_projection);
    m_waterShader->setUniform("v", m_view);

    m_waterShader->setUniform("extinc", 0.01f);
    m_waterShader->setUniform("sea_level", 5.f);

    changeDirectionalLightsColors();

    for (CS123SceneLightData light:m_lights) m_waterShader->setLight(light);

    m_waterShader->setUniform("m", glm::translate(glm::vec3(0,5,0))*glm::scale(glm::vec3(30,2,30)) );
//    m_waterShader->setUniform("m", glm::translate(glm::vec3(0,0,0))*glm::scale(glm::vec3(10,5,10)) );
    m_waterShader->setUniform("ambient_color", glm::vec3(0.0, 0.2, 0.6) );
    m_waterShader->setUniform("diffuse_color", glm::vec3(0.0, 0.3, 0.6) );
    m_waterShader->setUniform("specular_color", glm::vec3(0.5, 0.5, 0.5) );
    m_waterShader->setUniform("shininess", 30);
    m_waterShader->setUniform("transparent", glm::vec3(0.8, 0.8, 0.8));
    m_waterShader->setUniform("r0", 0.2f);
    m_waterShader->setUniform("eta", 9.f);

    m_sea_surface->WaveUpdate();
    m_sea_surface->draw();
    m_waterShader->unbind();


}


void View::loadTexture(unsigned char* heights, TexSource tex_type, std::vector<glm::vec3>* mesh)
{

    int h = m_sea_surface->m_param1+1;
    int w = m_sea_surface->m_param2+1;
    memset(heights, 0, h*w*4*sizeof(unsigned char));
    float height;
    glm::vec3 incident_ray = glm::normalize(glm::vec3(0, -10, 8));
    glm::vec3 refract;
    int i_ref, j_ref;
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++)
        {
            if(tex_type == TexSource::HEIGHT_FIELD)
            {
                if (j==0 || j==w-1) heights[4*(i*w+j)+0] = 0;
                else {
                    height = (mesh->at(i*w+j-1).y - mesh->at(i*w+j+1).y)*w/15;
                    heights[4*(i*w+j)+0] = std::min(std::max(height, 0.f), 1.f)*255;
                }
                heights[4*(i*w+j)+1] = std::min(std::max(-height, 0.f), 1.f)*255;
                heights[4*(i*w+j)+2] = std::min(std::max(height, 0.f), 1.f)*255;
            }
            else
            {
//                refract = glm::refract(incident_ray, mesh->at(i*w+j), 0.75f);
//                float dis = 10.f/(refract.y+0.001);
//                i_ref = i+dis*refract.z;
//                j_ref = j+dis*refract.x;
//                if( i_ref<h && i_ref>-1 && j_ref<w && j_ref>-1)
//                {
//                    heights[4*(i_ref*w+j_ref) + 0] += 5;
//                }
                height = std::abs(mesh->at(i*w+j).y);
                if (height<0.07)
                    heights[4*(i*w+j)+0] = (0.07f-height)*5*255;
            }
        }

}

void View::addGodray(std::vector<glm::vec3>* mesh)
{
    glClear(GL_DEPTH_BUFFER_BIT);

    m_godrayShader->bind();

    int h = m_sea_surface->m_param1+1;
    int w = m_sea_surface->m_param2+1;
    unsigned char heights[h][w][4];
    memset(heights, 0, h*w*4*sizeof(unsigned char));
    mesh = m_sea_surface->get_vertex();
    loadTexture((unsigned char*)heights, TexSource::HEIGHT_FIELD, mesh);
    Texture2D tex((unsigned char*)heights, w, h);
    applyTexParametri(tex);

    m_godrayShader->setTexture("wave_surface", tex);
    m_godrayShader->setUniform("surface_width", (float) w);
    m_godrayShader->setUniform("surface_height", (float) h);
    m_godrayShader->setUniform("finegrainess", m_godray_grain);
    m_godrayShader->setUniform("p" , m_projection);
    m_godrayShader->setUniform("v", m_view);

    glm::mat4 m;
    if (m_width>m_height)
        m = glm::mat4x4(1.0f)*glm::scale(glm::vec3(4.5*m_width/m_height,4.5,5));
    else
        m = glm::mat4x4(1.0f)*glm::scale(glm::vec3(4.5,4.5*m_width/m_height,5));
//    glm::mat4 m = glm::mat4x4(1.0f)*glm::rotate(-m_angleX, glm::vec3(0,1,0))*glm::scale(glm::vec3(20,10,5));
    m_godrayShader->setUniform("m" , m);

    load_param_godray_shader(m_godrayShader);

    m_godrayShader->setUniform("lightColor", glm::vec3(
                                  settings.directionalLightR / 255.0f,
                                  settings.directionalLightG / 255.0f,
                                  settings.directionalLightB / 255.0f
                                                    ));
    m_godrayShader->setUniform("diffuse_color", glm::vec3(
                                  settings.directionalLightR / 255.0f*0.5,
                                  settings.directionalLightG / 255.0f*0.5,
                                  settings.directionalLightB / 255.0f*0.5
                                                    ));

    m_godray_mask->draw();
    m_godrayShader->unbind();
}




void View::drawShadowMap(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);

    m_shadowMapShader->bind();
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-50,50,-50,50,-50,50);
    glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(-m_lights[0].dir), glm::vec3(0,0,0), glm::vec3(0,1,0));
    m_shadowMapShader->setUniform("p" , depthProjectionMatrix);
    m_shadowMapShader->setUniform("v", depthViewMatrix);


    m_shadowFBO->bind();
    for (int i=0;i<m_shapes.size()-1;i++)
    {

        m_shadowMapShader->setUniform("m", m_transforms[i]);

        m_shapes[i]->draw();
    }
    m_shadowFBO->unbind();

    m_shadowMapShader->unbind();
}


void View::drawObjectsWShadow() {

    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-50,50,-50,50,-50,50);
    glm::mat4 depthViewMatrix = glm::lookAt(glm::vec3(-m_lights[0].dir), glm::vec3(0,0,0), glm::vec3(0,1,0));
    m_shadowShader->bind();
    m_shadowShader->setUniform("ka", m_global.ka);
    m_shadowShader->setUniform("kd", m_global.kd);
    m_shadowShader->setUniform("ks", m_global.ks);

    m_shadowShader->setUniform("useArrowOffsets", false);
    m_shadowShader->setUniform("isShapeScene", false);
    m_shadowShader->setUniform("p" , m_projection);
    m_shadowShader->setUniform("v", m_view);

    m_shadowShader->setUniform("extinc", 0.07f);
    m_shadowShader->setUniform("sea_level", 5.f);
    m_shadowShader->setUniform("biasMatrix", biasMatrix);


    for (CS123SceneLightData light:m_lights) m_shadowShader->setLight(light);

    m_shadowShader->setUniform("dp" , depthProjectionMatrix);
    m_shadowShader->setUniform("dv", depthViewMatrix);
    m_shadowFBO->getColorAttachment(0).bind();
    int size_shape = m_shapes.size();
    for (int i=0;i<m_shapes.size()-1;i++)
    {
        m_shadowShader->setUniform("m", m_transforms[i]);
        m_shadowShader->setUniform("dm", m_transforms[i]);
        m_shadowShader->applyMaterial(m_primitives[i].material);

        m_shapes[i]->draw();
    }
    m_shadowFBO->getColorAttachment(0).unbind();

    m_shadowShader->unbind();
    // water
    drawWaterSurface();

    // draw light pattern by the water
    m_patternShader->bind();

    int h = m_sea_surface->m_param1+1;
    int w = m_sea_surface->m_param2+1;
    unsigned char norms[h][w][4];
    memset(norms, 0, h*w*4*sizeof(unsigned char));
    std::vector<glm::vec3>* mesh = m_sea_surface->get_vertex();
    loadTexture((unsigned char*)norms, TexSource::NORMAL_FIELD, mesh);
    Texture2D tex((unsigned char*)norms, w, h);
    applyTexParametri(tex);

    m_patternShader->setUniform("p" , m_projection);
    m_patternShader->setUniform("v", m_view);
    m_patternShader->setUniform("m", m_transforms[1]);
    m_patternShader->setUniform("lightColor", glm::vec3(
                                  settings.directionalLightR / 255.0f,
                                  settings.directionalLightG / 255.0f,
                                  settings.directionalLightB / 255.0f
                                                    ));
    m_patternShader->applyMaterial(m_primitives[1].material);
    m_patternShader->setTexture("wave_surface", tex);
    m_shapes[1]->draw();

    m_patternShader->unbind();


    m_shadowShader->bind();
    m_shadowShader->setUniform("m", m_transforms[size_shape-1]);
    m_shadowShader->applyMaterial(m_primitives[size_shape-1].material);
    m_shapes[size_shape-1]->draw();

    m_shadowShader->unbind();
}


void View::resizeGL(int w, int h) {
    m_width = w;
    m_height = h;
    m_hdrFBO = std::make_unique<FBO>(
        1,
        FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY,
        w, h,
        TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
        TextureParameters::FILTER_METHOD::LINEAR,
        GL_FLOAT
    );

//    m_blurFBO = std::make_unique<FBO>(
//        1,
//        FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY,
//        w, h,
//        TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
//        TextureParameters::FILTER_METHOD::LINEAR,
//        GL_FLOAT
//    );

//    m_saveFBO = std::make_unique<FBO>(
//        1,
//        FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY,
//        w, h,
//        TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
//        TextureParameters::FILTER_METHOD::LINEAR,
//        GL_FLOAT
//    );

    m_shadowFBO = std::make_unique<FBO>(
        1,
        FBO::DEPTH_STENCIL_ATTACHMENT::NONE,
        w, h,
        TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
        TextureParameters::FILTER_METHOD::LINEAR,
        GL_FLOAT
    );

}

void View::mousePressEvent(QMouseEvent *event) {
    m_prevMousePos = event->pos();
    m_captureMouse = true;
}

void View::mouseMoveEvent(QMouseEvent *event) {
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    if(m_captureMouse) {
//        int deltaX = event->x() - width() / 2;
//        int deltaY = event->y() - height() / 2;
        int deltaX = event->x() - m_prevMousePos.x();
        int deltaY = event->y() - m_prevMousePos.y();
        if (!deltaX && !deltaY) return;
//        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here
        m_angleX += 3 * deltaX / (float) width();
        m_angleY += 3 * deltaY / (float) height();
        m_prevMousePos = event->pos();
        rebuildMatrices();
    }
}

void View::wheelEvent(QWheelEvent *event) {
    m_zoom -= event->delta() / 100.f;
    rebuildMatrices();
}

void View::mouseReleaseEvent(QMouseEvent *event) {
    m_captureMouse = false;
}

void View::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event) {

}

void View::tick() {
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;

    // TODO: Implement the demo update here
    m_sea_surface->addTime(seconds);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}

void View::rebuildMatrices() {
    m_view = glm::translate(glm::vec3(0, 0, -m_zoom)) *
             glm::rotate(m_angleY, glm::vec3(1,0,0)) *
             glm::rotate(m_angleX, glm::vec3(0,1,0));

    m_projection = glm::perspective(0.8f, (float)width()/height(), 0.1f, 100.f);
    update();
}

void View::changeDirectionalLightsColors() {
    for (CS123SceneLightData& light : m_lights) {
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            light.color.r = settings.directionalLightR / 255.0f;
            light.color.g = settings.directionalLightG / 255.0f;
            light.color.b = settings.directionalLightB / 255.0f;
        }
    }
}




//void View::addGodrayBlur(std::vector<glm::vec3>* mesh)
//{
//    m_blurFBO->bind();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    m_godrayShader->bind();

//    int h = m_sea_surface->m_param1+1;
//    int w = m_sea_surface->m_param2+1;
//    unsigned char heights[h][w][4];
//    for(int i=0;i<h;i++)
//        for(int j=0;j<w;j++)
//        {
//            float height = mesh->at(i*w+j).y;
//            heights[i][j][0] = std::min(std::max(height, 0.f), 1.f)*255;
//            heights[i][j][1] = std::min(std::max(-height, 0.f), 1.f)*255;;
//            heights[i][j][2] = 0;
//            heights[i][j][3] = 0;
//        }

//    Texture2D tex((unsigned char*)heights, w, h);

//    TextureParametersBuilder builder;
//    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
//    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
//    TextureParameters parameters = builder.build();
//    parameters.applyTo(tex);
//    m_godrayShader->setTexture("wave_surface", tex);
//    m_godrayShader->setUniform("surface_width", (float) w);
//    m_godrayShader->setUniform("surface_height", (float) h);
//    m_godrayShader->setUniform("finegrainess", m_godray_grain);
//    m_godrayShader->setUniform("p" , m_projection);
//    m_godrayShader->setUniform("v", m_view);
//    load_param_godray_shader(m_godrayShader);

//    m_godray_mask->draw();
//    m_godrayShader->unbind();

//    m_blurFBO->unbind();

////    glDisable(GL_BLEND);
//    if (settings.hdr) m_hdrFBO->bind();
//    else glViewport(0, 0, m_width, m_height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    // draw the original scene
//    m_identityShader->bind();
//    m_saveFBO->getColorAttachment(0).bind();
//    m_quad->draw();
//    m_saveFBO->getColorAttachment(0).unbind();
//    m_identityShader->unbind();
//    // draw the original scene
//    glClear(GL_DEPTH_BUFFER_BIT);
//    m_foggyGodrayShader->bind();
//    m_foggyGodrayShader->setUniform("unit", 1 / m_width);
//    m_blurFBO->getColorAttachment(0).bind();
//    m_quad->draw();
//    m_blurFBO->getColorAttachment(0).unbind();
//    m_foggyGodrayShader->unbind();
//}
