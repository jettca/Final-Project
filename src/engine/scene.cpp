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
    windowWidth(),
    windowHeight(),
    texFramebuffer(),
    sceneTexture(),
    depthbuffer(),
    canvasPosBuffer(),
    renderProgramID(),
    canvasProgramID()
{}

scene::scene(std::vector<mesh> meshes,
        std::vector<light> lights,
        glm::mat4 projectionMatrix,
        glm::mat4 camRotation,
        glm::vec3 camPosition,
        int windowWidth, int windowHeight) :
    meshes(meshes),
    lights(lights),
    projectionMatrix(projectionMatrix),
    invCamRotation(glm::inverse(camRotation)),
    invCamPosition(glm::vec4(-camPosition, 1)),
    keysDown(),
    specialsDown(),
    windowWidth(windowWidth),
    windowHeight(windowHeight),
    texFramebuffer(),
    sceneTexture(),
    depthbuffer(),
    canvasPosBuffer(),
    renderProgramID(),
    canvasProgramID()
{
    initShaders();
    initBuffers();
}

scene::scene(const scene& s) :
    meshes(s.meshes),
    lights(s.lights),
    projectionMatrix(s.projectionMatrix),
    invCamRotation(s.invCamRotation),
    invCamPosition(s.invCamPosition),
    keysDown(s.keysDown),
    specialsDown(s.specialsDown),
    windowWidth(s.windowWidth),
    windowHeight(s.windowHeight),
    texFramebuffer(),
    sceneTexture(),
    depthbuffer(),
    canvasPosBuffer(),
    renderProgramID(),
    canvasProgramID()
{
    initShaders();
    initBuffers();
}

scene& scene::operator=(const scene& s)
{
    meshes = s.meshes;
    lights = s.lights;
    projectionMatrix = s.projectionMatrix;
    invCamRotation = s.invCamRotation;
    invCamPosition = s.invCamPosition;
    keysDown = s.keysDown;
    specialsDown = s.specialsDown;
    windowWidth = s.windowWidth;
    windowHeight = s.windowHeight;

    glDeleteProgram(canvasProgramID);
    glDeleteTextures(1, &sceneTexture);
    glDeleteRenderbuffers(1, &depthbuffer);
    glDeleteFramebuffers(1, &texFramebuffer);
    glDeleteBuffers(1, &canvasPosBuffer);

    initShaders();
    initBuffers();

    return *this;
}

scene::~scene()
{
    glDeleteProgram(canvasProgramID);
    glDeleteTextures(1, &sceneTexture);
    glDeleteRenderbuffers(1, &depthbuffer);
    glDeleteFramebuffers(1, &texFramebuffer);
    glDeleteBuffers(1, &canvasPosBuffer);
}

void scene::initShaders()
{
    // Rendering texture to screen
    std::vector<std::string> inAttributes;
    std::vector<std::string> outAttributes;

    inAttributes.push_back("canvasVertPos");
    canvasProgramID = loadshaders("src/engine/shaders/canvas.vert",
            "src/engine/shaders/canvas.frag", inAttributes, outAttributes);
}

void scene::initBuffers()
{
    // Set up textures
    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0,
            GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set up depthbuffer
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
            windowWidth, windowHeight);

    // Set up framebuffer
    glGenFramebuffers(1, &texFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, texFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, depthbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, sceneTexture, 0);


    GLenum drawbuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawbuffers);

    int status;
    if((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) !=
            GL_FRAMEBUFFER_COMPLETE)
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

void scene::loadMeshes(std::vector<std::string> meshPaths,
        std::vector<glm::mat4> modelMatrices)
{
    int oldNumMeshes = meshes.size();
    int newNumMeshes = oldNumMeshes + meshPaths.size();
    meshes.resize(newNumMeshes);

    for(int i = oldNumMeshes; i < newNumMeshes; i++)
    {
        meshes[i] = mesh(meshPaths.at(i), modelMatrices[i]);
    }
}

void scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int num_meshes = meshes.size();
    int num_lights = lights.size();

    for(int l = 0; l < num_lights; l++)
    {

        // Render meshes to texture
        glBindFramebuffer(GL_FRAMEBUFFER, texFramebuffer);
        glViewport(0, 0, windowWidth, windowHeight);
        static const GLuint clearColor[4] = {0, 0, 0, 0};
        static const GLfloat clearDepth[1] = {1.0f};
        glClearBufferuiv(GL_COLOR, 0, clearColor);
        glClearBufferfv(GL_DEPTH, 0, clearDepth);

        for(int m = 0; m < num_meshes; m++)
        {
            meshes.at(m).draw(viewMatrix(), projectionMatrix, lights.at(l));
        }

        // Draw texture to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        glUseProgram(canvasProgramID);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        GLuint textureLoc = glGetUniformLocation(canvasProgramID, "sceneTexture");
        glUniform1i(textureLoc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);

        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(0);
        glDisable(GL_BLEND);
    }

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
