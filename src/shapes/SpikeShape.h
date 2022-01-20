#ifndef SpikeShape_H
#define SpikeShape_H

#include "BaseShape.h"

// hand-written cube points and normals.. if only there were a way to do this procedurally
// data format {p1-1, n1, p1-2, n1, p1-3, n3.....}

struct vector_pair
{
    glm::vec3 a;
    glm::vec3 b;
};

class SpikeShape : public BaseShape
{
public:
    SpikeShape();
    SpikeShape(int param1, int param2);
    ~SpikeShape();

private:
    virtual void CreateVertices();
    virtual void Tesselate();
    int m_param1;
    int m_param2;


    glm::vec3 base[6];
};

#endif // SpikeShape_H
