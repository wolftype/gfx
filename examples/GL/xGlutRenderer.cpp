/*
 * =====================================================================================
 *
 *       Filename:  xGlutRenderer.cpp
 *
 *    Description:  demonstrates how to incorporate a GFXRenderer in your own loop
 *    
 *        Version:  1.0
 *        Created:  02/18/2015 12:27:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome  
 *
 * =====================================================================================
 */

#include "gfx_lib.h"
#include "gfx_render.h"

float width, height;

MBO circle;               //<-- A Mesh Buffer Object (defined in gfx_mbo.h)
Scene scene;              //<-- Model View Projection Matrix Transforms (defined in gfx_scene.h)


/*-----------------------------------------------------------------------------
 *  The Following Render Nodes are Defined in gfx_render.h
 *-----------------------------------------------------------------------------*/
GFXMeshNode meshNode;     //<-- A pointer to a mesh buffer object
GFXSceneNode sceneNode;   //<-- A pointer to a scene
GFXShaderNode shaderNode; //<-- A shader (has default settings)
GFXViewNode viewNode;     //<-- A viewport (or many)
GFXRenderNode renderNode; //<-- Root Node (pulls all upstream processes)


/*-----------------------------------------------------------------------------
 *  This Initialization function should be called AFTER an OpenGL context is created
 *-----------------------------------------------------------------------------*/
void initGraphicsObjects(){

  /*-----------------------------------------------------------------------------
   *  mesh information
   *-----------------------------------------------------------------------------*/
   circle = Mesh::Circle();

  /*-----------------------------------------------------------------------------
   *  scene matrix information
   *-----------------------------------------------------------------------------*/
  scene.camera.pos(0,0,5);     //<-- set camera position
  scene.resize(width,height);  //<-- set view width and height (pixels)

  sceneNode.scene( &scene );   //<-- attach pointer to scene to sceneNode
  meshNode.add( &circle );     //<-- attach pointer to mesh to meshnode

  /*-----------------------------------------------------------------------------
   *  pipeline: rendernode is a root node
   *            viewNode  
   *            shadernode binds shader and calls scenenode
   *            scenenode updates uniforms of bound shader and calls meshnode
   *            meshnode update attributes of bound shader and draws itself to screen
   *-----------------------------------------------------------------------------*/
  renderNode << viewNode << shaderNode << sceneNode << meshNode;
  //<------downstream                      upstream---------->//

  /*-----------------------------------------------------------------------------
   *  initialize starting at root (this will call init() on all attached in pipeline)
   *-----------------------------------------------------------------------------*/
  renderNode.init(100,100);

  //choose whether to render in immediate mode or programmable pipeline
  shaderNode.immediate(false);
  
  cout <<"GRAPHICS OBJECTS INITIALIZED"<<endl;

}

static void reshape(int w, int h){
  width  = w; 
  height = h;

  renderNode.resize(w,h);
}

// animate is a loop that is called by glutTimerFunc
static void animate(int){    
    //1. Update The Display (calls glutDisplayFunc, which calls draw)
    glutPostRedisplay();
    //2. Swap the Buffers
    glutSwapBuffers();
    //3. Call this function again . . .
    glutTimerFunc(10, animate, 1);
}


static void draw(void){

    //1. Set view port size and position
    glViewport(0,0,width,height);
    //2. Set the Clear Screen Background Color To Red
    glClearColor(1,0,0,1);
    //3. Clear the Screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    scene.push(shaderNode.immediate());

       renderNode.onRender();

    scene.pop(shaderNode.immediate());

}



int main(int argc, char * argv[] ){
   
    //1. Initialize Glut
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //2. Create A Window Context
    glutCreateWindow("glut");
    //3. Specify a Reshape Function (for whenever window is resized)
    glutReshapeFunc(reshape);
    //4. Reshape the window (calls the reshape function)
    glutReshapeWindow(640, 480);

    //3. Specify a Display Function (for whenever glutPostRedisplay is called)
    glutDisplayFunc(draw); 
    //4. Begin Animation Timer          
    glutTimerFunc(10,animate,1);

    glewExperimental = true;
    GLenum glewError = glewInit();
      if (glewError != GLEW_OK){
       printf("glew init error\n%s\n", glewGetErrorString( glewError) );
     }

    //5. Initialize Graphics Objects
    initGraphicsObjects();

    //4. Start The Main Loop
    glutMainLoop();  

    return 0;
}
