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
            scene(std::vector<std::string> mesh_paths,
                    std::vector<glm::mat4> modelMatrices,
                    std::vector<light> lights,
                    glm::mat4 projectionMatrix,
                    glm::mat4 camRotation,
                    glm::vec3 camPosition,
                    int window_width, int window_height);

            void draw();
            void update();

            void setKey(int key);
            void setSpecial(int key);
            void unsetKey(int key);
            void unsetSpecial(int key);

            int window_width, window_height;

        private:
            std::vector<mesh> meshes;
            std::vector<light> lights;

            glm::mat4 projectionMatrix;
            glm::mat4 viewMatrix();
            glm::vec4 invCamPosition;
            glm::mat4 invCamRotation;

            std::set<int> keysDown;
            std::set<int> specialsDown;
    };
}

#endif  // ifndef __SCENE_HPP__
