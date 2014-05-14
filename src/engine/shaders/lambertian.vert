/* Vertex shader for evaluating blinn phong
 * shading with a polar coordinate shadowmap
 */

#version 150

#define M_PI 3.1415926535897932384626433832795

// Vertex data
in vec3 vertexPosition_modelspace;
in vec3 vertexNormal;

// Light and shadow data
uniform vec3 lightPosition_modelspace;
uniform vec3 lightPosition_worldspace;
uniform vec3 shadowmapSize;

// Transformation data
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 normalTransform;

// Data for fragment shader
out vec3 fragNormal;
out vec3 lightDir;
out vec3 halfViewDir;
out vec3 shadowPos;

void main()
{
    vec4 pos_modelspace = vec4(vertexPosition_modelspace, 1);
    vec4 pos_worldspace = M*pos_modelspace;
    vec4 pos_cameraspace = V*pos_worldspace;

    fragNormal = normalize(normalTransform*vertexNormal);
    lightDir = normalize(lightPosition_worldspace - pos_worldspace.xyz);
    halfViewDir = normalize(lightDir - normalize(pos_cameraspace).xyz);

    vec3 pos_lightspace = vertexPosition_modelspace - lightPosition_modelspace;
    float rho = length(pos_lightspace);
    float phi = 2*atan(pos_lightspace.y/(pos_lightspace.x + rho));
    float theta = acos(pos_lightspace.z/rho);
    shadowPos = vec3(phi/M_PI, 2*theta/M_PI - 1.0f, rho/shadowmapSize.z);

    gl_Position = P*pos_cameraspace;
}
