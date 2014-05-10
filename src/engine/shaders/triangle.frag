#version 150

in vec3 fragNormal;
in vec3 lightDir;
in vec3 halfViewDir;

uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

out vec3 color;

void main()
{
    float diffuse = dot(lightDir, fragNormal);
    float specular = pow(dot(fragNormal, halfViewDir), 100.);

    color = lightDiffuse*diffuse + lightSpecular*specular;
}
