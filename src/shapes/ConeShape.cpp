#include "ConeShape.h"

ConeShape::ConeShape()
{
}

ConeShape::ConeShape(int param1, int param2) :
    m_param1(param1),
    m_param2(param2)
{
    /**
     * We build in vertex data for a cube, in this case they are handwritten.
     * You'll want to expand upon or refactor this code heavily to take advantage
     * of polymorphism, vector math/glm library, and utilize good software design
     *
     */

    m_param1 = std::max(m_param1,1);
    m_param2 = std::max(m_param2,3);
    CreateVertices();
    Tesselate();
    /**
     * Initialize the OpenGLShapeProperties so that the shape is ready to be drawn.
     * Note that this does not actually draw the shape, that happens in ShapesScene.cpp
     */
    initializeOpenGLShapeProperties();
}

ConeShape::~ConeShape()
{
}

void ConeShape::CreateVertices()
{
    m_center.x = 0;
    m_center.y = -0.5;
    m_center.z = 0;
}


void ConeShape::Tesselate()
{
    TesselateCone(m_center, 1, 0, 0.5, &m_vertexData, false, m_param1, m_param2);
    TesselateCone(m_center, 0, 0.5, 0, &m_vertexData, true, m_param1, m_param2);
}
