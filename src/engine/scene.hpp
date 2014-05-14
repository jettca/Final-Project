#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <vector>
#include <string>
#include <set>

#include "engine/mesh.hpp"
#include "engine/light.hpp"
#include "includes/glm_include.hpp"

namespace engine
{
    class scene
    {
        public:
            scene();
            scene(std::vector<mesh> meshes,
                    std::vector<light> lights,
                    glm::mat4 projectionMatrix,
                    glm::mat4 camRotation,
                    glm::vec3 camPosition,
                    int windowWidth, int windowHeight);
            scene(const scene& s);
            scene& operator=(const scene& s);
            ~scene();

            /* Load meshes from filepaths with given initial
             * model matrices
             */
            void loadMeshes(std::vector<std::string> meshPaths,
                    std::vector<glm::mat4> modelMatrices);

            /* draw scene to screne using shadow mapping */
            void draw();

            /* update scene from user input */
            void update();

            /* set user input */
            void setKey(int key);
            void setSpecial(int key);
            void unsetKey(int key);
            void unsetSpecial(int key);

            int windowWidth, windowHeight;

        private:
            /* scene object data */
            std::vector<mesh> meshes;
            std::vector<light> lights;

            /* matrix data */
            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix();
            glm::vec4 invCamPosition;
            glm::mat4 invCamRotation;

            /* user input storage */
            std::set<int> keysDown;
            std::set<int> specialsDown;

            /* programs and buffers for rendering */
            GLuint canvasProgramID;
            GLuint shadowFramebuffer, shadowTexture;
            GLuint sceneFramebuffer, sceneTexture, sceneDepthbuffer;
            GLuint canvasPosBuffer;

            glm::vec3 shadowmapSize;

            /* constructor and destructor helpers */
            void initShaders();
            void initShadowBuffers();
            void initSceneBuffers();
            void deleteGLData();

            /* draw function helpers */
            void drawShadowmap(light l);
            void drawToTexture(light l);
            void drawSceneToScreen();
    };
}

#endif  // ifndef __SCENE_HPP__
