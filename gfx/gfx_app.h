/*
 * =====================================================================================
 *
 *       Filename:  gfx_app.h
 *
 *    Description:  an appclass that can take a variety of window contexts
 *                  WINDOWCONTEXT must have a System (singleton)
 *                  An Initialize function
 *                  A Start function
 *        Version:  1.0
 *        Created:  05/29/2014 07:18:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "gfx_lib.h"
#include "util/gfx_event.h"
#include "util/glv_control.h"

namespace gfx {

template<class WINDOWCONTEXT>
struct App : public InputEventHandler<WINDOWCONTEXT>, public WindowEventHandler<WINDOWCONTEXT>{

    WINDOWCONTEXT mWindow;
    //WINDOWCONTEXT& window() { return mWindow; }
    //
    GLVBinding<WINDOWCONTEXT> glv;

  App(int w=800, int h=400, int argc = 0, char ** argv = NULL)  
  {

     /*-----------------------------------------------------------------------------
      *  Initialize Window and Callbacks
      *-----------------------------------------------------------------------------*/
      WINDOWCONTEXT::System -> Initialize();
     // mWindow.template create(this,w,h);
      mWindow.create(w,h);

      //not sure why Event Handlers need pointer to parent that calls it . . . maybe useful later
      InputEventHandler<WINDOWCONTEXT>::window( &mWindow );
      WindowEventHandler<WINDOWCONTEXT>::window( &mWindow );

      //add listeners to window and input events
      mWindow.interface.addWindowEventHandler(this);
      mWindow.interface.addInputEventHandler(this); 
      //do same for glv
      glv.bindTo(mWindow);

      /*-----------------------------------------------------------------------------
       *  Initialize GLEW
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
   *  User Initialization (init is called automatically by App::start() method)
   *-----------------------------------------------------------------------------*/
  virtual void init() = 0;
  virtual void onDraw() = 0;

  void start(){
    init();
    WINDOWCONTEXT::System -> Start(this);
  }

  ~App(){
     WINDOWCONTEXT::System -> Terminate();
  }

  virtual void update(){}

  virtual bool onFrame(){
    update();
    mWindow.setViewport();

    glClearColor(.2,.2,.2,1);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    onDraw();
     
    mWindow.onFrame(); //<-- Swap buffers, listen for events, etc.

    return true;
  }

  
  /*-----------------------------------------------------------------------------
   *  INPUT EVENT HANDLER METHODS
   *-----------------------------------------------------------------------------*/
  virtual bool onMouseMove(const Mouse& m){ return true; }//int x, int y){}
  virtual bool onMouseDrag(const Mouse& m){return true; }//int x, int y){}
  virtual bool onMouseDown(const Mouse& m){return true; }//int button, int action){}
  virtual bool onMouseUp(const Mouse& m){return true; }//int x, int y){}
  virtual bool onKeyDown(const Keyboard& k){return true; }//int key, int action=0){}
  virtual bool onKeyUp(const Keyboard& k){return true; }//int key){}

  /*-----------------------------------------------------------------------------
   *  WINDOW EVENT HANDLER METHODS
   *-----------------------------------------------------------------------------*/
  virtual bool onCreate(){return true; }
  virtual bool onDestroy(){return true; }
 // virtual bool onFrame();{}


};


}//gfx::
