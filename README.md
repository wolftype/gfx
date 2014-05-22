gfx
===

header only generic graphics library (openGL and openGL es2.0)

cobbled together by pablo colapinto, likely includes code from wesley smith, graham wakefield, lance putnam and other members of the allosphere research group . . .
wolftype@gmail.com

file guide
==========
gfx_renderer.h: include this header for default configurations of immediate mode and advanced mode

 Renderer(screen pixel width, screen pixel height)
 Renderer::initGL( GL or GLES, IMMEDIATE MODE or VERTEX BUFFER MODE)
 
Renderer has:
  A Layout (for 1 or multiscreens)
  A Scene Matrix


 gfx_scene.h: 

util
===
GlutWindow.h: 
