#include "engine/mesh.hpp"

#include <sstream>
#include <fstream>

using namespace engine;

mesh::mesh() :
    vertices(),
    texture_uvs(),
    normals(),
    diffuse(),
    ambient()
{}

mesh::mesh(std::string filepath) :
    vertices(),
    texture_uvs(),
    normals(),
    diffuse(),
    ambient()
{
    std::ostringstream mtlpath;
    mtlpath << "static/";

    std::ifstream objfile(filepath.c_str());
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
            vertices.push_back(glm::vec3(x, y, z));
        }
        else if(type == "vt")
        {
            float u, v;
            iss >> u >> v;
            texture_uvs.push_back(glm::vec2(u, v));
        }
        else if(type == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }
        else if(type == "f")
        {
            glm::mat3 face;
            int v, vt, vn;
            char slash;
            for(int i = 0; i < 3; i++)
            {
                iss >> v >> slash >> vt >> slash >> vn;
                face[i] = glm::vec3(v, vt, vn);
            }
            faces.push_back(face);
        }
        else if(type == "mtllib")
        {
            std::string temp;
            iss >> temp;
            mtlpath << temp;
        }
    }

    loadMTL(mtlpath.str());
}

void mesh::loadMTL(std::string mtlpath)
{
    std::ifstream mtlfile(mtlpath.c_str());
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

void mesh::draw()
{
}

void mesh::translate(glm::vec3 delta)
{
    modelMatrix = glm::translate(modelMatrix, delta);
}

void mesh::rotate(float angle, glm::vec3 axis)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}
