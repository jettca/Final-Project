#version 150

in vec3 vertexPosition_modelspace;
in vec3 vertexNormal;

uniform vec3 lightPosition_modelspace;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 normalTransform;

out vec3 fragNormal;
out vec3 lightDir;
out vec3 halfViewDir;

void main()
{
    vec4 pos_modelspace = vec4(vertexPosition_modelspace, 1);
    vec4 pos_cameraspace = V*M*pos_modelspace;

    fragNormal = normalize(normalTransform*vertexNormal);
    lightDir = normalize(lightPosition_modelspace - vertexPosition_modelspace);
    halfViewDir = normalize(lightDir - normalize(pos_cameraspace).xyz);

    gl_Position = P*pos_cameraspace;
}
