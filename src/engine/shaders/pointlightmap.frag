
/* Fragment shader for generating a shadowmap
 * in polar coordinates using phi and theta
 * for U and V, and rho for depth
 */

#version 150

in float depth;

out float fragdepth;

void main()
{
    fragdepth = depth;
}
