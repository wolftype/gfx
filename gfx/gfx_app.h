/*
 * =====================================================================================
 *
 *       Filename:  gfx_app.h
 *
 *    Description:  a lightweight utility that takes window contexts as a template parameter
 *
 *                  Basic Usage: subclass this and define setup() and onDraw() methods;
 *                  
 *                  GFXApp will handle mouse and window event callbacks defined in gfx_control.h
 *
 *                  for fancier rendering (i.e. rendering to texture or blur etc)
 *                  overwrite the onFrame() and onRender() methods.
 *
 *                    onMouseDown(const Mouse& m)
 *                    onMouseDrag(const Mouse& m)
 *
 *                    etc.
 *
 *                  WINDOWCONTEXT template parameter must have:
 *                    System() (singleton method)
 *                    Initialize() function
 *                    Start() function
 *                    SwapBuffers() method
 *                    create(int w, int h) method
 *                    interace (a public member of type gfx::Interface)
 *
 *                    see util/glut_window.hpp and util/glfw_window.hpp as example context classes
 *                    that can be passed to this GfxApp class as a template parameter.
 *
 *        Version:  1.0
 *        Created:  05/29/2014 07:18:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  cuttlefish 
 *
 * =====================================================================================
 */

#include <stdio.h>

#include "gfx_lib.h"              //<- Graphics Libraries
#include "gfx_scene.h"            //<- Matrix transforms
#include "gfx_control.h"          //<- Event Handling
#include "gfx_sceneController.h"  //<- Matrix transforms user input
#include "gfx_objectController.h" //<- Objects in Memory Callbacks for interface
#include "gfx_nprocess.h"         //<- Graphics Pipeline Processes


namespace gfx {

template<class WINDOWCONTEXT>
struct GFXApp : 
public InputEventHandler,
public WindowEventHandler,
public GFXRenderNode 
{

  WINDOWCONTEXT mContext;
  WindowData& window() { return mContext.window(); }

  Scene scene;
  SceneController sceneController;
  ObjectController objectController;
  
  /*-----------------------------------------------------------------------------
   *  Pass in width and height of window, and any command line arguments
   *-----------------------------------------------------------------------------*/
  GFXApp(int w=800, int h=400, int argc = 0, char ** argv = NULL) :
  GFXRenderNode(w,h)  
  {

     /*-----------------------------------------------------------------------------
      *  Initialize Window and Callbacks
      *-----------------------------------------------------------------------------*/
      WINDOWCONTEXT::System -> Initialize();
      mContext.create(w,h);

      //add this application to list of listeners to window and input events
      mContext.interface.addWindowEventHandler(this);
      mContext.interface.addInputEventHandler(this); 

      /*-----------------------------------------------------------------------------
       *  Add SceneController and ObjectController Callbacks
       *-----------------------------------------------------------------------------*/
      //bind sceneController to scene and add as listener to input events 
      sceneController.scene(&scene);
      sceneController.io(&mContext.interface.io);
      mContext.interface.addInputEventHandler(&sceneController);

      //attach this application (io and scene) to objectController 
      objectController.io( &mContext.interface.io );  
      objectController.scene( &scene );
    
      //add object controller as listener to input and window events
      mContext.interface.addInputEventHandler(&objectController);
      mContext.interface.addWindowEventHandler(&objectController);

      /*-----------------------------------------------------------------------------
       *  Initialize GLEW and check for features
       *-----------------------------------------------------------------------------*/
      glewExperimental = true;
      GLenum glewError = glewInit();
      if (glewError != GLEW_OK){
       printf("glew init error\n%s\n", glewGetErrorString( glewError) );
     }

      if (GLEW_APPLE_vertex_array_object){
        printf("genVertexArrayAPPLE supported\n");
      } else if (GLEW_ARB_vertex_array_object){
        printf("genVertexArrays supported\n");
      }
            
  }


  /*-----------------------------------------------------------------------------
   *  User must define setup() in a subclass. setup() is called by App::start() method
   *-----------------------------------------------------------------------------*/
  virtual void setup() = 0;

  /*-----------------------------------------------------------------------------
   *  User must Define onDraw() in a subclass. onDraw() is called by onFrame() method;
   *-----------------------------------------------------------------------------*/
  virtual void onDraw() = 0;
 
  /*-----------------------------------------------------------------------------
   *  Starts Graphics Thread.  To be called from main()
   *-----------------------------------------------------------------------------*/
  void start(){
    setup();
    WINDOWCONTEXT::System -> Start(this);
  }
  
  /*-----------------------------------------------------------------------------
   *  Optional method for updating physics etc -- called onFrame();
   *-----------------------------------------------------------------------------*/
  virtual void update(){}

  /*-----------------------------------------------------------------------------
   *  Clear Window Contents
   *-----------------------------------------------------------------------------*/
  void clear(){
     mContext.setViewport();      
     glClearColor(.2,.2,.2,1);
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  
  /*-----------------------------------------------------------------------------
   *  Default Rendering (Shader ambivalent)
   *-----------------------------------------------------------------------------*/
  virtual void onFrame(){

     clear(); 
     update();

     scene.push();
      onRender();
     scene.pop();
    
     scene.step();
     //note: swapbuffers not called here because
     //this is just one of many potential windowEventHandler callbacks
     //swapbuffers is called by Interface::onDraw() after all eventhandlers have been called
  }

  virtual void onRender(){ onDraw(); }


  /*-----------------------------------------------------------------------------
   *  Destructor
   *-----------------------------------------------------------------------------*/
   ~GFXApp(){
     WINDOWCONTEXT::System -> Terminate();
   } 
  
  /*-----------------------------------------------------------------------------
   *  INPUT EVENT HANDLER METHODS
   *-----------------------------------------------------------------------------*/
  virtual void onMouseMove(const Mouse& m){ }
  virtual void onMouseDrag(const Mouse& m){ }
  virtual void onMouseDown(const Mouse& m){ }
  virtual void onMouseUp(const Mouse& m){ }
  virtual void onKeyDown(const Keyboard& k){}
  virtual void onKeyUp(const Keyboard& k){ }

  /*-----------------------------------------------------------------------------
   *  WINDOW EVENT HANDLER METHODS
   *-----------------------------------------------------------------------------*/
  virtual void onCreate(){ }
  virtual void onDestroy(){ }
  virtual void onResize(int w, int h){
   // scene.camera.lens.width() = w;
   // scene.camera.lens.height() = h;
    scene.resize(w,h);
  }

};


}//gfx::
