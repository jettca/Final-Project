#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "includes/glm_include.hpp"

namespace engine
{
    class light
    {
        public:
            light();
            light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular);

            glm::vec3 position, diffuse, specular;
        private:
    };
}

#endif  // ifndef __LIGHT_HPP__
