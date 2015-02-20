gfx
===

a simple header only generic graphics library (openGL and openGL es2.0)

features/goals:
---

* does not rely on glu (has own project and unproject)
* is context agnostic
* is self-contained math-wise (does not rely on glm)
* provides a simple scene graph model
* can be used stand alone or as a renderer in your own openGL projects
* has some helpful utilities for spatial coherence across multiple screens

see also cuttlefish project

license:
---
free to be for you and me. 
cobbled together by pablo colapinto, likely includes code from wesley smith, graham wakefield, lance putnam and other members of the allosphere research group . . .
please credit these folk if you copy into your own project.

email: gmail -> wolftype

build
---
  mkdir build
  cd build
  cmake ..
  make xEmpty

or

  cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/your/toolchain/file ..

to optionally add compiler flags in a toolchain file (see for instance toolchains/rpi.cmake for cross-compilating to the raspberry pi)
note that CMAKE_CXX_FLAGS cannot be set here, but must be set in the CMAKE file itself

examples
========
xEmpty.cpp uses the gfx_app class wrapper.
xCubeMap.cpp renders to a cubemap texture.

xGlut.cpp just makes a glut window
xGlutRenderer.cpp shows how to inject a render pipeline into a glut context

file guide
==========

data structures and basic math
---
gfx_matrix.h: lightweight vec3fs and matrix multiplication
gfx_scene.h:  for model view matrix transforms
gfx_mesh.h:   for mesh data structures

opengl
---
gfx_vbo.h:    simple wrapper around gl vertex buffer objects
gfx_mbo.h:    for mesh buffering to gpu (contains two vertex buffer and index buffer and mesh)
gfx_fbo.h:    simple wrapper around gl framebuffer objects
gfx_vao.h:    simple wrapper around gl vertex array objects
gfx_vattrib.h:  simple wrapper aroung gl vertex attributes
gfx_texture.h:  simple wrapper around gl texture2D

meta
---
gfx_render.h: include this header for default configurations of immediate mode and advanced mode

util
--
Context Managers for EGL, Glut, GLFW

