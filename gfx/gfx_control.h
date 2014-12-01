/*
 * =====================================================================================
 *
 *       Filename:  gfx_control.h
 *
 *    Description:  generic input and window event handlers
 *    partially ripped from allocore, but templated so no awkward dependency graphs or
 *    friend classes, etc.
 *
 *        Version:  1.0
 *        Created:  10/30/2014 14:14:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  gfx_control_INC
#define  gfx_control_INC

#include "gfx_matrix.h"
#include <vector>

using std::vector;
using std::cout;
using std::endl;

namespace gfx { 

/*!
 *  Stores width, height, and id of a Window
 */
struct WindowData {
  WindowData(int width = 100, int height = 100, int id = 0) : 
    mWidth(width), mHeight(height), mID(id), isFullScreen(false){}
  int mWidth, mHeight, mPrevWidth, mPrevHeight, mID;
  bool isFullScreen;
  float ratio() { return (float) mWidth/mHeight; }
  void reshape(int w, int h){
    mWidth = w; mHeight =h;
  }
  void save(){
    mPrevWidth = mWidth;
    mPrevHeight = mHeight;
  }
  int width() const { return mWidth; }
  int height() const { return mHeight; }
  int prevWidth() const { return mPrevWidth; }
  int prevHeight() const { return mPrevHeight; }
};

  /*-----------------------------------------------------------------------------
   *  IO DATA CONTAINERS (mouse, keyboard, view)
   *-----------------------------------------------------------------------------*/
 struct Mouse {

     Mouse(){}

     Mouse(int but, int ste, int _x, int _y) :
     button(but), state(ste), x(_x), y(_y)
     {}

     enum  {
         Up = 1, Down, Left, Right
     };

     enum {
         IsDown   = 1, 
         IsMoving = 1<<1, 
         Click    = 1<<2
     };

     ///< 2D Mouse Position, first and second derivatives of motion, position relative to TL corner
     float lastX, lastY, x, y, dx, dy;//, ddx, ddy, xrel, yrel; 

     Vec3f click() { return Vec3f(lastX, lastY, 0); }
     Vec3f pos() { return Vec3f(x,y,0); }

     //confusing! get rid of these . . .
     Vec3f move, accel, cat, drag, dragAccum, dragCat;
     Vec3f biv, bivCat, dragBiv, dragBivCat;

     int gesture;  // stores major direction of mouse movement (Mouse::Up, Mouse::Down, Mouse::Left, Mouse::Right)
     int button;   // which button is down
     int state;    // current mouse state (IsDown, IsMoving) 

     bool isDown(){ return state & IsDown; }
     bool isMoving() { return state & IsMoving; }
     bool newClick(){ return state & Click; }   
     
     
     void calc(){
        //gesture
        int mdir = 0;
        if ( fabs(dx) >= fabs(dy) ) mdir = dx > 0 ? Mouse::Right : Mouse::Left;
        else if ( fabs(dx) < fabs(dy) ) mdir = dy < 0 ? Mouse::Up : Mouse::Down; 
        gesture = mdir;
     }
     
     //void toggle() { newClick = !newClick; }
  };

  struct ViewData  {
    float w, h; 
    Vec3f z; 
    Vec3f projectFar, projectNear, projectMid;
    Vec3f ray, clickray; 
  };

  /* Mapped Key values ripped from GLV */
  namespace Key{
      enum {

          //Modifiers (glut anyway)
          Shift = 1,
          Ctrl  = 1<<1,
          Alt   = 1<<2,

          // Standard ASCII non-printable characters
          Enter      =3,    /**< */
          Backspace  =8,    /**< */
          Tab        =9,    /**< */
          Return     =13,    /**< */
          Escape     =27,    /**< */
          Delete     =127,   /**< */

          Left=100, Up, Right, Down,

          // Non-standard, but common keys.
          F1=256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
          Insert, PageDown, PageUp, End, Home
      };
  }

  struct Keyboard {
    int modifier;
    bool down;
    int code;
    Keyboard(){}
    Keyboard(int key, int mod, int x, int y, bool bDown) : code(key), modifier(mod), down(bDown) {}

    bool ctrl() { return modifier & Key::Ctrl; }
    bool shift() { return modifier & Key::Shift; }
    bool alt() { return modifier & Key::Alt; }

  };    


  /*-----------------------------------------------------------------------------
   * GFX IO DATA CONTAINER
   *-----------------------------------------------------------------------------*/
  struct GFXio{
    Mouse mouse;
    Keyboard keyboard;
    ViewData viewdata;
    Vec3f click() { Vec3f m = mouse.click(); return Vec3f(m[0]/viewdata.w, m[1]/viewdata.h,0); } 
    bool trigger=true;
  };

//RIPPED FROM ALLOCORE (AND TEMPLATED)...
/// Controller for handling input events

/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
//template<class TWIN>
class InputEventHandler{
public:
  InputEventHandler() {}//: mWindow(NULL){}
  virtual ~InputEventHandler(){}

  /// Called when a keyboard key is pressed
  virtual void onKeyDown(const Keyboard& k){}
  
  /// Called when a keyboard key is released
  virtual void onKeyUp(const Keyboard& k){}

