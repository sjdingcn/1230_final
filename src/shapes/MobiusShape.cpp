#include "MobiusShape.h"

MobiusShape::MobiusShape()
{
}

MobiusShape::MobiusShape(int param1, int param2) :
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
    m_param2 = std::max(m_param2,10);
//    m_param2 = std::min(m_param2,5);
    CreateVertices();
    Tesselate();
    /**
     * Initialize the OpenGLShapeProperties so that the shape is ready to be drawn.
     * Note that this does not actually draw the shape, that happens in ShapesScene.cpp
     */
    initializeOpenGLShapeProperties();
}

MobiusShape::~MobiusShape()
{
}

void MobiusShape::CreateVertices()
{
}


void MobiusShape::Tesselate()
{
    TesselateMobius(0.5, 0.1, & m_vertexData,
                       false, m_param1, m_param2);
    TesselateMobius(0.5, -0.1, & m_vertexData,
                       false, m_param1, m_param2);
}
