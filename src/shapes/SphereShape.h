#ifndef SphereShape_H
#define SphereShape_H

#include "BaseShape.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}

class SphereShape : public BaseShape
{
public:
    SphereShape();
    SphereShape(int param1, int param2);
    ~SphereShape();

private:
    virtual void CreateVertices();
    virtual void Tesselate();
    int m_param1;
    int m_param2;

    glm::vec3 m_center;
};

#endif // SphereShape_H
