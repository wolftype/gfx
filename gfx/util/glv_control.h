/*
 * =====================================================================================
 *
 *       Filename:  glv_control.h
 *
 *    Description:  adapted from alloGLV glue (here we use templates to help disentangle)
 *                  to relay info from windowing context to glv...
 *
 *        Version:  1.0
 *        Created:  10/28/2014 18:25:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  glv_control_INC
#define  glv_control_INC

#include "GLV/glv_core.h"
#include "GLV/glv_buttons.h"
#include "util/gfx_event.h"

namespace gfx {

/// Base class for mapping window and input events to a GLV controller
class GLVControl {
public:
	///
	GLVControl(glv::GLV& v): mGLV(&v){}

	/// Set GLV controller
	GLVControl& glv(glv::GLV& v){ mGLV=&v; return *this; }
	
	/// Get mutable GLV controller
	glv::GLV& glv(){ return *mGLV; }

protected:
	glv::GLV * mGLV;
};



/// Mapping from keyboard and mouse controls to a GLV controller
template<class CONTEXT>
class GLVInputControl : public GLVControl, public InputEventHandler<CONTEXT> {
public:
	///
	GLVInputControl(glv::GLV& v): GLVControl(v){}
	virtual ~GLVInputControl(){}

	virtual bool onMouseDown(const Mouse& m);
	virtual bool onMouseUp(const Mouse& m);

	virtual bool onMouseDrag(const Mouse& m){
		return !motionToGLV(m, glv::Event::MouseDrag);
	}

	virtual bool onMouseMove(const Mouse& m){

		return !motionToGLV(m, glv::Event::MouseMove);
	}

	virtual bool onKeyDown(const Keyboard& k){
		return !keyToGLV(k, true);
	}

	virtual bool onKeyUp(const Keyboard& k){
		return !keyToGLV(k, false);
	}

protected:
	bool keyToGLV(const Keyboard& k, bool down);
	bool motionToGLV(const Mouse& m, glv::Event::t e);
};



/// Mapping from window events to a GLV controller
template<class CONTEXT>
class GLVWindowControl : public GLVControl, public WindowEventHandler<CONTEXT> {
public:
	///
	GLVWindowControl(glv::GLV& v): GLVControl(v){}
	virtual ~GLVWindowControl(){}

	virtual bool onCreate();
	virtual bool onDestroy();
	virtual bool onResize(int dw, int dh);
	//virtual bool onVisibility(bool v){ return true; }

	virtual bool onFrame();
};



/// A glv::GLV subclass that can be easily bound to an al::Window
template<class CONTEXT>
class GLVBinding : public glv::GLV{
public:

	GLVBinding():	mWindowCtrl(*this), mInputCtrl(*this)
  {
	  glv::GLV::disable(glv::DrawBack);
  }
	/// Bind GLV GUI to window
	
	/// By default, the GLV input event handler is attached to the front of the 
	/// window handler list and the GLV window event handler to the end of the
	/// window handler list. This means that the GUI will receive input events
	/// first and be drawn last.
	void bindTo(CONTEXT& win){
    win.interface.addWindowEventHandler(&mWindowCtrl);
    win.interface.addInputEventHandler(&mInputCtrl);
  }

private:
	GLVWindowControl<CONTEXT> mWindowCtrl;
	GLVInputControl<CONTEXT> mInputCtrl;
};



/* /// A GLV that can be detached into its own window from a parent window */
/* class GLVDetachable : public glv::GLV { */
/* public: */

/* 	/// */
/* 	GLVDetachable(); */

/* 	/// @param[in] parent	parent window */
/* 	GLVDetachable(Window& parent); */
	
/* 	/// Get button for detaching/attaching GUI */
/* 	glv::Button& detachedButton(){ return mDetachedButton; } */
	
/* 	/// Get parent window */
/* 	Window& parentWindow(){ return *mParentWindow; } */

/* 	/// Set parent window */
/* 	GLVDetachable& parentWindow(Window& v); */

/* 	/// Get detached window */
/* 	Window& detachedWindow(){ return mDetachedWindow(); } */
/* 	const Window& detachedWindow() const { return mDetachedWindow(); } */

/* 	/// Get whether GUI is detached from parent window */
/* 	bool detached() const { return detachedWindow().created(); } */

/* 	/// Get whether GUI is detached from parent window */
/* 	GLVDetachable& detached(bool v); */
	
/* 	/// Toggle whether GUI is detached from parent window */
/* 	GLVDetachable& detachedToggle(){ return detached(!detached()); } */

/* private: */
/* 	Window * mParentWindow; */
/* 	glv::Lazy<Window> mDetachedWindow; */
/* 	GLVInputControl mInputControl; */
/* 	GLVWindowControl mWindowControl; */
/* 	glv::Button mDetachedButton; */

/* 	void addGUI(Window& w); */
/* 	void remGUI(Window& w); */
/* 	void init(); */
/* }; */


template<class CONTEXT>
inline bool GLVInputControl<CONTEXT>::onMouseDown(const Mouse& m){
	glv::space_t xrel=m.x, yrel=m.y;
	glv().setMouseDown(xrel,yrel, m.button, 0);
	glv().setMousePos(m.x, m.y, xrel, yrel);
	return !glv().propagateEvent();
}

template<class CONTEXT>
inline bool GLVInputControl<CONTEXT>::onMouseUp(const Mouse& m){
	glv::space_t xrel, yrel;
	glv().setMouseUp(xrel,yrel, m.button, 0);
	glv().setMousePos(m.x, m.y, xrel, yrel);
	return !glv().propagateEvent();
}

template<class CONTEXT>
inline bool GLVInputControl<CONTEXT>::keyToGLV(const Keyboard& k, bool down){
	down ? glv().setKeyDown(k.code) : glv().setKeyUp(k.code);
	const_cast<glv::Keyboard*>(&glv().keyboard())->alt(k.alt);
	const_cast<glv::Keyboard*>(&glv().keyboard())->caps(k.caps);
	const_cast<glv::Keyboard*>(&glv().keyboard())->ctrl(k.ctrl);
	const_cast<glv::Keyboard*>(&glv().keyboard())->meta(k.meta);
	const_cast<glv::Keyboard*>(&glv().keyboard())->shift(k.shift);
	return glv().propagateEvent();
}

template<class CONTEXT>
inline bool GLVInputControl<CONTEXT>::motionToGLV(const Mouse& m, glv::Event::t e){
	glv::space_t x = m.x, y = m.y, relx = x, rely = y;
	glv().setMouseMotion(relx, rely, e);
	glv().setMousePos((int)x, (int)y, relx, rely);
	return glv().propagateEvent();
}


template<class CONTEXT>
inline bool GLVWindowControl<CONTEXT>::onCreate(){
	glv().broadcastEvent(glv::Event::WindowCreate);
	return true;
}

template<class CONTEXT>
inline bool GLVWindowControl<CONTEXT>::onDestroy(){
	glv().broadcastEvent(glv::Event::WindowDestroy);
	return true;
}

template<class CONTEXT>
inline bool GLVWindowControl<CONTEXT>::onResize(int dw, int dh){
	glv().extent(glv().width() + dw, glv().height() + dh);
	glv().broadcastEvent(glv::Event::WindowResize);
	return true;
}

template<class CONTEXT>
inline bool GLVWindowControl<CONTEXT>::onFrame(){
	glv().drawGLV(glv().w, glv().h, .01);//WindowEventHandler<CONTEXT>::window().spfActual());
	return true;
}




} // gfx::

#endif   /* ----- #ifndef glv_control_INC  ----- */
