#include <iostream>

#include "includes/gl_include.h"
#include "input/input.hpp"
#include "engine/scene.hpp"
#include "engine/shaders/loadshaders.hpp"

engine::scene world;
GLuint vertexBuffer;
GLuint programID;

void initGL()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    programID = engine::loadshaders("src/engine/shaders/triangle.vert",
            "src/engine/shaders/triangle.frag");

}

void initWorld()
{
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
       -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
            g_vertex_buffer_data, GL_STATIC_DRAW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    glEnableVertexAttribArray(0);

    glBindAttribLocation(programID, 0, "vertexPosition_modelspace");

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
    );

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

    world.draw();

    glFlush();
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    input::reshape(w, h, world);
}

void keyboard(unsigned char key, int x, int y)
{
    input::keyDown(key, x, y, world);
}

void keyboardUp(unsigned char key, int x, int y)
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
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    int win_width = 1024;
    int win_height = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Project");

    glewExperimental = GL_TRUE;
    glewInit();

    initGL();
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
