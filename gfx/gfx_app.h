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
 *                    Initialize() method
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
public GFXRenderer 
{

  WINDOWCONTEXT mContext;
  WINDOWCONTEXT& context() { return mContext; }
  WindowData& windowData() { return mContext.windowData(); }

  Scene scene;                            ///< modelviewprojection matrix transforms

  SceneController sceneController;        ///< interface to matrix transforms 
  ObjectController objectController;      ///< interface to objects on screen

  Vec3f mColor;                           ///< Background Color

  GFXio& io() { return mContext.interface.io; } ///< get io

  /*-----------------------------------------------------------------------------
   *  Constructor: Optional to Pass in width and height of window, and any command line arguments
   *-----------------------------------------------------------------------------*/
  GFXApp(int w=800, int h=400, string name = "GFXApp", int argc = 0, char ** argv = NULL) :
  GFXRenderer(w,h,this), mColor(.2,.2,.2)
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

      /*-----------------------------------------------------------------------------
       * 3.  Initialize GLEW and check for features
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
     
      /*-----------------------------------------------------------------------------
       * 5. Set up Programmable Rendering Pipeline
       *-----------------------------------------------------------------------------*/
       GFXRenderer::parent(this);
       GFXRenderer::scene(&scene);
       GFXRenderer::init();

      /*-----------------------------------------------------------------------------
       * 4. Enable Presets (depth func, blend func)
       *-----------------------------------------------------------------------------*/
       GL::enablePreset();

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

     clear(); 
     update();

     scene.push( GFXRenderNode::bImmediate );
      (*this)();    //<-- this operator()() is overloaded in GFXRenderNode (see gfx_render.h)
     scene.pop( GFXRenderNode::bImmediate );
    
     scene.step();
     
     //NOTE: swapbuffers is NOT called here because
     //we are in just one of many potential windowEventHandler callbacks (which add optional effects)
     //swapbuffers is called by Interface::onDraw() only after ALL eventhandlers have been called
     //see gfx_control.h for the Interface class
  }

  
  /*-----------------------------------------------------------------------------
   *  onRender() is inherited from GFXRenderNode (see gfx_render.h) 
   *-----------------------------------------------------------------------------*/
  virtual void onRender(){ 
    GL::enablePreset();
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
