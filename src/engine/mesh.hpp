#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>
#include <string>

#include "includes/glm_include.hpp"
#include "includes/gl_include.h"
#include "engine/light.hpp"

namespace engine
{
    class mesh
    {
        public:
            mesh();
            mesh(std::string filepath, glm::mat4 modelMatrix);
            mesh(const mesh& m);
            mesh& operator=(const mesh& m);
            ~mesh();

            void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, light l,
                    GLuint shadowTexture, glm::vec3 shadowmapSize);
            void drawShadowmap(light l, glm::vec3 shadowmapSize);

            void translate(glm::vec3 delta);
            void rotate(float angle, glm::vec3 axis);

        private:
            std::vector<glm::vec3> vertices;
            std::vector<glm::vec2> textureUVs;
            std::vector<glm::vec3> normals;
            glm::vec3 diffuse, ambient;
            glm::mat4 modelMatrix;

            GLuint shadowProgramID, renderProgramID;
            GLuint vertexBuffer, normalBuffer;

            // Constructor helpers
            void loadMesh(std::string filepath);
            void loadMaterial(std::string mtlpath);

            void initShaders();
            void initBuffers();
    };
}

#endif  // ifndef __MESH_HPP__
