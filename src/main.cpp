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

void initWorld(int windowWidth, int windowHeight)
{
    std::vector<engine::light> lights;
    lights.push_back(engine::light(glm::vec3(50.0f, 0.0f, -15.0f),
                glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)));

    lights.push_back(engine::light(glm::vec3(-50.0f, 0.0f, -25.0f),
                glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f)));

    glm::mat4 projection(glm::perspective(45.0f, float(windowWidth)/windowHeight,
                0.1f, 100.0f));
    glm::mat4 camRotation;
    glm::vec3 camPosition(0, 0, 0);

    world = engine::scene(std::vector<engine::mesh>(), lights, projection,
            camRotation, camPosition, windowWidth, windowHeight);

    std::vector<std::string> meshpaths;
    meshpaths.push_back("static/test_mesh.obj");
    meshpaths.push_back("static/test_mesh.obj");

    std::vector<glm::mat4> modelMatrices;
    glm::mat4 init;
    init = glm::translate(init, glm::vec3(0.0f, 0.0f, -15.0f));
    modelMatrices.push_back(init);
    init = glm::translate(glm::mat4(), glm::vec3(-10.0f, 0.0f, -18.0f));
    modelMatrices.push_back(init);

    world.loadMeshes(meshpaths, modelMatrices);
}

void display()
{
    world.draw();
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
    int windowWidth = 1024;
    int windowHeight = 512;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Project");

    glewExperimental = GL_TRUE;
    glewInit();

    initGL();
    initWorld(windowWidth, windowHeight);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutIdleFunc(idle);

    glutMainLoop();

}
