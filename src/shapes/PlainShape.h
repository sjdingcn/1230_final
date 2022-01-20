#ifndef PlainShape_H
#define PlainShape_H

#include "BaseShape.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}

class PlainShape : public BaseShape
{
public:
    PlainShape();
    PlainShape(int param1, int param2);
    ~PlainShape();

private:
    virtual void CreateVertices();
    virtual void Tesselate();
    int m_param1;
    int m_param2;

    std::vector<glm::vec3> m_vertices;
    int m_face[4];
};

#endif // PlainShape_H
