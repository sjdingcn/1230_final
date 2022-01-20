#include "CylinderShape.h"

CylinderShape::CylinderShape()
{
}

CylinderShape::CylinderShape(int param1, int param2) :
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

CylinderShape::~CylinderShape()
{
}

void CylinderShape::CreateVertices()
{
    m_center_top.x = 0;
    m_center_top.y = 0.5;
    m_center_top.z = 0;
    m_center_bottom.x = 0;
    m_center_bottom.y = -0.5;
    m_center_bottom.z = 0;
}



void CylinderShape::Tesselate()
{
    TesselateCone(m_center_bottom, 1, 0.5, 0.5, &m_vertexData, false, m_param1, m_param2);
    TesselateCone(m_center_top, 0, 0.5, 0, &m_vertexData, true, m_param1, m_param2);
    TesselateCone(m_center_bottom, 0, 0.5, 0, &m_vertexData, true, m_param1, m_param2);
}
