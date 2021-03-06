#include "engine/mesh.hpp"
#include "engine/shaders/loadshaders.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

using namespace engine;

mesh::mesh() :
    vertices(),
    textureUVs(),
    normals(),
    diffuse(),
    ambient(),
    modelMatrix(),
    shadowProgramID(),
    renderProgramID(),
    vertexBuffer(),
    normalBuffer()
{}

mesh::mesh(std::string filepath, glm::mat4 modelMatrix) :
    vertices(),
    textureUVs(),
    normals(),
    diffuse(),
    ambient(),
    modelMatrix(modelMatrix),
    shadowProgramID(),
    renderProgramID(),
    vertexBuffer(),
    normalBuffer()
{
    loadMesh(filepath);
    
    initShaders();
    initBuffers();
}

mesh::mesh(const mesh& m) :
    vertices(m.vertices),
    textureUVs(m.textureUVs),
    normals(m.normals),
    diffuse(m.diffuse),
    ambient(m.ambient),
    modelMatrix(m.modelMatrix),
    shadowProgramID(),
    renderProgramID(),
    vertexBuffer(),
    normalBuffer()
{
    initShaders();
    initBuffers();
}

mesh& mesh::operator=(const mesh& m)
{
    vertices = m.vertices;
    textureUVs = textureUVs;
    normals = m.normals;
    diffuse = m.diffuse;
    ambient = m.ambient;
    modelMatrix = m.modelMatrix;

    glDeleteProgram(shadowProgramID);
    glDeleteProgram(renderProgramID);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &normalBuffer);

    initShaders();
    initBuffers();

    return *this;
}

mesh::~mesh()
{
    glDeleteProgram(shadowProgramID);
    glDeleteProgram(renderProgramID);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &normalBuffer);
}

void mesh::initShaders()
{
    // Rendering light depth map
    std::vector<std::string> inAttributes;
    std::vector<std::string> outAttributes;

    inAttributes.push_back("vertexPosition_modelspace");
    outAttributes.push_back("fragdepth");

    shadowProgramID = loadshaders("src/engine/shaders/pointlightmap.vert",
            "src/engine/shaders/pointlightmap.frag", inAttributes, outAttributes);

    // Rendering image with one light
    inAttributes.clear();
    outAttributes.clear();

    inAttributes.push_back("vertexPosition_modelspace");
    inAttributes.push_back("vertexNormal");
    outAttributes.push_back("color");

    renderProgramID = loadshaders("src/engine/shaders/lambertian.vert",
            "src/engine/shaders/lambertian.frag", inAttributes, outAttributes);
}

void mesh::initBuffers()
{
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

void mesh::draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, light l,
        GLuint shadowTexture, glm::vec3 shadowmapSize)
{
    glUseProgram(renderProgramID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Load vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load vertex normals
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load light data
    GLuint lightMPosLoc = glGetUniformLocation(renderProgramID, "lightPosition_modelspace");
    GLuint lightPosLoc = glGetUniformLocation(renderProgramID, "lightPosition_worldspace");
    GLuint lightDiffuseLoc = glGetUniformLocation(renderProgramID, "lightDiffuse");
    GLuint lightSpecularLoc = glGetUniformLocation(renderProgramID, "lightSpecular");
    glm::vec4 lightmpos = glm::inverse(modelMatrix)*glm::vec4(l.position, 1);
    glUniform3fv(lightMPosLoc, 1, &lightmpos[0]);
    glUniform3fv(lightPosLoc, 1, &l.position[0]);
    glUniform3fv(lightDiffuseLoc, 1, &l.diffuse[0]);
    glUniform3fv(lightSpecularLoc, 1, &l.specular[0]);

    // Load shadow texture and shadowmap size
    GLuint shadowmapLoc = glGetUniformLocation(renderProgramID, "shadowmap");
    glUniform1i(shadowmapLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    GLuint shadowSizeLoc = glGetUniformLocation(renderProgramID, "shadowmapSize");
    glUniform3fv(shadowSizeLoc, 1, &shadowmapSize[0]);

    // Load M, V, and P
    GLuint modelLoc = glGetUniformLocation(renderProgramID, "M");
    GLuint viewLoc = glGetUniformLocation(renderProgramID, "V");
    GLuint projectionLoc = glGetUniformLocation(renderProgramID, "P");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Load normal transform
    glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
    GLuint normalTransformID = glGetUniformLocation(renderProgramID, "normalTransform");
    glUniformMatrix3fv(normalTransformID, 1, GL_FALSE, &normalTransform[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mesh::drawShadowmap(light l, glm::vec3 shadowmapSize)
{
    glUseProgram(shadowProgramID);
    glEnableVertexAttribArray(0);

    // Load vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load light data
    GLuint lightMPosLoc = glGetUniformLocation(shadowProgramID, "lightPosition_modelspace");
    glm::vec4 lightmpos = glm::inverse(modelMatrix)*glm::vec4(l.position, 1);
    glUniform3fv(lightMPosLoc, 1, &lightmpos[0]);

    // Load shadowmap bounds
    GLuint depthID = glGetUniformLocation(shadowProgramID, "shadowmapDepth");
    glUniform1fv(depthID, 1, &shadowmapSize.z);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
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
    std::vector<glm::vec2> textureUVs_;
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
            textureUVs_.push_back(glm::vec2(u, v));
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
            textureUVs.push_back(textureUVs_.at(triangle[v][1]-1));
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
