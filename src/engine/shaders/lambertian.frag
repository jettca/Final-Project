#version 150

in vec3 fragNormal;
in vec3 lightDir;
in vec3 halfViewDir;
in vec3 shadowPos;

uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform sampler2D shadowmap;

out vec3 color;

void main()
{
    float diffuse = dot(lightDir, fragNormal);
    float specular = pow(dot(fragNormal, halfViewDir), 100.);

    float depth = texture(shadowmap, (shadowPos.xy + 1)/2).r;

    float offset = .00005;
    if(depth + offset > shadowPos.z)
        color = lightDiffuse*diffuse + lightSpecular*specular;
    else
        color = vec3(0, 0, 0);
}
