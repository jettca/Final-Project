#include "engine/scene.hpp"

using namespace engine;

scene::scene() :
    meshes(),
    keysDown(),
    specialsDown(),
    viewMatrix()
{}

scene::scene(std::vector<std::string> mesh_paths, 
        std::vector<glm::vec3> mesh_positions) :
    meshes(),
    keysDown(),
    specialsDown(),
    viewMatrix()
{
    int num_meshes = mesh_paths.size();
    meshes.resize(num_meshes);
    for(int i = 0; i < num_meshes; i++)
    {
        meshes[i] = mesh(mesh_paths.at(i));
        meshes[i].translate(mesh_positions.at(i));
    }
}

void scene::draw()
{
    int num_meshes = meshes.size();
    for(int i = 0; i < num_meshes; i++)
    {
        meshes.at(i).draw();
    }
}

void scene::update()
{}

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
