#include "engine/mesh.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

using namespace engine;

mesh::mesh() :
    vertices(),
    texture_uvs(),
    normals(),
    diffuse(),
    ambient(),
    modelMatrix(),
    programID(),
    vertexBuffer(),
    normalBuffer()
{}

mesh::mesh(std::string filepath, glm::mat4 modelMatrix, GLuint programID) :
    vertices(),
    texture_uvs(),
    normals(),
    diffuse(),
    ambient(),
    modelMatrix(modelMatrix),
    programID(programID),
    vertexBuffer(),
    normalBuffer()
{
    loadMesh(filepath);

    // Load position buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
            &vertices[0], GL_STATIC_DRAW);

    // Load normal buffer
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
            &normals[0], GL_STATIC_DRAW);
}

void mesh::draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, light l)
{

    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Load vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load vertex normals
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load light data
    GLuint lightPosID = glGetUniformLocation(programID, "lightPosition_modelspace");
    GLuint lightDiffuseID = glGetUniformLocation(programID, "lightDiffuse");
    GLuint lightSpecularID = glGetUniformLocation(programID, "lightSpecular");
    glUniform3fv(lightPosID, 1, &l.position[0]);
    glUniform3fv(lightDiffuseID, 1, &l.diffuse[0]);
    glUniform3fv(lightSpecularID, 1, &l.specular[0]);

    // Load M, V, and P
    GLuint modelID = glGetUniformLocation(programID, "M");
    GLuint viewID = glGetUniformLocation(programID, "V");
    GLuint projectionID = glGetUniformLocation(programID, "P");
    glUniformMatrix4fv(modelID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(viewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionID, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Load normal transform
    glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    GLuint normalTransformID = glGetUniformLocation(programID, "normalTransform");
    glUniformMatrix3fv(normalTransformID, 1, GL_FALSE, &normalTransform[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mesh::translate(glm::vec3 delta)
{
    modelMatrix = glm::translate(glm::mat4(), delta) * modelMatrix;
}

void mesh::rotate(float angle, glm::vec3 axis)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}

void mesh::loadMesh(std::string filepath)
{
    // Temporary lists (to be organized)
    std::vector<glm::mat3> triangles;
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec2> texture_uvs_;
    std::vector<glm::vec3> normals_;

    std::ostringstream mtlpath;
    mtlpath << "static/";

    std::ifstream objfile(filepath.c_str());
    if(!objfile)
    {
        std::cout << filepath << " not found!\n";
        exit(1);
    }

    std::string line;
    while(getline(objfile, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if(type == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices_.push_back(glm::vec3(x, y, z));
        }
        else if(type == "vt")
        {
            float u, v;
            iss >> u >> v;
            texture_uvs_.push_back(glm::vec2(u, v));
        }
        else if(type == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals_.push_back(glm::vec3(x, y, z));
        }
        else if(type == "f")
        {
            glm::mat3 triangle;
            int v, vt, vn;
            char slash;
            for(int i = 0; i < 3; i++)
            {
                iss >> v >> slash >> vt >> slash >> vn;
                triangle[i] = glm::vec3(v, vt, vn);
            }
            triangles.push_back(triangle);
        }
        else if(type == "mtllib")
        {
            std::string temp;
            iss >> temp;
            mtlpath << temp;
        }
    }

    // Move temporary lists into organized member variables
    unsigned int numtriangles = triangles.size();
    for(int i = 0; i < numtriangles; i++)
    {
        glm::mat3 triangle = triangles.at(i);
        for(int v = 0; v < 3; v++)
        {
            vertices.push_back(vertices_.at(triangle[v][0]-1));
            texture_uvs.push_back(texture_uvs_.at(triangle[v][1]-1));
            normals.push_back(normals_.at(triangle[v][2]-1));
        }
    }

    loadMaterial(mtlpath.str());
}

void mesh::loadMaterial(std::string mtlpath)
{
    std::ifstream mtlfile(mtlpath.c_str());
    if(!mtlfile)
    {
        std::cout << mtlpath << " not found!\n";
        exit(1);
    }
    std::string line;
    while(getline(mtlfile, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if(type == "Kd")
        {
            iss >> diffuse.x >> diffuse.y >> diffuse.z;
        }
        else if(type == "Ka")
        {
            iss >> ambient.x >> ambient.y >> ambient.z;
        }
    }
}
