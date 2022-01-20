#include "TankShape.h"

TankShape::TankShape()
{
}

TankShape::TankShape(int param1, int param2) :
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

TankShape::~TankShape()
{
}

void TankShape::CreateVertices()
{
    m_vertices.push_back(glm::vec3(0.5f, 0.5f, 0.5f));      // 0
    m_vertices.push_back(glm::vec3(0.5f, -0.5f, 0.5f));     // 1
    m_vertices.push_back(glm::vec3(0.5f, -0.5f, -0.5f));    // 2
    m_vertices.push_back(glm::vec3(0.5f, 0.5f, -0.5f));     // 3
    m_vertices.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));     // 4
    m_vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));    // 5
    m_vertices.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));   // 6
    m_vertices.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));    // 7

    int faces[5][4] = {{0,1,3,2},
                       {7,6,4,5},
                       {4,5,0,1},
                       {5,6,1,2},
                       {3,2,7,6}};
    for (int i=0;i<5;i++)
    {
        for (int j=0;j<4;j++)
        {
            m_faces.push_back(faces[i][j]);
        }
    }
}


void TankShape::Tesselate()
{
    for(int i=0;i<5;i++)
    {
        glm::vec3 p1 = m_vertices[m_faces[i*4+0]];
        glm::vec3 p2 = m_vertices[m_faces[i*4+1]];
        glm::vec3 p3 = m_vertices[m_faces[i*4+2]];
        glm::vec3 p4 = m_vertices[m_faces[i*4+3]];

        TesselateRectangle(p1, p2, p3, p4, &m_vertexData, true, m_param1, m_param1);
    }
}
