#include "engine/light.hpp"

using namespace engine;

light::light() :
    position(),
    diffuse(),
    specular()
{}

light::light(glm::vec3 position, glm::vec3 diffuse, glm::vec3 specular) :
    position(position),
    diffuse(diffuse),
    specular(specular)
{}
