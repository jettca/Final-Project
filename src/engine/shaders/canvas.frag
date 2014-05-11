#version 150

in vec2 UV;

uniform sampler2D sceneTexture;

out vec3 color;

void main()
{
    color = texture(sceneTexture, UV).rgb;
    color = vec3(color.r*UV.x, color.g*UV.y, color.b*length(UV));
}
