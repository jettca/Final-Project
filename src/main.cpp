#include <iostream>

#include "includes/gl_include.h"
#include "input/input.hpp"
#include "scene/scene.hpp"


engine::scene world;

void initWorld();
{
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
}

void display()
{
    world.draw();
}

void reshape(int w, int h)
{
    input::reshape(w, h);
}

void keyboard(unsigned char key, int x, int y)
{
    input::keyDown(key, x, y, world);
}

void keyboard(unsigned char key, int x, int y)
{
    input::keyUp(key, x, y, world);
}

void special(int key, int x, int y)
{
    input::specialDown(key, x, y, world);
}

void specialUp(int key, int x, int y)
{
    input::specialUp(key, x, y, world);
}

void idle()
{
    world.update();
}

int main(int argc, char **argv)
{
    int win_width = 1024;
    int win_height = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Project");

    initWorld();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutIdleFunc(idle);

    glutMainLoop();
}
