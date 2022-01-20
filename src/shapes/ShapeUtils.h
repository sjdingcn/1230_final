#ifndef SHAPEUTILS_H
#define SHAPEUTILS_H

#include <glm/glm.hpp>
#include "GL/glew.h"

#include<memory>
#include <vector>


class Mesh2D
{
public:
    Mesh2D(std::vector<glm::vec3>* mesh, int len1);
    Mesh2D(std::vector<glm::vec2>* mesh, int len1);
    virtual ~Mesh2D(){}
    glm::vec3 get(int i, int j);
    glm::vec2 get2(int i, int j);
private:
    int m_len;
    std::vector<glm::vec3>* m_mesh=NULL;
    std::vector<glm::vec2>* m_mesh2=NULL;
};

glm::vec3 SmoothNorm(glm::vec3 center, std::vector<glm::vec3>* neighbors, int offset);


void PushVec3(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 norm,
              std::vector<GLfloat>* m_vertexData);

void LoadVBOInput(std::vector<GLfloat>* m_vertexData,
                  std::vector<glm::vec3>* normals,
                  std::vector<glm::vec3>* mesh_vetices,
                  std::vector<glm::vec2>* texture_uvs,
                  bool same_norm, int param1, int param2);

inline glm::vec3 SmoothNorm(glm::vec3 center, std::vector<glm::vec3>* neighbors, int offset);

void CalcNormals(std::vector<glm::vec3>* mesh_vetices,
                 std::vector<glm::vec3>* normals,
                 int param1, int param2);

void TesselateRectangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4,
                        std::vector<GLfloat>* m_vertexData, bool same_norm,
                        int param1, int param2);

void TesselateCone(glm::vec3 center, float height, float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2);

void TesselateSphere(float degree1, float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2);

void TesselateSpike(glm::vec3 center, glm::vec3 top,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2);

void TesselateMobius(float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2);

#endif // SHAPEUTILS_H
