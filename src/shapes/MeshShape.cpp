#include "MeshShape.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

MeshShape::MeshShape()
{
}

MeshShape::MeshShape(std::string filename)
{
    /**
     * We build in vertex data for a cube, in this case they are handwritten.
     * You'll want to expand upon or refactor this code heavily to take advantage
     * of polymorphism, vector math/glm library, and utilize good software design
     *
     */


    FILE * file = std::fopen(filename.c_str(), "r");
    if( file == NULL ){
        std::cout<<"Impossible to open the file "<< filename.c_str() <<" !\n";
    }
    std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(vertex);

        }else if ( strcmp( lineHeader, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            temp_uvs.push_back(uv);
        }else if ( strcmp( lineHeader, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            temp_normals.push_back(normal);
        }else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2],
                                 &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
            if (matches != 12){
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[3]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[3]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[3]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
//    std::vector< glm::vec3 > vertices;
//    std::vector< glm::vec2 > uvs;
//    std::vector< glm::vec3 > normals;
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
        unsigned int vertexIndex = vertexIndices[i];
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];

        unsigned int normalIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];

        unsigned int uvIndex = vertexIndices[i];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];

        std::vector<float> new_points;
        new_points = {vertex.x, vertex.y, vertex.z,
                      normal.x, normal.y, normal.z,
                      uv.x, uv.y};

        m_vertexData.insert(m_vertexData.end(), new_points.begin(), new_points.end());
    }

//    LoadVBOInput(m_vertexData, &normals, &vertices, &uvs, false, param1, param2);
    initializeOpenGLShapeProperties();
}

MeshShape::~MeshShape()
{
}

void MeshShape::CreateVertices()
{

}


void MeshShape::Tesselate()
{

}
