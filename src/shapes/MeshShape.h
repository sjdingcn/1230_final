#ifndef MeshShape_H
#define MeshShape_H

#include "BaseShape.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}

class MeshShape : public BaseShape
{
public:
    MeshShape();
    MeshShape(std::string filename);
    ~MeshShape();

private:
    virtual void CreateVertices();
    virtual void Tesselate();
    int m_param1;
    int m_param2;

    std::vector<glm::vec3> m_vertices;
    std::vector<int> m_faces;
};

#endif // MeshShape_H
