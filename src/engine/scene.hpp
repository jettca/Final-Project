#include <vector>
#include <string>
#include <set>

#include "engine/mesh.hpp"
#include "includes/glm_include.hpp"

namespace engine
{
    class scene
    {
        public:
            scene();
            scene(std::vector<std::string> mesh_paths,
                    std::vector<glm::vec3> mesh_positions);

            void draw();
            void update();

            void setKey(int key);
            void setSpecial(int key);
            void unsetKey(int key);
            void unsetSpecial(int key);

        private:
            std::vector<mesh> meshes;
            glm::mat4 viewMatrix;

            std::set<int> keysDown;
            std::set<int> specialsDown;
    };
}
