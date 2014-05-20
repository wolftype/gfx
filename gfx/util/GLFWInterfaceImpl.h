/*
 * =====================================================================================
 *
 *       Filename:  GLFWInterfaceImpl.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/07/2014 18:09:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  GLFWInterfaceImpl_INC
#define  GLFWInterfaceImpl_INC

#include "gfx/gfx_interface.h"
#include "GLFWWindow.h"


struct GLFWInterfaceImpl : public gfx::Interface::Impl {    

  Window * win; 
  
  GLFWInterfaceImpl( gfx::Interface * i ) : gfx::Interface::Impl(i) {}
    
  virtual void fullScreenToggle(){
  //  win -> fullScreenToggle(); 
  }

  virtual void getViewData(void * udata) {

        
        //COPY VIEW DATA
        interface -> vd().w = win->mWidth,;
        interface -> vd().h = win->mHeight; 

    }
  
    virtual void getKeyboardData(void * udata){
        GLFWInterface& glfw = *(GLFWInterface*)(udata);
           
        //COPY KEYBOARD DATA
        interface -> keyboard.alt       = glfw.keyboard.alt;
        interface -> keyboard.shift     = glfw.keyboard.shift;
        interface -> keyboard.ctrl      = glfw.keyboard.ctrl; 
        interface -> keyboard.code      = glfw.keyboard.key;
    }                                

    virtual void getMouseData(void * udata){
           GLFWInterface& glfw = *(GLFWInterface*)(udata);       
           //COPY MOUSE DATA
           interface -> mouse.x            = glfw.mouse.x;
           interface -> mouse.y            = glfw.mouse.y;

           interface -> mouse.dx           = glfw.mouse.dx / win->mWidth;
           interface -> mouse.dy           = -glfw.mouse.dy / win->mHeight;
           
           interface -> mouse.pos          = Vec3f( glfw.mouse.x / win->mWidth, 1 - glfw.mouse.y / win->mHeight, 0 ) ;
           interface -> mouse.move         = Vec3f( glfw.mouse.dx / win->mWidth, - glfw.mouse.dy / win->mHeight, 0 ) ;

       }
}; 


  /* struct GLVInterface : public gfx::Interface { */
  /*   GLVInterface() : gfx::Interface() { init(); } */
  /*   GLVImpl& glv(){ return *(GLVImpl*)impl; } */
  /*   virtual void init(){ */
  /*   impl = new GLVImpl(this); */
  /* } */ 
};


#endif   /* ----- #ifndef GLFWInterfaceImpl_INC  ----- */

