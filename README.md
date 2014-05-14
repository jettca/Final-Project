Forward rendering and soft shadows
=============

by Jett Andersen

This is my final project for Columbia's computer graphics course (Spring 2014).
It features soft shadows and lambertian shading, and is my first attempt at
programming with modern OpenGL.

Contents
I.      Directions
II.     Features
III.    Implementation
IV.     Results
V.      Sources


I. Directions
wasd - Move camera (with strafing)
Arrow keys - Move camera (with rotating)
xz - Rotation meshes


II. Features
I implemented an interface for exploring a 3D world with three degrees of
freedom.  The world features soft shadows from multiple point light sources, and
surfaces with blinn phong shading.


III. Implementation
I implemented this program entirely with modern OpenGL 3 features, abandoning
the fixed function pipeline with which we began the class.

My algorithm for shadow mapping is slightly unique, leading to some strange
behavior.  I implemented it with simple orward rendering: for each light source,
I render the shadowmap from its perspective, render the scene as if that light
were the only light, and then add my results for each light.  The unique part is
in rendering the shadowmap.  The space that is visible to a point light is
typically recorded in a cubemap, treating the world around it as lying on a
cube.  I decided to record the point light's view in a single texture that
treated the surrounding world as if it were on a sphere.  To do this, I
converted each x, y, z point relative to the light to polar coordinates, and
stored the rho value at the texture UV coordinates (theta, phi).

I achieved soft shadows using a simple percentage-closer filter that sampled
depth values locally, averaged the boolean mask that represented whether or not
a pixel was visible to the light, and scaled the shading by that average.


IV. Results
My results are clearly soft shadows, albeit with some aliassing problems due to
the shadowmap's resolution.  My radial method produces interesting artifiacts
that, although detracting from the quality of the result, did give me a much
better understanding of rasterization.  The first of these is that a few rare
angles cause a strange pattern of triangles to be shaded, and is due to
wraparound errors when the triangle crosses a "seam" on my projection, and the
texture samples incorrect, usually empty, values within the triangle.  The
second is that some viewing angles show light slipping through the mesh, and I
believe this is due to linear rasterization not mixing nicely with a nonlinear
texture projection.  As a result, the center of large triangles can seem to
curve through small triangles from the perspective of the fragment shader.


V. Sources
For learning modern OpenGL, I used the tutorials located at opengl-tutorial.org.
They do have a section on shadowmapping, but this was not useful for me, given
that I was interested in point light shadow mapping and soft shadows.

I learned about percentage closer filtering from the paper located in the
Sources directory titled ReevesPCF.pdf.
