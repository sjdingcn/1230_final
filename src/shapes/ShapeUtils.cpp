
#include "ShapeUtils.h"
#include <math.h>       /* sin */



bool use_texture = true;

Mesh2D::Mesh2D(std::vector<glm::vec3>* mesh, int len1) :
m_len(len1), m_mesh(mesh)
{}

Mesh2D::Mesh2D(std::vector<glm::vec2>* mesh, int len1) :
m_len(len1), m_mesh2(mesh)
{}


glm::vec3 Mesh2D::get(int i, int j)
{
    return m_mesh->at(i*m_len+j);
}

glm::vec2 Mesh2D::get2(int i, int j)
{
    return m_mesh2->at(i*m_len+j);
}




void PushVec3(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
              glm::vec3 n1, glm::vec3 n2, glm::vec3 n3,
              glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
              std::vector<GLfloat>* m_vertexData)
{
    glm::vec3 p_data[] = {p1, p2, p3};
    glm::vec3 n_data[] = {n1, n2, n3};
    glm::vec2 uv_data[] = {uv1, uv2, uv3};
    for (int i=0;i<3;i++)
    {
        std::vector<float> new_points;
        new_points = {p_data[i].x, p_data[i].y, p_data[i].z,
                      n_data[i].x, n_data[i].y, n_data[i].z,
                      uv_data[i].x, uv_data[i].y};
        m_vertexData->insert(m_vertexData->end(), new_points.begin(), new_points.end());
    }
}




void LoadVBOInput(std::vector<GLfloat>* m_vertexData,
                  std::vector<glm::vec3>* normals,
                  std::vector<glm::vec3>* mesh_vetices,
                  std::vector<glm::vec2>* texture_uvs,
                  bool same_norm, int param1, int param2)
{
    glm::vec3 p1_, p2_, p3_, p4_, n1_, n2_, n3_, n4_, norm;
    glm::vec2 uv1_, uv2_, uv3_, uv4_;

    Mesh2D mesh(mesh_vetices, param2+1);
    Mesh2D norms(normals, param2+1);
    Mesh2D uvs(texture_uvs, param2+1);

    if (same_norm)
    {
        p1_ = mesh.get(0,0);
        p2_ = mesh.get(0,1);
        p3_ = mesh.get(1,0);
        p4_ = mesh.get(1,1);
        norm = glm::normalize(glm::cross(p3_-p1_, p2_-p1_));
    }

    for(int i=0;i<param1;i++)
    {
        for(int j=0;j<param2;j++)
        {
            p1_ = mesh.get(i,j);
            p2_ = mesh.get(i,j+1);
            p3_ = mesh.get(i+1,j);
            p4_ = mesh.get(i+1,j+1);
            n1_ = norms.get(i,j);
            n2_ = norms.get(i,j+1);
            n3_ = norms.get(i+1,j);
            n4_ = norms.get(i+1,j+1);
            uv1_ = uvs.get2(i,j);
            uv2_ = uvs.get2(i,j+1);
            uv3_ = uvs.get2(i+1,j);
            uv4_ = uvs.get2(i+1,j+1);

            if (!(p1_.x==p2_.x && p1_.y==p2_.y && p1_.z==p2_.z))
            {
                if(!same_norm)
                {
                    PushVec3(p3_, p2_, p1_, n3_, n2_, n1_, uv3_, uv2_, uv1_,
                             m_vertexData);
                }
                else
                {
                    PushVec3(p3_, p2_, p1_, norm, norm, norm, uv3_, uv2_, uv1_,
                             m_vertexData);
                }

            }

            if (!(p3_.x==p4_.x && p3_.y==p4_.y && p3_.z==p4_.z))
            {
                if(!same_norm)
                {
                    PushVec3(p2_, p3_, p4_, n2_, n3_, n4_, uv2_, uv3_, uv4_,
                             m_vertexData);
                }
                else
                {
                    PushVec3(p2_, p3_, p4_, norm, norm, norm, uv2_, uv3_, uv4_,
                             m_vertexData);
                }
            }

        }
    }
}


void TesselateRectangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4,
                        std::vector<GLfloat>* m_vertexData, bool same_norm,
                        int param1, int param2)
{
//    glm::vec3 mesh_vetices[param1+1][param2+1];
    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec2> texture_uvs;

    for(int i=0;i<param1+1;i++)
    {
        float step1 = (float)i/param1;
        glm::vec3 start = p1*(1.f - step1 ) + p2*step1;
        glm::vec3 end = p3*(1.f - step1 ) + p4*step1;
        for(int j=0;j<param2+1;j++)
        {
            float step2 = (float)j/param2;
            mesh_vetices.push_back(start*(1.f - step2 ) + end*step2);
            texture_uvs.push_back(glm::vec2(step2, step1));
        }
    }
    // in rectangle same_norm is always true, so just pass any pointer to *normals
    LoadVBOInput(m_vertexData, &mesh_vetices, &mesh_vetices, &texture_uvs, same_norm, param1, param2);
}


inline glm::vec3 SmoothNorm(glm::vec3 center, std::vector<glm::vec3>* neighbors, int offset)
{
    glm::vec3 norm;
    norm.x = 0.f;
    norm.y = 0.f;
    norm.z = 0.f;
    int len = neighbors->size();
    int base_offset = (len==8);
    for(int i=offset;i<len-1+base_offset+offset;i++)
    {
        glm::vec3 vec1 = neighbors->at((i+len)%len)-center;
        glm::vec3 vec2 = neighbors->at((i+1)%len)-center;
        if (glm::length(vec1-vec2)<0.00001) continue;
        norm += glm::normalize(glm::cross(vec2, vec1));
    }
    return glm::normalize(norm);
}

void CalcNormals(std::vector<glm::vec3>* mesh_vetices,
                 std::vector<glm::vec3>* normals,
                 int param1, int param2)
{
    for(int i=0;i<param1+1;i++)
    {
        for(int j=0;j<param2+1;j++)
        {
            int len2 = param2+1;
            glm::vec3 cur = mesh_vetices->at(i*len2+j%param2);
            std::vector<glm::vec3> neighbors;
            if (i>0 )
            {
                neighbors.push_back(mesh_vetices->at((i-1)*len2+(j-1+param2)%param2));
                neighbors.push_back(mesh_vetices->at((i-1)*len2+j%param2));
                neighbors.push_back(mesh_vetices->at((i-1)*len2+(j+1)%param2));
            }
            neighbors.push_back(mesh_vetices->at(i*len2+(j+1)%param2));
            if (i<param1)
            {
                neighbors.push_back(mesh_vetices->at((i+1)*len2+(j+1)%param2));
                neighbors.push_back(mesh_vetices->at((i+1)*len2+j%param2));
                neighbors.push_back(mesh_vetices->at((i+1)*len2+(j-1+param2)%param2));
            }
            neighbors.push_back(mesh_vetices->at(i*len2+(j-1+param2)%param2));

            normals->push_back(SmoothNorm(cur, &neighbors, (i<param1)-1));
        }
    }
}


