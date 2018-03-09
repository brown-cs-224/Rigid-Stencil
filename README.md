# Rigid-Stencil
Starter code for CS 224 Assignment 3

The only file you need to modify is `deformMesh.m`

There are two ways to interact with the code: running `testscript.m`, or running `testscript2.m`.

`testscript.m` will open a window displaying a mesh, allow you to pick two constrained vertices to keep in place, and then allow to pick a third vertex and a new position to which that vertex should move. It will then show (side-by-side with the original mesh) a static snapshot of the mesh deforms in response to these constraints.

`testscript2.m` will open a window displaying a mesh, and allow you pick two constrained vertices to keep in place. You can then click and drag any third vertex to interactively deform the mesh (this is how we generated the videos that we showed in class). Once you release the mouse button, the script is finished--if you want to deform the mesh again, you'll need to close the window and rerun the script.
