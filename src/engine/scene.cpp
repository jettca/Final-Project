#include "engine/scene.hpp"
#include "engine/shaders/loadshaders.hpp"
#include <iostream>

using namespace engine;

scene::scene() :
    meshes(),
    lights(),
    projectionMatrix(),
    invCamRotation(),
    invCamPosition(glm::vec4(0, 0, 0, 0)),
    keysDown(),
    specialsDown()
{}

scene::scene(std::vector<std::string> mesh_paths,
        std::vector<glm::mat4> modelMatrices,
        std::vector<light> lights,
        glm::mat4 projectionMatrix,
        glm::mat4 camRotation,
        glm::vec3 camPosition) :
    meshes(),
    lights(lights),
    projectionMatrix(projectionMatrix),
    invCamRotation(glm::inverse(camRotation)),
    invCamPosition(glm::vec4(-camPosition, 1)),
    keysDown(),
    specialsDown()
{
    int num_meshes = mesh_paths.size();
    meshes.resize(num_meshes);

    std::vector<std::string> attributes;
    attributes.push_back("vertexPosition_modelspace");
    attributes.push_back("vertexNormal");
    GLuint programID = engine::loadshaders("src/engine/shaders/triangle.vert",
            "src/engine/shaders/triangle.frag", attributes);

    for(int i = 0; i < num_meshes; i++)
    {
        meshes[i] = mesh(mesh_paths.at(i), modelMatrices[i], programID);
    }
}

void scene::draw()
{
    int num_meshes = meshes.size();
    int num_lights = lights.size();
    for(int i = 0; i < num_meshes; i++)
    {
        for(int l = 0; l < num_lights; l++)
        {
            meshes.at(i).draw(viewMatrix(), projectionMatrix, lights.at(l));
        }
    }
}

void scene::update()
{
    // Move camera
    if(specialsDown.find(GLUT_KEY_RIGHT) != specialsDown.end())
    {
        invCamRotation = glm::rotate(invCamRotation, glm::radians(2.0f), glm::vec3(0, 1, 0));
    }
    if(specialsDown.find(GLUT_KEY_LEFT) != specialsDown.end())
    {
        invCamRotation = glm::rotate(invCamRotation, glm::radians(-2.0f), glm::vec3(0, 1, 0));
    }
    if(specialsDown.find(GLUT_KEY_UP) != specialsDown.end() ||
            keysDown.find('w') != keysDown.end())
    {
        invCamPosition += glm::inverse(invCamRotation)*glm::vec4(0, 0, 0.3, 0);
    }
    if(specialsDown.find(GLUT_KEY_DOWN) != specialsDown.end() ||
            keysDown.find('s') != keysDown.end())
    {
        invCamPosition += glm::inverse(invCamRotation)*glm::vec4(0, 0, -0.3, 0);
    }
    if(keysDown.find('d') != keysDown.end())
    {
        invCamPosition += glm::inverse(invCamRotation)*glm::vec4(-0.3, 0, 0, 0);
    }
    if(keysDown.find('a') != keysDown.end())
    {
        invCamPosition += glm::inverse(invCamRotation)*glm::vec4(0.3, 0, 0, 0);
    }

    // Rotate meshes
    if(keysDown.find('x') != keysDown.end())
    {
        int numMeshes = meshes.size();
        for(int i = 0; i < numMeshes; i++)
        {
            meshes[i].rotate(2.0f, glm::vec3(0, 1, 0));
        }
    }
    if(keysDown.find('z') != keysDown.end())
    {
        int numMeshes = meshes.size();
        for(int i = 0; i < numMeshes; i++)
        {
            meshes[i].rotate(-2.0f, glm::vec3(0, 1, 0));
        }
    }
}

glm::mat4 scene::viewMatrix()
{
    glm::mat4 shift;

    for(int i = 0; i < 3; i++)
    {
        shift[3][i] = invCamPosition[i];
    }

    return invCamRotation*shift;
}

void scene::setKey(int key)
{
    keysDown.insert(key);
}

void scene::unsetKey(int key)
{
    keysDown.erase(key);
}

void scene::setSpecial(int key)
{
    specialsDown.insert(key);
}

void scene::unsetSpecial(int key)
{
    specialsDown.erase(key);
}
