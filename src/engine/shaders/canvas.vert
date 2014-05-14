
/* Vertex shader for texturing a canvas
 * over the whole screen at z = 0
 */

#version 150

in vec3 canvasVertPos;

out vec2 UV;

void main()
{
    gl_Position = vec4(canvasVertPos, 1);
    UV = (canvasVertPos.xy + vec2(1.0, 1.0))/2;
}
