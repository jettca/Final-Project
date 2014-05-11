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
    specialsDown(),
    window_width(),
    window_height(),
    texFramebuffer(),
    sceneTexture(),
    depthbuffer(),
    canvasPosBuffer(),
    renderProgramID(),
    canvasProgramID()
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
    window_height(window_height),
    texFramebuffer(),
    sceneTexture(),
    depthbuffer(),
    canvasPosBuffer(),
    renderProgramID(),
    canvasProgramID()
{
    int num_meshes = mesh_paths.size();
    meshes.resize(num_meshes);

    initShaders();

    for(int i = 0; i < num_meshes; i++)
    {
        meshes[i] = mesh(mesh_paths.at(i), modelMatrices[i], renderProgramID);
    }

    initTextureBuffers();
}

void scene::initShaders()
{
    // Rendering image with one light
    std::vector<std::string> in_attributes;
    in_attributes.push_back("vertexPosition_modelspace");
    in_attributes.push_back("vertexNormal");

    std::vector<std::string> out_attributes;
    out_attributes.push_back("color");

    renderProgramID = engine::loadshaders("src/engine/shaders/triangle.vert",
            "src/engine/shaders/triangle.frag", in_attributes, out_attributes);

    // Rendering texture to screen
    in_attributes.clear();
    out_attributes.clear();

    in_attributes.push_back("canvasVertPos");
    canvasProgramID = loadshaders("src/engine/shaders/canvas.vert",
            "src/engine/shaders/canvas.frag", in_attributes, out_attributes);
}

void scene::initTextureBuffers()
{
    // Set up textures
    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set up depthbuffer
    glGenRenderbuffersEXT(1, &depthbuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
            window_width, window_height);

    // Set up framebuffer
    glGenFramebuffersEXT(1, &texFramebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texFramebuffer);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, depthbuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
            GL_TEXTURE_2D, sceneTexture, 0);


    GLenum drawbuffers[1] = {GL_COLOR_ATTACHMENT0_EXT};
    glDrawBuffers(1, drawbuffers);

    int status;
    if((status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) !=
            GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        std::cerr << "Failed to set up framebuffer: " << status << "\n";
        exit(1);
    }

    //  Set up canvas
    glGenBuffers(1, &canvasPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);

    static const GLfloat canvasPositions[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(canvasPositions), canvasPositions,
            GL_STATIC_DRAW);
}

void scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render meshes to texture
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, texFramebuffer);

    static const GLuint clearColor[4] = {0, 0, 0, 0};
    glClearBufferuiv(GL_COLOR, 0, clearColor);

    static const GLfloat clearDepth[1] = {1.0f};
    glClearBufferfv(GL_DEPTH, 0, clearDepth);

    glViewport(0, 0, window_width, window_height);

    int num_meshes = meshes.size();
    int num_lights = lights.size();
    for(int l = 0; l < num_lights; l++)
    {
        for(int m = 0; m < num_meshes; m++)
        {
            meshes.at(m).draw(viewMatrix(), projectionMatrix, lights.at(l));
        }
    }

    // Draw texture to screen
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(canvasProgramID);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    GLuint textureLoc = glGetUniformLocation(canvasProgramID, "sceneTexture");
    glUniform1i(textureLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(0);

    glFlush();
    glutSwapBuffers();
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
