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

  
  /*-----------------------------------------------------------------------------
   *  IO DATA CONTAINERS (mouse, keyboard, view)
   *-----------------------------------------------------------------------------*/
 struct Mouse {

     Mouse( int but, int ste, int _x, int _y) :
     button(but), state(ste), x(_x), y(_y)
     {}

     enum  {
         Up = 1, Down, Left, Right
     };

     enum {
         IsDown = 1, IsMoving, Click
     };

     ///< 2D Mouse Position, first and second derivatives of motion, position relative to TL corner
		 float x, y, dx, dy, ddx, ddy, xrel, yrel; 

     Vec3f origin, click, pos, move, accel, cat, drag, dragAccum, dragCat, projectFar, projectNear, projectMid;

		 Vec3f biv, bivCat, dragBiv, dragBivCat;

     int gesture;  // stores major direction of mouse movement
     int button;   // which button is down
     int state;    // current mouse state (IsDown, IsMoving, 

		 bool isDown, isMoving, newClick;		
		 
     void toggle() { newClick = !newClick; }
	};

	struct ViewData  {
		float w, h; 
    Vec3f z; 
    Vec3f ray, clickray; 
	};

	struct Keyboard {
		bool shift, alt, caps, ctrl, meta, f, down;
		int code;
		Keyboard(int key, int x, int y) : shift(0), alt(0), caps(0), ctrl(0), code(key){}
	};    

  /* Mapped Key values ripped from GLV */
  namespace Key{
      enum {

          // Standard ASCII non-printable characters
          Enter		  =3,		/**< */
          Backspace	=8,		/**< */
          Tab			  =9,		/**< */
          Return		=13,	/**< */
          Escape		=27,	/**< */
          Delete		=127,	/**< */

          // Non-standard, but common keys.
          F1=256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
          Insert, Left, Up, Right, Down, PageDown, PageUp, End, Home
      };
  }

//RIPPED FROM ALLOCORE (AND TEMPLATED)...
/// Controller for handling input events

/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
template<class TWIN>
class InputEventHandler{
public:
	InputEventHandler() : mWindow(NULL){}
	virtual ~InputEventHandler(){}

	/// Called when a keyboard key is pressed
	virtual bool onKeyDown(const Keyboard& k){return true;}
	
	/// Called when a keyboard key is released
	virtual bool onKeyUp(const Keyboard& k){return true;}

	/// Called when a mouse button is pressed
	virtual bool onMouseDown(const Mouse& m){return true;}
	
	/// Called when the mouse moves while a button is down
	virtual bool onMouseDrag(const Mouse& m){return true;}
	
	/// Called when the mouse moves
	virtual bool onMouseMove(const Mouse& m){return true;}
	
	/// Called when a mouse button is released
	virtual bool onMouseUp(const Mouse& m){return true;}

	/// Return self
	InputEventHandler& inputEventHandler(){ return *this; }

	bool attached() const { return NULL != mWindow; }
	TWIN& window(){ return *mWindow; }
	const TWIN& window() const { return *mWindow; }

protected:
	TWIN * mWindow;
	InputEventHandler& window(TWIN * v){ mWindow=v; return *this; }
	void removeFromWindow(){}
};


/// Subscriber for general Window events
/// The return value of the event handlers determines whether or not
/// the event should be propagated to other handlers.
template<class TWIN>
class WindowEventHandler {
public:
	WindowEventHandler() : mWindow(NULL){}
	virtual ~WindowEventHandler(){}


	/// Called after window is created with valid OpenGL context
	virtual bool onCreate(){ return true; }
	
	/// Called before the window and its OpenGL context are destroyed
	virtual bool onDestroy(){ return true; }
	
	/// Called every frame
	virtual bool onFrame(){ return true; }
	
	/// Called whenever window dimensions change
	virtual bool onResize(int dw, int dh){ return true; }
	
	/// Called when window changes from hidden to shown and vice versa
	virtual bool onVisibility(bool v){ return true; }

	/// Return self
	WindowEventHandler& windowEventHandler(){ return *this; }

	bool attached() const { return NULL != mWindow; }
	TWIN& window(){ return *mWindow; }
	const TWIN& window() const { return *mWindow; }

protected:
	//friend class Window;
	TWIN * mWindow;
	WindowEventHandler& window(TWIN * v){ mWindow=v; return *this; }
	void removeFromWindow(){}
};


