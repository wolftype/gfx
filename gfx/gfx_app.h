/*
 * =====================================================================================
 *
 *       Filename:  gfx_app.h
 *
 *    Description:  an OpenGL utility that takes windowing contexts (e.g. Glut or GLFW) as a template parameter,
 *                  handles keyboard and mouse inputs, and gives two GL rendering modes (fixed or programmable)
 *
 *                  Basic Usage: subclass GFXApp and define setup() and onDraw() methods;
 *
 *                  struct App : GFXApp<GlutContext>{
 *
 *                    void setup(){
 *                      ...
 *                    }
 *
 *                    void onDraw(){
 *                      ...
 *                    }
 *                  
 *                  };
 *                  
 *
 *                  for fancier rendering (i.e. rendering to texture or blur etc)
 *                  overwrite the onFrame() and onRender() methods.
 *
 *                  GFXApp will handle mouse and window event callbacks defined in gfx_control.h
 * 
 *                    onMouseDown(const Mouse& m)
 *                    onMouseDrag(const Mouse& m)
 *
 *                    etc.
 *
 *                  WINDOWCONTEXT template parameter (e.g. GlutContext) must have:
 *                    System() (singleton method)
 *                        with an Initialize() method
 *                    Start() method
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
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */

#include <stdio.h>

#include "gfx_lib.h"              //<- Graphics Libraries
#include "gfx_scene.h"            //<- Matrix transforms (should rename this to gfx_modelViewProjection.h)
#include "gfx_control.h"          //<- Event Handling
#include "gfx_sceneController.h"  //<- Matrix transforms bound to user inputEvents
#include "gfx_objectController.h" //<- Objects in Memory bound to user inputEvents and windowEvents
#include "gfx_render.h"           //<- Graphics Pipeline Rendering Processes


namespace gfx {

template<class WINDOWCONTEXT>
struct GFXApp : 
public InputEventHandler,
public WindowEventHandler,
public GFXRenderNode                //< has onRender() method called by mRenderer 
{

  WINDOWCONTEXT mContext;
  WINDOWCONTEXT& context() { return mContext; }
  WindowData& windowData() { return mContext.windowData(); }

  Scene scene;                            ///< modelviewprojection matrix transforms
  GFXSceneRenderer mRenderer;              ///< shader pipeline

  SceneController sceneController;        ///< interface to matrix transforms 
  ObjectController objectController;      ///< interface to objects on screen

  Vec3f mColor;                           ///< Background Color

  GFXio& io() { return mContext.interface.io; } ///< get io

  /*-----------------------------------------------------------------------------
   *  Constructor: Optional to Pass in width and height of window, and any command line arguments
   *-----------------------------------------------------------------------------*/
  GFXApp(int w=800, int h=400, string name = "GFXApp", int argc = 0, char ** argv = NULL) :
  mColor(.2,.2,.2)
  {

     /*-----------------------------------------------------------------------------
      *  1. Initialize Window and Callbacks
      *-----------------------------------------------------------------------------*/
      WINDOWCONTEXT::System -> Initialize();
      mContext.create(w,h,name);

      //add this to window context's list of listeners to events
      mContext.interface.addWindowEventHandler(this);
      mContext.interface.addInputEventHandler(this); 

      /*-----------------------------------------------------------------------------
       * 2. Add SceneController and ObjectController Callbacks
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

      mContext.interface.OnResize(w,h);

      /*-----------------------------------------------------------------------------
       * 3.  Initialize GLEW and check for features (if not using GLES
       *-----------------------------------------------------------------------------*/

#ifndef GFX_USE_GLES
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
#endif
      /*-----------------------------------------------------------------------------
       * 5. Set up Programmable Rendering Pipeline
       *-----------------------------------------------------------------------------*/
       mRenderer.child(this); 
       mRenderer.scene(&scene);
       mRenderer.init();

      /*-----------------------------------------------------------------------------
       * 4. Enable Presets (depth func, blend func) see gfx_gl.h
       *-----------------------------------------------------------------------------*/
       GL::enablePreset();


  }


  /*-----------------------------------------------------------------------------
   *  User must define setup() in a subclass. setup() is called by App::start() method
   *-----------------------------------------------------------------------------*/
  virtual void setup() = 0;

  /*-----------------------------------------------------------------------------
   *  User must Define onDraw() in a subclass. onDraw() is called by onRender() method;
   *-----------------------------------------------------------------------------*/
  virtual void onDraw() = 0;

  template<class T>
  void draw(const T& t, float r=1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate=false){
    mRenderer.draw(t,r,g,b,a,bUpdate);
  }
 
  void draw(MBO& t, float r=1.0, float g=1.0, float b=1.0, float a=1.0){
    mRenderer.draw(t,r,g,b,a);
  }
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
      if (sceneController.io().mode( ControlMode::Edit ) ){
        mColor=Vec3f(.2,.2,.2);
      } else {
        mColor=Vec3f(.1,.1,.1);
      }

     mContext.setViewport();      
     glClearColor( mColor[0],mColor[1],mColor[2], 1 );
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  
  /*-----------------------------------------------------------------------------
   *  Default Rendering (Shader ambivalent)
   *-----------------------------------------------------------------------------*/
  virtual void onFrame(){

     GL::enablePreset();

     clear(); 
     update();

     scene.push( mRenderer.immediate() ); ///< push matrices

      mRenderer.enter();                      ///< bind shader and update shader uniforms
          onRender();                         ///< call this->onRender() method (defaults to this->onDraw())  
      mRenderer.exit();                       ///< unbind shader
    
     scene.pop( mRenderer.immediate() );  ///< pop matrices
    
     scene.step();                            ///< update camera physics
     
     /* NOTE: swapbuffers is NOT called here because
      * we are in just one of many potential windowEventHandler callbacks (which add optional effects)
      * swapbuffers is called by Interface::onDraw() only after ALL eventhandlers have been called
      * see gfx_control.h for the Interface class */
  }

  
  /*-----------------------------------------------------------------------------
   *  onRender() is inherited from GFXRenderNode (see gfx_render.h) 
   *-----------------------------------------------------------------------------*/
  virtual void onRender(){ 
        onDraw();
  }

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
    scene.resize(w,h);
  }

};


}//gfx::