  /// Called when a mouse button is pressed
  virtual void onMouseDown(const Mouse& m){}
  
  /// Called when the mouse moves while a button is down
  virtual void onMouseDrag(const Mouse& m){}
  
  /// Called when the mouse moves
  virtual void onMouseMove(const Mouse& m){}
  
  /// Called when a mouse button is released
  virtual void onMouseUp(const Mouse& m){}

  /// Return self
  InputEventHandler& inputEventHandler(){ return *this; }

//  bool attached() const { return NULL != mWindow; }
//  TWIN& window(){ return *mWindow; }
//  const TWIN& window() const { return *mWindow; }
//  InputEventHandler& window(TWIN * v){ mWindow=v; return *this; }

protected:
//  TWIN * mWindow; //Q: do we even need this reference back to window ?
//  void removeFromWindow(){}
};


/// Subscriber for general Window events
/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
//template<class TWIN>
class WindowEventHandler {
public:
  WindowEventHandler() {}// : mWindow(NULL){}
  virtual ~WindowEventHandler(){}


  /// Called after window is created with valid OpenGL context
  virtual void onCreate(){ }
  
  /// Called before the window and its OpenGL context are destroyed
  virtual void onDestroy(){  }
  
  /// Called every frame
  virtual void onFrame(){}
  
  /// Called whenever window dimensions change
  virtual void onResize(int dw, int dh){}
  
  /// Called when window changes from hidden to shown and vice versa
  virtual void onVisibility(bool v){}

  /// Return self
  WindowEventHandler& windowEventHandler(){ return *this; }

//  bool attached() const { return NULL != mWindow; }
//  TWIN& window(){ return *mWindow; }
//  const TWIN& window() const { return *mWindow; }
//  WindowEventHandler& window(TWIN * v){ mWindow=v; return *this; }

protected:
//  TWIN * mWindow; //Q: do we even need this reference back to window ?
//  void removeFromWindow(){}
};

/*-----------------------------------------------------------------------------
 *  Some Callbacks to be implemented later 
 *-----------------------------------------------------------------------------*/
template <class CONTEXT>
struct Interface {

  static GFXio io;

  static vector<InputEventHandler*> mInputEventHandlers;
  static vector<WindowEventHandler*> mWindowEventHandlers;
  
  Interface& addInputEventHandler( InputEventHandler * e, CONTEXT * w=NULL ){ 
     mInputEventHandlers.push_back(e); 
      // note: could io data containeres: bind e -> io(io) here.
   //  if(w!=NULL)e->window(w); 
     return *this; 
   } 
  Interface& addWindowEventHandler( WindowEventHandler * e, CONTEXT * w=NULL ){ 
     mWindowEventHandlers.push_back(e); 
   //  if(w!=NULL)e->window(w);
     return *this; 
   }

  static void OnDraw(){
    for (auto& i : mWindowEventHandlers){
      i->onFrame();
    }
    CONTEXT::SwapBuffers(); //<-- Swap Buffers only AFTER all window events onFrame() 
  }

  static void OnResize(int w, int h){
    io.viewdata.w = w; io.viewdata.h = h;
    for (auto& i : mWindowEventHandlers){
      i->onResize(w,h);
    }
  }

  static void OnCreate(){
    for (auto& i : mWindowEventHandlers){
      i->onCreate();
    }
  }

  static void OnMouseUp(const Mouse& m){
    io.mouse.state=0;
    for (auto& i : mInputEventHandlers){
      i->onMouseUp(io.mouse);
    }
  }

  static void OnMouseDown(const Mouse& m){
    io.mouse.state |= Mouse::IsDown;
    io.mouse.lastX = m.x;
    io.mouse.lastY = m.y;
    for (auto& i : mInputEventHandlers){
      i->onMouseDown(io.mouse);
    }
  }


  static void OnMouseMove(const Mouse& m){
    io.mouse.state = m.state; io.mouse.x=m.x; io.mouse.y=m.y;
    for (auto& i : mInputEventHandlers){ i->onMouseMove(io.mouse); }
  }
  
  static void OnMouseDrag(const Mouse& m){
    io.mouse.state=m.state; io.mouse.x=m.x; io.mouse.y=m.y;
    io.mouse.dx = io.mouse.x - io.mouse.lastX;
    io.mouse.dy = io.mouse.y - io.mouse.lastY; 

    io.mouse.calc();

    for (auto& i : mInputEventHandlers){ i->onMouseDrag(io.mouse); }
  }

  static void OnKeyDown(const Keyboard& k){
    io.keyboard=k;
    for (auto& i : mInputEventHandlers){ i->onKeyDown(io.keyboard); }
  }

  static void OnKeyUp(const Keyboard& k){
    io.keyboard=k;
    for (auto& i : mInputEventHandlers){ i->onKeyUp(io.keyboard); }
  }

};   

template<class CONTEXT>
GFXio Interface<CONTEXT>::io;

template<class CONTEXT>
vector<InputEventHandler*> Interface<CONTEXT>::mInputEventHandlers;

template<class CONTEXT>
vector<WindowEventHandler*> Interface<CONTEXT>::mWindowEventHandlers;



} //gfx::

#endif   /* ----- #ifndef gfx_control_INC  ----- */
