#version 150

#define M_PI 3.1415926535897932384626433832795

in vec3 vertexPosition_modelspace;

uniform vec3 lightPosition_modelspace;
uniform float shadowmapDepth;

out float depth;

void main()
{
    vec3 pos = vertexPosition_modelspace - lightPosition_modelspace;

    float rho = length(pos);
    float phi = 2*atan(pos.y/(pos.x + rho));
    float theta = acos(pos.z/rho);
    depth = rho/shadowmapDepth;
    gl_Position = vec4(phi/M_PI, 2*theta/M_PI - 1.0f, rho/shadowmapDepth, 1);
}
