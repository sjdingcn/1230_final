#ifndef BaseShape_H
#define BaseShape_H

#include "src/openglshape.h"
#include "ShapeUtils.h"

//using namespace CS123::GL;

class BaseShape
{
public:
    BaseShape();
    BaseShape(int param1, int param2);
    ~BaseShape();

    void draw();

protected:
    virtual void CreateVertices(){};
    virtual void Tesselate(){};
    void initializeOpenGLShapeProperties();

    int m_param1;
    int m_param2;

    std::vector<GLfloat> m_vertexData;
    std::unique_ptr<CS123::GL::VAO> m_VAO;
};

#endif // BaseShape_H
