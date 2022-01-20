#include "SpikeShape.h"

SpikeShape::SpikeShape()
{
}

SpikeShape::SpikeShape(int param1, int param2) :
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
    m_param2 = std::max(m_param2,1);
    m_param2 = std::min(m_param2,5);
    CreateVertices();
    Tesselate();
    /**
     * Initialize the OpenGLShapeProperties so that the shape is ready to be drawn.
     * Note that this does not actually draw the shape, that happens in ShapesScene.cpp
     */
    initializeOpenGLShapeProperties();
}

SpikeShape::~SpikeShape()
{
}

void SpikeShape::CreateVertices()
{
    base[0] = glm::vec3(0, 1, 0);
    base[1] = glm::vec3(1, 0, 0);
    base[2] = glm::vec3(0, 0, 1);
    base[3] = glm::vec3(0, -1, 0);
    base[4] = glm::vec3(-1, 0, 0);
    base[5] = glm::vec3(0, 0, -1);
}


void SpikeShape::Tesselate()
{
    std::vector<vector_pair> nodes;
    std::vector<vector_pair> old_nodes;
    float len_spike = 1;
    int mesh_param1 = 10;
    int mesh_param2 = m_param1;

    for(int i=0;i<1;i++)
    {
        vector_pair start;
        start.a = glm::vec3(0,0,0);
        start.b = base[i];

        TesselateSpike(start.a, start.b, &m_vertexData,
                       false, mesh_param1, mesh_param2);
        nodes.push_back(start);
    }


    for(int t=0;t<m_param2-1;t++)
    {
        old_nodes = nodes;
        nodes.clear();
        len_spike = len_spike*0.707f;
        int len1 = old_nodes.size();

        mesh_param1 = std::max(mesh_param1/2,2);
        mesh_param2 = std::max(mesh_param2/2,5);
        for(int n=0;n<len1;n++)
        {
            vector_pair cur = old_nodes.at(n);
            glm::vec3 new_start = cur.a*0.707f + cur.b*0.293f;
            glm::vec3 reference = (cur.b - cur.a)*0.707f;
            for(int i=0;i<6;i++)
            {
                float pos_check = glm::dot(glm::normalize(reference), base[i]);
                if (pos_check<0 || pos_check==1) continue;
                vector_pair new_pair;
                new_pair.a = new_start;
                new_pair.b = new_start + (base[i]*len_spike+reference)*0.5f ;
                TesselateSpike(new_pair.a, new_pair.b, &m_vertexData,
                               false, mesh_param1, mesh_param2);
                nodes.push_back(new_pair);
            }
        }
    }

}
