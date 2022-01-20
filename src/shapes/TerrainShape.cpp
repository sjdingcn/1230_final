#include "TerrainShape.h"

TerrainShape::TerrainShape()
{
}

TerrainShape::TerrainShape(int param1, int param2) :
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

TerrainShape::~TerrainShape()
{
}

void TerrainShape::CreateVertices()
{
    m_vertices.push_back(glm::vec3(-0.5f, 0.f, -0.5f));
    m_vertices.push_back(glm::vec3(-0.5f, 0.f,  0.5f));
    m_vertices.push_back(glm::vec3( 0.5f, 0.f, -0.5f));
    m_vertices.push_back(glm::vec3( 0.5f, 0.f,  0.5f));
}

float TerrainShape::randValue(int row, int col)
{
    return glm::fract((sin(row * 127.1f + col * 311.7f)+1.f) * 43758.5453123f);
}


float TerrainShape::getY(int row, int col, float octave)
{
    float A = randValue(std::floor(row/octave), std::floor(col/octave));
    float B = randValue(std::floor(row/octave), std::ceil(col/octave));
    float C = randValue(std::ceil(row/octave), std::floor(col/octave));
    float D = randValue(std::ceil(row/octave), std::ceil(col/octave));
    float frac_y = glm::fract(row/octave);
    float frac_x = glm::fract(col/octave);
    frac_y = frac_y*frac_y*(3-2*frac_y);
    frac_x = frac_x*frac_x*(3-2*frac_x);

    return glm::mix(glm::mix(A, B, frac_x), glm::mix(C, D, frac_x), frac_y);
}

glm::vec3 TerrainShape::getPosition(int row, int col)
{
    glm::vec3 position;
    position.x = (float)row/m_param1 - 0.5;

    position.y = 0.3*TerrainShape::getY(row, col, 20.);
    position.y += 0.1*TerrainShape::getY(row, col, 12.);
    position.y += 0.3*TerrainShape::getY(row, col, 5.);
    position.y += 0.4*TerrainShape::getY(row, col, 2.);
//    position.y = 1-std::exp(-position.y);

    position.z = (float)col/m_param2 - 0.5;

    return position;
}



void TerrainShape::Tesselate()
{
    glm::vec3 p1 = m_vertices[0];
    glm::vec3 p2 = m_vertices[1];
    glm::vec3 p3 = m_vertices[2];
    glm::vec3 p4 = m_vertices[3];

    std::vector<glm::vec3> mesh_vertices;
    std::vector<glm::vec3> mesh_normals;
    std::vector<glm::vec2> texture_uvs;

    glm::vec3 point;
    for(int i=0;i<m_param1+1;i++)
    {
        float step1 = (float)i/m_param1;
//        glm::vec3 start = p1*(1.f - step1 ) + p2*step1;
//        glm::vec3 end = p3*(1.f - step1 ) + p4*step1;
        for(int j=0;j<m_param2+1;j++)
        {
            float step2 = (float)j/m_param2;
//            point = start*(1.f - step2 ) + end*step2;

            mesh_vertices.push_back( getPosition(i, j) );
            mesh_normals.push_back( getNormal  (i, j) );
            texture_uvs.push_back(glm::vec2(step2, step1));
        }
    }
    LoadVBOInput(&m_vertexData, &mesh_normals, &mesh_vertices, &texture_uvs, false, m_param1, m_param2);
}


glm::vec3 TerrainShape::getNormal(int row, int col)
{
    std::vector<glm::vec3> neighbors;
    neighbors.push_back(getPosition(row-1, col-1));
    neighbors.push_back(getPosition(row-1, col));
    neighbors.push_back(getPosition(row-1, col+1));
    neighbors.push_back(getPosition(row, col+1));
    neighbors.push_back(getPosition(row+1, col+1));
    neighbors.push_back(getPosition(row+1, col));
    neighbors.push_back(getPosition(row+1, col-1));
    neighbors.push_back(getPosition(row, col-1));
    glm::vec3 center = getPosition(row, col);

    glm::vec3 norm;
    norm.x = 0;
    norm.y = 0;
    norm.z = 0;
    for(int i=0;i<9;i++)
    {
        glm::vec3 vec1 = neighbors[i%8]-center;
        glm::vec3 vec2 = neighbors[(i+1)%8]-center;
        norm += glm::cross(vec1, vec2);
    }

    return glm::normalize(norm);
}
