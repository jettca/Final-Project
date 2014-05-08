#include <vector>
#include <string>

#include "includes/glm_include.hpp"

namespace engine
{
    class mesh
    {
        public:
            mesh();
            mesh(std::string filepath);

            void draw();

            void translate(glm::vec3 delta);
            void rotate(float angle, glm::vec3 axis);

        private:
            std::vector<glm::mat3> faces;
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec2> texture_uvs;
            std::vector<glm::vec3> normals;

            glm::vec3 diffuse, ambient;

            glm::mat4 modelMatrix;

            void loadMTL(std::string mtlpath);
    };
}
