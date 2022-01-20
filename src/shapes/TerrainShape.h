#ifndef TerrainShape_H
#define TerrainShape_H

#include "BaseShape.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}

class TerrainShape : public BaseShape
{
public:
    TerrainShape();
    TerrainShape(int param1, int param2);
    ~TerrainShape();

private:
    virtual void CreateVertices();
    virtual void Tesselate();
    float getY(int row, int col, float octave);
    float randValue(int row, int col);
    glm::vec3 getPosition(int row, int col);
    glm::vec3 getNormal(int row, int col);
    int m_param1;
    int m_param2;

    std::vector<glm::vec3> m_vertices;
    int m_face[4];
};

#endif // TerrainShape_H
