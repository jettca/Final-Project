#include <iostream>

#include "input/input.hpp"
#include "engine/scene.hpp"
#include "engine/shaders/loadshaders.hpp"

engine::scene world;
GLuint vertexBuffer;

void initGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
}

void initWorld(int window_width, int window_height)
{
    std::vector<std::string> filepaths;
    filepaths.push_back("static/test_mesh.obj");

    std::vector<glm::mat4> modelMatrices;
    glm::mat4 init;
    init = glm::translate(init, glm::vec3(0.0f, 0.0f, -15.0f));
    init = glm::rotate(init, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrices.push_back(init);

    std::vector<engine::light> lights;
    lights.push_back(engine::light(glm::vec3(0, 5, 5),
                glm::vec3(1, 1, 1), glm::vec3(1, 1, 1)));

    glm::mat4 projection(glm::perspective(45.0f, float(window_width)/window_height,
                0.1f, 100.0f));
    glm::mat4 camRotation;
    glm::vec3 camPosition(0, 0, 0);

    world = engine::scene(filepaths, modelMatrices, lights, projection,
            camRotation, camPosition, window_width, window_height);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    int window_width = 1024;
    int window_height = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Project");

    glewExperimental = GL_TRUE;
    glewInit();

    initGL();
    initWorld(window_width, window_height);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    // glutIdleFunc(idle);

    glutMainLoop();

}
