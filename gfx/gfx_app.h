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
 *                 Notes:
 *
 *                    immediate mode is set in the ROOT rendernode mMode
                      which is passed to subsequent onRender calls
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
#ifndef GFX_APP_INCLUDED
#define GFX_APP_INCLUDED

#include <stdio.h>

#include "gfx_lib.h"              //<- Graphics Libraries
#include "gfx_scene.h"            //<- Matrix transforms (should rename this to gfx_modelViewProjection.h)
#include "gfx_control.h"          //<- Event Handling GFXio
#include "gfx_sceneController.h"  //<- Matrix transforms bound to user inputEvents
#include "gfx_objectController.h" //<- Objects in Memory bound to user inputEvents and windowEvents
#include "gfx_render.h"           //<- Graphics Pipeline Rendering Processes
#include "gfx_effects.h"          //<- Graphics Pipeline Rendering Processes


namespace gfx {

template<class WINDOWCONTEXT>
struct GFXApp :
public GFXRenderNode,
public InputEventHandler,
public WindowEventHandler
{

  WINDOWCONTEXT mContext;
  WINDOWCONTEXT& context() { return mContext; }
  WindowData& windowData() { return mContext.windowData(); }

  Interface<WINDOWCONTEXT>& interface(){ return mContext.interface; }

  Scene scene;                            ///< modelviewprojection matrix transforms

  GFXRenderNode mRenderer;                ///< root render node
  GFXStereoNode mStereo;                  ///< stereo rendering
  GFXShaderNode mShaderNode;              ///< shader pipeline
  GFXSceneNode  mSceneNode;               ///< scene ptr

  GFXRenderGraph mRenderGraph;

  int mMode;                              ///< render mode

  SceneController sceneController;        ///< interface to matrix transforms
  ObjectController objectController;      ///< interface to objects on screen

  Vec3f mColor;                           ///< Background Color

  GFXio& io() { return mContext.interface.io; } ///< get io

  /*-----------------------------------------------------------------------------
   *  Constructor: Optional to Pass in width and height of window, and any command line arguments
   *-----------------------------------------------------------------------------*/
  GFXApp(int w=800, int h=600, string name = "GFXApp", bool bStereoBuf = false)
  {

      mColor = Vec3f(.2, .2, .2);//222.0/256.0,165.0/256.0,87.0/256.0);

      mSceneNode.mScenePtr = &scene;

      printf ("app is creating window context\n");
     /*-----------------------------------------------------------------------------
      *  1. Initialize Window and Callbacks
      *-----------------------------------------------------------------------------*/
      WINDOWCONTEXT::System -> Initialize( bStereoBuf );
      mContext.create(w,h,name);

      init();

  }

  void init (){

      int w = io().viewdata.w;
      int h = io().viewdata.h;

      printf ("app is adding itself to context events\n");
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

      mContext.interface.OnResize( io().viewdata.w, io().viewdata.h);

      /*-----------------------------------------------------------------------------
       * 3.  Initialize GLEW and check for features (if not using GLES @todo otherwise what?)
       *-----------------------------------------------------------------------------*/
#ifndef GFX_USE_GLES
      printf("glew init \n");
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
       * 4. Set up Default Programmable Rendering Pipeline
       *
       *  "this" is a GFXRenderNode bound to mRenderer, a GFXShaderNode (default)
       *  to pipe draw methods into a different shader, bind "this" to another
       *  subclassed GFXRenderNode and optionally overload the virtual update() method
       *  call mRenderer.reset() first and re-init the rendergraph
       *-----------------------------------------------------------------------------*/
       //int glmode = GFXRenderGraph::IMMEDIATE;
       //int stereomode = bStereoBuf ? GFXRenderGraph::ACTIVE : GFXRenderGraph::MONO;

       //todo fix bug that crashes without shader node (see notes)
       mRenderer << mStereo << mShaderNode << mSceneNode << this;
       //todo fix glmode (immediate vs programmable vs es, etc)
       mRenderGraph.init(&mRenderer,w,h,GFXRenderGraph::IMMEDIATE,GFXRenderGraph::MONO);

       // todo who handles resize events?
       // mContext.interface.addWindowEventHandler(&mRenderGraph);

      /*-----------------------------------------------------------------------------
       * 5. Enable Presets (depth func, blend func) see gfx_gl.h
       *-----------------------------------------------------------------------------*/
       GL::enablePreset();


  }

//  void anaglyphic () {
//    mRenderer.reset();
//    mRenderer << mStereo << mShaderNode << mSceneNode << this;
//    mRenderGraph.init(&mRenderer,width, height, GFXRenderGraph::IMMEDIATE, GFXRenderGraph::ANAGLYPH);
//  }

  /*-----------------------------------------------------------------------------
   *  User must define setup() in a subclass. setup() is called by App::start() method
   *-----------------------------------------------------------------------------*/
  virtual void setup() = 0;

  virtual void _setup () {
    setup();
  }

  /*-----------------------------------------------------------------------------
   *  User must Define onDraw() in a subclass. onDraw() is called by onRender() method;
   *-----------------------------------------------------------------------------*/
  virtual void onDraw() = 0;

  template<class T>
  void draw(const T& t, float r=1,float g=1,float b=1,float a=1){

    if ( mRenderGraph.immediate() ){
      render::begin(r,g,b,a);
      render::draw(t);
    }
    else Renderable<T>::Draw(t, &mSceneNode);
  }


  /*-----------------------------------------------------------------------------
   *  Starts Graphics Thread.  To be called from main()  @TODO change to startGFX()
   *-----------------------------------------------------------------------------*/
  void start(){
    _setup();
    WINDOWCONTEXT::System -> Start(this);
  }

  /*-----------------------------------------------------------------------------
   *  Optional method for updating physics etc -- called onFrame();
   *-----------------------------------------------------------------------------*/
  virtual void onAnimate(){}

  /*-----------------------------------------------------------------------------
   *  Clear Window Contents
      @todo conflict: clear() is also method for clearing upstream render nodes
   *-----------------------------------------------------------------------------*/
  void clear(){

     mContext.setViewport();
     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
     glClearColor( mColor[0],mColor[1],mColor[2], 1 );
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }


  /*-----------------------------------------------------------------------------
   *  Default Rendering (Shader ambivalent) Window Event Handler Callback @sa GFXio
   *-----------------------------------------------------------------------------*/
  virtual void onFrame(){

     GL::enablePreset();

     clear();
     onAnimate();

     //mRenderer calls one upstream render (namely, this)
     //which is this app's onRender method
     //below onRender() defaults to onDraw()
     //BUT we can rebind pipeline with overloaded << operator.
     //see examples/xRendertoTexture.cpp

     mRenderGraph.onRender();

     scene.step();                            ///< update camera physics

     /* NOTE: swapbuffers is NOT called here because
      * we are in just one of many potential windowEventHandler callbacks (which add optional side effects)
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
   *  INPUT EVENT HANDLER METHODS, CALLED BY mContext.interface
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
     set(w,h);
    // mRenderer.onResize(w,h);  //or resize
   }

};


}//gfx::


#endif
