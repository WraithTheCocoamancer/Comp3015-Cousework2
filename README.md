# Comp3015-Cousework2
Coursework two for comp3015 Open GL Graphics pipeline module where we are gonna work with geometry shaders and beyond
Write up for COMP3015 CW 2 Morgan Hill
=======
Visual studio and operating system
-----------

The project was made using Visual studio community 2019 on a windows 10 operating system.

-----------

How does it work

-----------


Project contains a rendered Plane, torus, teapot and a pigmesh plus three spotlights with one rotating. each have different colors and textures. Also featured are brick textured point sprites and a small particles fountain using a fire texture that is visable as the camera moves back and forth. there is also a faint fog in place 

there are 7 different shader files with 2 used for normal lighting. 3 used for the point sprites and 2 used for the Particle effects but confusingly named vertexanimation and vertexfrag due to some delerium and having attempted before that vertex animation.

--------------

How the code git and how should it be navigated
--------------
The code is put together in such a way where each light has it's own method for shading within the fragment shader. If someone wanted to navigate it they would only have to look in the vertex shader. fragment shader Scenebasic_uniform.cpp and it's respective header. 

Basic_Uniform.Frag and vert  are used for the blinnphong shading along with the main bulk of texturing 

For point sprites three shaders were made. Geometry_shader.geom
PointspriteFrag.frag and PointSpriteVert.vert this was done to keep things from clashing together as much 
VertexAnimation.vert and VertexFrag.frag as confusingly named as they are infact are used for particle fountains with most of the work done in the vertex shader for this

the Scenebasic_uniform.cpp has the main points of interest in the init where the uniforms for the lighting is set.  The Render where the models material uniform's and the spotlights position and direction are set as well as rendering in the models. The update method has a simple delta time timer to allow the spotlight to move we also have expanded from one compiled program to three! we now have
Prog,
pointspriteprog and
Animprog
 
In the cpp we also set a lot of uniforms for our particles aas well as generating and binding the many buffers needed for the sprites

Within the cpp we also have a new function called initBuffers where we Bindthe rest of the array buffers and input the data for them along with vertex arrays and attributes and set up the transform feedback

Within the Render along with the rendering of the models and mesh we also run the two pass system needed for the buffers

--------------
the media folder contains the pig mesh. and the texture folder with the textures
the shaders folder contains the shaders
other items from tutorials used for the knowledge to make this were within the helper folder
The Scenebasic_uniform.cpp was in source files along with the glad and main. scenebasic_uniform.h is in headers

_--------------

Link to youtube Video of discussion and presentation

---------------

https://www.youtube.com/watch?v=q4vCa3VqIDw

-------------
