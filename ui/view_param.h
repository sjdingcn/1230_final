#ifndef VIEW_PARAM_H
#define VIEW_PARAM_H

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


#include "src/gl/shaders/CS123Shader.h"


void load_param_godray_shader(std::unique_ptr<CS123::GL::CS123Shader>& shader)
{
//    shader->setUniform("diffuse_color", glm::vec3(0.4, 0.4, 0.9));
    shader->setUniform("sea_level", 5.f);
    shader->setUniform("tone", glm::vec3(4.7, 4.7, 4.7));
    shader->setUniform("func", glm::vec3(0.5, 0.2, 0.05));
    shader->setUniform("extinc", 0.25f);
    shader->setUniform("transparent", 1.f);


    shader->setUniform("surface_m", glm::translate(glm::vec3(0,5,0))*glm::scale(glm::vec3(30,2,30)));

    shader->setUniform("lightDirection", glm::vec3(0, -10, 8));
    shader->setUniform("lightPosition", glm::vec3(0, 10, -8));
}


#endif // VIEW_H
