#include "TorusShape.h"

TorusShape::TorusShape()
{
}

TorusShape::TorusShape(int param1, int param2) :
    m_param1(param1),
    m_param2(param2)
{
    /**
     * We build in vertex data for a cube, in this case they are handwritten.
     * You'll want to expand upon or refactor this code heavily to take advantage
     * of polymorphism, vector math/glm library, and utilize good software design
     *
     */

    m_param1 = std::max(m_param1,3);
    m_param2 = std::max(m_param2,3);
    CreateVertices();
    Tesselate();
    /**
     * Initialize the OpenGLShapeProperties so that the shape is ready to be drawn.
     * Note that this does not actually draw the shape, that happens in ShapesScene.cpp
     */
    initializeOpenGLShapeProperties();
}

TorusShape::~TorusShape()
{
}

void TorusShape::CreateVertices()
{
    m_center.x = 0;
    m_center.y = -0.5;
    m_center.z = 0;
}


void TorusShape::Tesselate()
{
    TesselateSphere(360, 1, 0.3, &m_vertexData, false, m_param1, m_param2);
}
