#include "engine/scene.hpp"
#include "engine/shaders/loadshaders.hpp"
#include <iostream>

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048
#define SHADOW_MAP_DEPTH 100

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
    shadowmapSize(),
    shadowFramebuffer(),
    shadowTexture(),
    sceneFramebuffer(),
    sceneTexture(),
    sceneDepthbuffer(),
    canvasPosBuffer(),
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
    shadowmapSize(glm::vec3(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, SHADOW_MAP_DEPTH)),
    shadowFramebuffer(),
    shadowTexture(),
    sceneFramebuffer(),
    sceneTexture(),
    sceneDepthbuffer(),
    canvasPosBuffer(),
    canvasProgramID()
{
    initShaders();
    initShadowBuffers();
    initSceneBuffers();
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
    shadowmapSize(),
    shadowFramebuffer(),
    shadowTexture(),
    sceneFramebuffer(),
    sceneTexture(),
    sceneDepthbuffer(),
    canvasPosBuffer(),
    canvasProgramID()
{
    initShaders();
    initShadowBuffers();
    initSceneBuffers();
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
    shadowmapSize = s.shadowmapSize;

    deleteGLData();

    initShaders();
    initShadowBuffers();
    initSceneBuffers();

    return *this;
}

scene::~scene()
{
    deleteGLData();
}

void scene::deleteGLData()
{
    glDeleteProgram(canvasProgramID);
    glDeleteTextures(1, &shadowTexture);
    glDeleteFramebuffers(1, &shadowFramebuffer);
    glDeleteTextures(1, &sceneTexture);
    glDeleteRenderbuffers(1, &sceneDepthbuffer);
    glDeleteFramebuffers(1, &sceneFramebuffer);
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

void scene::initShadowBuffers()
{
    // Set up depth texture
    glGenTextures(1, &shadowTexture);
    glBindTexture(GL_TEXTURE_2D, shadowTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowmapSize.x,
            shadowmapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set up framebuffer
    glGenFramebuffers(1, &shadowFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, shadowTexture, 0);
    glDrawBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Failed to set up shadowFramebuffer: " << status << "\n";
        exit(1);
    }
}

void scene::initSceneBuffers()
{
    // Set up texture
    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0,
            GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set up depthbuffer
    glGenRenderbuffers(1, &sceneDepthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, sceneDepthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
            windowWidth, windowHeight);

    // Set up framebuffer
    glGenFramebuffers(1, &sceneFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER, sceneDepthbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, sceneTexture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Failed to set up sceneFramebuffer: " << status << "\n";
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

    int numMeshes = meshes.size();
    int numLights = lights.size();

    for(int i = 0; i < numLights; i++)
    {
        // draw shadowmap for scene
        drawShadowmap(lights.at(i));

        // render scene to scene texture using shadowmap
        drawToTexture(lights.at(i));

        // Draw blended scene texture to screen
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        drawSceneToScreen();
        glDisable(GL_BLEND);
    }

    glFlush();
    glutSwapBuffers();
}

void scene::drawShadowmap(light l)
{
    // bind and clear depth texture
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFramebuffer);
    glViewport(0, 0, shadowmapSize.x, shadowmapSize.y);
    static const GLfloat clearDepth[1] = {1.0f};
    glClearBufferfv(GL_DEPTH, 0, clearDepth);

    // render mesh depths from light perspective
    int numMeshes = meshes.size();
    for(int i = 0; i < numMeshes; i++)
    {
        meshes.at(i).drawShadowmap(l, shadowmapSize);
    }
}

void scene::drawToTexture(light l)
{
    // bind and clear scene texture
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    glViewport(0, 0, windowWidth, windowHeight);
    static const GLuint clearColor[4] = {0, 0, 0, 0};
    static const GLfloat clearDepth[1] = {1.0f};
    glClearBufferuiv(GL_COLOR, 0, clearColor);
    glClearBufferfv(GL_DEPTH, 0, clearDepth);

    // render meshes to scene texture
    int numMeshes = meshes.size();
    for(int i = 0; i < numMeshes; i++)
    {
        meshes.at(i).draw(viewMatrix(), projectionMatrix, l, shadowTexture,
                shadowmapSize);
    }
}

void scene::drawSceneToScreen()
{
    // Prepare to draw to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);

    // Set canvas program and vertices
    glUseProgram(canvasProgramID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, canvasPosBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Load scene texture to draw on canvas
    GLuint textureLoc = glGetUniformLocation(canvasProgramID, "sceneTexture");
    glUniform1i(textureLoc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    // Clear depth buffer and draw
    glClear(GL_DEPTH_BUFFER_BIT);
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
    // Generate view matrix from camera position and rotation
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
