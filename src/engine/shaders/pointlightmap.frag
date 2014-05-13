#version 150

out float fragdepth;

void main()
{
    fragdepth = gl_FragCoord.z;
}
