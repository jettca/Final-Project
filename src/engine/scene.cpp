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
        glm::vec3 camPosition,
        int window_width, int window_height) :
    meshes(),
    lights(lights),
    projectionMatrix(projectionMatrix),
    invCamRotation(glm::inverse(camRotation)),
    invCamPosition(glm::vec4(-camPosition, 1)),
    keysDown(),
    specialsDown(),
    window_width(window_width),
    window_height(window_height)
{ int num_meshes = mesh_paths.size();
    meshes.resize(num_meshes);

    std::vector<std::string> in_attributes;
    in_attributes.push_back("vertexPosition_modelspace");
    in_attributes.push_back("vertexNormal");

    std::vector<std::string> out_attributes;
    out_attributes.push_back("color");

    GLuint programID = engine::loadshaders("src/engine/shaders/triangle.vert",
            "src/engine/shaders/triangle.frag", in_attributes, out_attributes);

    for(int i = 0; i < num_meshes; i++)
    {
        meshes[i] = mesh(mesh_paths.at(i), modelMatrices[i], programID);
    }
}

void scene::draw()
{
    // Set up framebuffer
    GLuint framebuffer;
    glGenFramebuffersEXT(1, &framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);

    // Set up texture
    GLuint sceneTexture;
    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set up depth buffer
    GLuint depthbuffer;
    glGenRenderbuffersEXT(1, &depthbuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
            window_width, window_height);

    // Connect to framebuffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, depthbuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
            GL_TEXTURE_2D, sceneTexture, 0);

    // Set up draw buffer
    GLenum drawbuffers[1] = {GL_COLOR_ATTACHMENT0_EXT};
    glDrawBuffers(1, drawbuffers);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
    glViewport(0, 0, window_width, window_height);

    int status;
    if((status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) !=
            GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        std::cerr << "Failed to set up framebuffer: " << status << "\n";
        exit(1);
    }

    int num_meshes = meshes.size();
    int num_lights = lights.size();
    for(int l = 0; l < num_lights; l++)
    {
        for(int m = 0; m < num_meshes; m++)
        {
            meshes.at(m).draw(viewMatrix(), projectionMatrix, lights.at(l));
        }
    }

    glFlush();

    GLuint canvasArrayID;
    glGenVertexArrays(1, &canvasArrayID);
    glBindVertexArray(canvasArrayID);

    glEnableVertexAttribArray(0);

    static const GLfloat canvasPositions[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    GLuint canvasPosBuffer;
    glGenBuffers(1, &canvasPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(canvasPositions), canvasPositions,
            GL_STATIC_DRAW);

    std::vector<std::string> in_attributes;
    in_attributes.push_back("canvasVertPos");
    GLuint canvasProgramID = loadshaders("src/engine/shaders/canvas.vert",
            "src/engine/shaders/canvas.frag", in_attributes,
            std::vector<std::string>());

    glUseProgram(canvasProgramID);

    glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint textureLoc = glGetUniformLocation(canvasProgramID, "sceneTexture");
    glUniform1i(textureLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
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
