#ifndef __INPUT_HPP__
#define __INPUT_HPP__

#include "engine/scene.hpp"

namespace input
{
    void reshape(int w, int h, engine::scene &s);
    void keyDown(int key, int x, int y, engine::scene &s);
    void keyUp(int key, int x, int y, engine::scene &s);
    void specialDown(int key, int x, int y, engine::scene &s);
    void specialUp(int key, int x, int y, engine::scene &s);
}

#endif  // ifndef __INPUT_HPP__
