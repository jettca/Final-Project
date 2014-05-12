#version 150

in vec2 UV;

uniform sampler2D sceneTexture;

out vec3 color;

void main()
{
    color = texture(sceneTexture, UV).rgb;
}