void TesselateCone(glm::vec3 center, float height, float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2)
{
    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_uvs;

    glm::vec3 top = center;
    glm::vec3 foot = center;
    glm::vec3 normal;
    float divide = 1;

    if(height==0) normal.y = (radius2-radius1)/abs(radius2-radius1);
    else{
       divide = sqrt((radius2-radius1)*(radius2-radius1)/(height*height)+1);
       normal.y = (radius2-radius1)/height/divide;
    }

    top.y += height;
    float angle;
    float step1;

    for(int i=0;i<param1+1;i++)
    {
        step1 = (float)i/param1;

        for(int j=0;j<param2+1;j++)
        {
            angle = j==param2 ? 0: center.y/0.5*(float)j/param2*M_PI*2;

            top.x = cos(angle)*radius1;
            top.z = sin(angle)*radius1;
            foot.x = cos(angle)*radius2;
            foot.z = sin(angle)*radius2;

            if(height==0){
                normal.x = 0;
                normal.z = 0;
            }
            else {
                normal.x = cos(angle)/divide;
                normal.z = sin(angle)/divide;
            }

            mesh_vetices.push_back(top*(1.f - step1 ) + foot*step1);
            normals.push_back(normal);

            float r = radius1*(1.f - step1 ) + radius2*step1;
            if (height==0)
            {
                if (center.y>0)
                    texture_uvs.push_back(glm::vec2( r*cos(angle)+0.5, -r*sin(angle)+0.5 ));
                else
                    texture_uvs.push_back(glm::vec2( r*cos(angle)+0.5, r*sin(angle)+0.5 ));
            }
            else texture_uvs.push_back(glm::vec2( (float)j/param2, 1.f-step1 ));
        }
    }

    LoadVBOInput(m_vertexData, &normals, &mesh_vetices, &texture_uvs, same_norm, param1, param2);
}

void TesselateSphere(float degree1, float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2)
{
    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_uvs;

    float angle1;
    float angle2;

//    bool sphere_flag = degree1==180;
    degree1 = degree1/180*M_PI;
    for(int i=0;i<param1+1;i++)
    {
        if (degree1 == 360)
        {
            angle1 = i==param1 ? 0: (float)i/param1*degree1;
        }
        else
        {
            angle1 = (float)i/param1*degree1;
        }


        for(int j=0;j<param2+1;j++)
        {
            angle2 = j==param2 ? 0: -(float)j/param2*M_PI*2;
            glm::vec3 tmp;
            tmp.x = (radius1+radius2*sin(angle1))*cos(angle2);
            tmp.y = radius2*cos(angle1);
            tmp.z = (radius1+radius2*sin(angle1))*sin(angle2);

            glm::vec3 normal;
            normal.x = radius2*sin(angle1)*cos(angle2);
            normal.y = radius2*cos(angle1);
            normal.z = radius2*sin(angle1)*sin(angle2);

            mesh_vetices.push_back(tmp);
            normals.push_back(glm::normalize(normal));
            texture_uvs.push_back(glm::vec2((float)j/param2, 1.f-(float)i/param1));
        }
    }


    LoadVBOInput(m_vertexData, &normals, &mesh_vetices, &texture_uvs, same_norm, param1, param2);
}

void TesselateSpike(glm::vec3 center, glm::vec3 top,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2)
{
    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_uvs;

    glm::vec3 foot;
    glm::vec3 base1, base2, reference;
    reference = top-center;
    float angle;
    float step1;

    base1 = -reference;
    float len_proj;
    if (!(reference.x == 0 && reference.z == 0) )
    {
        len_proj = sqrt(reference.x*reference.x + reference.z*reference.z);
        if(reference.y==0) base1 = glm::vec3(0,1,0);
        else base1.y = len_proj*len_proj/reference.y;
    }
    else if (!(reference.x == 0 && reference.y == 0))
    {
        len_proj = sqrt(reference.x*reference.x + reference.y*reference.y);
        if(reference.z==0) base1 = glm::vec3(0,0,1);
        else base1.z = len_proj*len_proj/reference.z;
    }
    else
    {
        len_proj = sqrt(reference.z*reference.z + reference.y*reference.y);
        if(reference.x==0) base1 = glm::vec3(1,0,0);
        else base1.x = len_proj*len_proj/reference.x;
    }
    base1 = glm::normalize(base1);
    base2 = glm::normalize(glm::cross(reference, base1));
    base1 = base1*glm::length(reference)/20.f;
    base2 = base2*glm::length(reference)/20.f;


    for(int i=0;i<param1+1;i++)
    {
        step1 = (float)i/param1;

        for(int j=0;j<param2+1;j++)
        {
            angle = j==param2 ? 0: (float)j/param2*M_PI*2;
            foot = cos(angle)*base1 + sin(angle)*base2 + center;
            glm::vec3 tmp = top*(1.f - step1 ) + foot*step1;
            float step2 = sin(step1*step1*M_PI*2)*0.1;
            mesh_vetices.push_back(tmp*(1.f - step2 ) + center*step2);
            texture_uvs.push_back(glm::vec2((float)j/param2, 1.f-step1));
        }
    }

    if (!same_norm)
    {
        CalcNormals(&mesh_vetices, &normals, param1, param2);
    }

    LoadVBOInput(m_vertexData, &normals, &mesh_vetices, &texture_uvs, same_norm, param1, param2);
}


