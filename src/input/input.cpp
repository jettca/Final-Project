#include "input/input.hpp"
#include "includes/gl_include.h"

void input::reshape(int w, int h, engine::scene &s)
{
}

void input::keyDown(int key, int x, int y, engine::scene &s)
{
    s.setKey(key);
}

void input::keyUp(int key, int x, int y, engine::scene &s)
{
    s.unsetKey(key);
}

void input::specialDown(int key, int x, int y, engine::scene &s)
{
    s.setSpecial(key);
}

void input::specialUp(int key, int x, int y, engine::scene &s)
{
    s.unsetSpecial(key);
}
