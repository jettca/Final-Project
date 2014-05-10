#include "engine/shaders/loadshaders.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

GLuint engine::loadshaders(std::string vertfile, std::string fragfile,
        std::vector<std::string> in_attributes,
        std::vector<std::string> out_attributes)
{
    /* init glew and load compiled shaders */
    glewInit();
    if(!glewIsSupported("GL_VERSION_2_0 GL_ARB_multitexture GL_EXT_framebuffer_object")) 
    {
        fprintf(stderr, "Required OpenGL extensions missing\n");
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    /* read shader files into strings */
    std::ifstream vfile(vertfile.c_str());
    std::ifstream ffile(fragfile.c_str());

    if(!vfile)
    {
        std::cout << vertfile << " does not exist!\n";
        exit(1);
    }
    if(!ffile)
    {
        std::cout << fragfile << " does not exist!\n";
        exit(1);
    }

    std::ostringstream vbuffer, fbuffer;
    vbuffer << vfile.rdbuf();
    fbuffer << ffile.rdbuf();
    
    std::string vertexString(vbuffer.str());
    std::string fragmentString(fbuffer.str());

    /* convert strings to c_strings and set as shader source */
    char *vertexCString, *fragmentCString;
    vertexCString = new char[vertexString.length() + 1];
    fragmentCString = new char[fragmentString.length() + 1];
    strcpy(vertexCString, vertexString.c_str());
    strcpy(fragmentCString, fragmentString.c_str());

    glShaderSource(vertexShader, 1, (const char**)&vertexCString, 0);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentCString, 0);

    delete vertexCString;
    delete fragmentCString;

    /* compile shaders and check for errors */
    GLint status;
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        char log[2048];
        int len;
        glGetShaderInfoLog(vertexShader, 2048, (GLsizei*)&len, log);
        fprintf(stderr, "Vertex Shader: %s", log);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        exit(1);
    }

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        char log[2048];
        int len;
        glGetShaderInfoLog(fragmentShader, 2048, (GLsizei*)&len, log);
        fprintf(stderr, "Fragment Shader: %s", log);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        exit(1);
    }

    /* create program, bind attributes, and link program */
    int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    int num_in_attributes = in_attributes.size();
    for(int i = 0; i < num_in_attributes; i++)
    {
        glBindAttribLocation(program, i, in_attributes.at(i).c_str());
    }
    int num_out_attributes = out_attributes.size();
    for(int i = 0; i < num_out_attributes; i++)
    {
        glBindFragDataLocation(program, i, out_attributes.at(i).c_str());
    }
    glLinkProgram(program);

    GLint pstatus;
    glGetProgramiv(program, GL_LINK_STATUS, &pstatus);
    if(pstatus != GL_TRUE)
    {
        char log[2048];
        int len;
        glGetProgramInfoLog(program, 2048, (GLsizei*)&len, log);
        fprintf(stderr, "%s", log);
        glDeleteProgram(program);
        return -1;
    }

    return program;
}
