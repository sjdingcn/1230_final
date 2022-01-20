#include "PlainShape.h"

PlainShape::PlainShape()
{
}

PlainShape::PlainShape(int param1, int param2) :
    m_param1(param1),
    m_param2(param2)
{
    /**
     * We build in vertex data for a cube, in this case they are handwritten.
     * You'll want to expand upon or refactor this code heavily to take advantage
     * of polymorphism, vector math/glm library, and utilize good software design
     *
     */
    CreateVertices();
    Tesselate();
    /**
     * Initialize the OpenGLShapeProperties so that the shape is ready to be drawn.
     * Note that this does not actually draw the shape, that happens in ShapesScene.cpp
     */
    initializeOpenGLShapeProperties();
}

PlainShape::~PlainShape()
{
}

void PlainShape::CreateVertices()
{
    m_vertices.push_back(glm::vec3(0.5f, 0.5f, -1.0f));      // 0
    m_vertices.push_back(glm::vec3(0.5f, -0.5f, -1.0f));     // 1
    m_vertices.push_back(glm::vec3(-0.5f, 0.5f, -1.0f));     // 4
    m_vertices.push_back(glm::vec3(-0.5f, -0.5f, -1.0f));    // 5

    m_face[0] = 0;
    m_face[1] = 1;
    m_face[2] = 2;
    m_face[3] = 3;
}


void PlainShape::Tesselate()
{
    glm::vec3 p1 = m_vertices[m_face[0]];
    glm::vec3 p2 = m_vertices[m_face[1]];
    glm::vec3 p3 = m_vertices[m_face[2]];
    glm::vec3 p4 = m_vertices[m_face[3]];

    TesselateRectangle(p1, p2, p3, p4, &m_vertexData, true, m_param1, m_param2);
}
