#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <memory>
#include <QGLWidget>
#include <math.h>

#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include "src/Scene.h"
#include "src/gl/textures/Texture2D.h"
#include "src/gl/textures/TextureParametersBuilder.h"
#include "src/shapes/ShapeResources.h"

enum class TexSource {
    HEIGHT_FIELD,
    NORMAL_FIELD
};
class BaseShape;
class OpenGLShape;

class CS123XmlSceneParser;
namespace CS123 { namespace GL {
    class Shader;
    class CS123Shader;
    class Texture2D;
    class FBO;
}}


//class View : public QGLWidget {
class View : public QGLWidget, Scene {
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    // basic members
    QTime m_time;
    QTimer m_timer;
    bool m_captureMouse = false;

    void initializeGL();
    void initializeObjects();
    void paintGL();
    void resizeGL(int w, int h);

    std::unique_ptr<CS123XmlSceneParser> m_sceneParser;


    // camera hci
private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void rebuildMatrices();

    void changeDirectionalLightsColors();

    void setOpenGLTone();
    void drawObjects();
    glm::mat4 m_view, m_projection;
    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;

    /** For mouse interaction. */
    float m_angleX, m_angleY, m_zoom;
    QPoint m_prevMousePos;

    // HDR members
private:
    std::unique_ptr<CS123::GL::FBO> m_hdrFBO;
    std::unique_ptr<CS123::GL::CS123Shader> m_hdrShader;
    std::unique_ptr<OpenGLShape> m_quad;
    int m_width;
    int m_height;

    // crepuscular ray members
private:
    std::unique_ptr<CS123::GL::CS123Shader> m_waterShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_godrayShader;
//    std::unique_ptr<CS123::GL::FBO> m_blurFBO;
//    std::unique_ptr<CS123::GL::CS123Shader> m_foggyGodrayShader;
//    std::unique_ptr<CS123::GL::FBO> m_saveFBO;
    std::unique_ptr<CS123::GL::CS123Shader> m_identityShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_patternShader;

    void drawWaterSurface();
    void loadTexture(unsigned char* heights, TexSource tex_type, std::vector<glm::vec3>* mesh);
    void addGodray(std::vector<glm::vec3>* mesh);

    std::vector<BaseShape*> m_shapes;
    std::unique_ptr<BaseShape> m_godray_mask;
    std::unique_ptr<WaveShape> m_sea_surface;
    int m_param1 = 40;
    int m_param2 = 40;
    float m_godray_grain = 200;

    // Shadow mapping members
private:
    std::unique_ptr<CS123::GL::CS123Shader> m_shadowShader;
    std::unique_ptr<CS123::GL::CS123Shader> m_shadowMapShader;
    std::unique_ptr<CS123::GL::FBO> m_shadowFBO;
    void drawObjectsWrapper();
    void drawShadowMap();
    void drawObjectsWShadow();

private slots:
    void tick();
};


#endif // VIEW_H