void TesselateMobius(float radius1, float radius2,
                   std::vector<GLfloat>* m_vertexData,
                   bool same_norm, int param1, int param2)
{
    std::vector<glm::vec3> mesh_vetices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texture_uvs;

    float angle1;
    float angle2;
    float step1;

    glm::vec3 top;
    glm::vec3 bottom;

    for(int i=0;i<param1+1;i++)
    {
        step1 = (float)i/param1;

        for(int j=0;j<param2+1;j++)
        {
            angle1 = j==param2 ? 0: -(float)j/param2*M_PI*4;
            angle2 = angle1/2.;


            top.x = (radius1+radius2*sin(angle2))*cos(angle1);
            top.y = radius2*cos(angle2);
            top.z = (radius1+radius2*sin(angle2))*sin(angle1);

            bottom.x = (radius1-radius2*sin(angle2))*cos(angle1);
            bottom.y = -radius2*cos(angle2);
            bottom.z = (radius1-radius2*sin(angle2))*sin(angle1);

            mesh_vetices.push_back(top*(1.f - step1 ) + bottom*step1);
            texture_uvs.push_back(glm::vec2((float)j/param2, step1));
        }
    }

    if (!same_norm)
    {
//        CalcNormals(&mesh_vetices, &normals, param1, param2);
        for(int i=0;i<param1+1;i++)
        {
            for(int j=0;j<param2+1;j++)
            {
                int len2 = param2+1;
                glm::vec3 cur = mesh_vetices.at(i*len2+j%param2);
                std::vector<glm::vec3> neighbors;
                if (i>0)
                {
                    if(j==0) neighbors.push_back(mesh_vetices.at((param1-i+1)*len2+(j-1+param2)%param2));
                    else neighbors.push_back(mesh_vetices.at((i-1)*len2+(j-1+param2)%param2));

                    neighbors.push_back(mesh_vetices.at((i-1)*len2+j%param2));

                    if(j==param2) neighbors.push_back(mesh_vetices.at((param1-i+1)*len2+(j+1)%param2));
                    else neighbors.push_back(mesh_vetices.at((i-1)*len2+(j+1)%param2));
                }
                if(j==param2) neighbors.push_back(mesh_vetices.at((param1-i)*len2+(j+1)%param2));
                else neighbors.push_back(mesh_vetices.at(i*len2+(j+1)%param2));
                if (i<param1)
                {
                    if(j==param2) neighbors.push_back(mesh_vetices.at((param1-i-1)*len2+(j+1)%param2));
                    else neighbors.push_back(mesh_vetices.at((i+1)*len2+(j+1)%param2));

                    neighbors.push_back(mesh_vetices.at((i+1)*len2+j%param2));

                    if(j==0) neighbors.push_back(mesh_vetices.at((param1-i-1)*len2+(j-1+param2)%param2));
                    else neighbors.push_back(mesh_vetices.at((i+1)*len2+(j-1+param2)%param2));
                }
                if(j==0) neighbors.push_back(mesh_vetices.at((param1-i)*len2+(j-1+param2)%param2));
                else neighbors.push_back(mesh_vetices.at(i*len2+(j-1+param2)%param2));


                normals.push_back(SmoothNorm(cur, &neighbors, (i<param1)-1 ));

            }
        }

    }

    LoadVBOInput(m_vertexData, &normals, &mesh_vetices, &texture_uvs, same_norm, param1, param2);
}
