/*
 * =====================================================================================
 *
 *       Filename:  glv_control.h
 *
 *    Description:  adapted from alloGLV glue (here we use templates to help disentangle dependencies)
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

#include "gfx_control.h"

#include "GLV/glv_core.h"
#include "GLV/glv_buttons.h"

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

  virtual void onMouseDown(const Mouse& m);
  virtual void onMouseUp(const Mouse& m);

  virtual void onMouseDrag(const Mouse& m){
    motionToGLV(m, glv::Event::MouseDrag);
  }

  virtual void onMouseMove(const Mouse& m){
    motionToGLV(m, glv::Event::MouseMove);
  }

  virtual void onKeyDown(const Keyboard& k){
    keyToGLV(k, true);
  }

  virtual void onKeyUp(const Keyboard& k){
    keyToGLV(k, false);
  }

protected:
  void keyToGLV(const Keyboard& k, bool down);
  void motionToGLV(const Mouse& m, glv::Event::t e);
};



/// Mapping from window events to a GLV controller
template<class CONTEXT>
class GLVWindowControl : public GLVControl, public WindowEventHandler<CONTEXT> {
public:
  ///
  GLVWindowControl(glv::GLV& v): GLVControl(v){}
  virtual ~GLVWindowControl(){}

  virtual void onCreate();
  virtual void onDestroy();
  virtual void onResize(int dw, int dh);
  virtual void onFrame();
};



/// A glv::GLV subclass that can be easily bound to an al::Window
template<class CONTEXT>
class GLVBinding : public glv::GLV{
public:

  GLVBinding() : mWindowCtrl(*this), mInputCtrl(*this){
    disable( glv::DrawBack);
  }
  
  void listenTo(CONTEXT& win){
    win.interface.addWindowEventHandler(&mWindowCtrl, &win);
    win.interface.addInputEventHandler(&mInputCtrl, &win);
  }

private:
  GLVWindowControl<CONTEXT> mWindowCtrl;
  GLVInputControl<CONTEXT> mInputCtrl;
};


template<class CONTEXT>
inline void GLVInputControl<CONTEXT>::onMouseDown(const Mouse& m){
  glv::space_t xrel=m.x, yrel=m.y;
  glv().setMouseDown(xrel,yrel, m.button, 0);
  glv().setMousePos(m.x, m.y, xrel, yrel);
  glv().propagateEvent();
}

template<class CONTEXT>
inline void GLVInputControl<CONTEXT>::onMouseUp(const Mouse& m){
  glv::space_t xrel, yrel;
  glv().setMouseUp(xrel,yrel, m.button, 0);
  glv().setMousePos(m.x, m.y, xrel, yrel);
  glv().propagateEvent();
}

template<class CONTEXT>
inline void GLVInputControl<CONTEXT>::keyToGLV(const Keyboard& k, bool down){
  down ? glv().setKeyDown(k.code) : glv().setKeyUp(k.code);
  const_cast<glv::Keyboard*>(&glv().keyboard())->alt(k.alt);
  const_cast<glv::Keyboard*>(&glv().keyboard())->caps(k.caps);
  const_cast<glv::Keyboard*>(&glv().keyboard())->ctrl(k.ctrl);
  const_cast<glv::Keyboard*>(&glv().keyboard())->meta(k.meta);
  const_cast<glv::Keyboard*>(&glv().keyboard())->shift(k.shift);
  glv().propagateEvent();
}

template<class CONTEXT>
inline void GLVInputControl<CONTEXT>::motionToGLV(const Mouse& m, glv::Event::t e){
  glv::space_t x = m.x, y = m.y, relx = x, rely = y;
  glv().setMouseMotion(relx, rely, e);
  glv().setMousePos((int)x, (int)y, relx, rely);
  glv().propagateEvent();
}


template<class CONTEXT>
inline void GLVWindowControl<CONTEXT>::onCreate(){
  glv().broadcastEvent(glv::Event::WindowCreate);
}

template<class CONTEXT>
inline void GLVWindowControl<CONTEXT>::onDestroy(){
  glv().broadcastEvent(glv::Event::WindowDestroy);
}

template<class CONTEXT>
inline void GLVWindowControl<CONTEXT>::onResize(int dw, int dh){
  glv().extent(dw,dh );//glv().width(), glv().height());
  glv().broadcastEvent(glv::Event::WindowResize);
}

template<class CONTEXT>
inline void GLVWindowControl<CONTEXT>::onFrame(){
  glv().drawGLV(glv().w, glv().h, .1);//WindowEventHandler<CONTEXT>::window().spfActual());
}



/* /// A GLV that can be detached into its own window from a parent window */
/* template<class CONTEXT> */
/* class GLVDetachable : public glv::GLV { */
/* public: */

/* 	/// */
/* 	GLVDetachable() :	glv::GLV(0,0), */ 
/* 	mParentWindow(NULL), mInputControl(*this), mWindowControl(*this) */
/*   { */
/* 	 init(); */
/*   } */



/* 	/// @param[in] parent	parent window */
/* 	GLVDetachable(CONTEXT& parent) :	glv::GLV(0,0), */ 
/* 	mInputControl(*this), mWindowControl(*this) */
/* { */
/* 	parentWindow(parent); */
/* 	init(); */
/* } */

	
/* 	/// Get button for detaching/attaching GUI */
/* 	glv::Button& detachedButton(){ return mDetachedButton; } */
	
/* 	/// Get parent window */
/* 	Window& parentWindow(){ return *mParentWindow; } */

/* 	/// Set parent window */
/* 	GLVDetachable& parentWindow(CONTEXT& v); */

/* 	/// Get detached window */
/* 	CONTEXT& detachedWindow(){ return mDetachedWindow(); } */
/* 	const CONTEXT& detachedWindow() const { return mDetachedWindow(); } */

/* 	/// Get whether GUI is detached from parent window */
/* //	bool detached() const { return detachedWindow().created(); } */

/* 	/// Get whether GUI is detached from parent window */
/* //	GLVDetachable& detached(bool v); */
	
/* 	/// Toggle whether GUI is detached from parent window */
/* //	GLVDetachable& detachedToggle(){ return detached(!detached()); } */

/* private: */
/* 	CONTEXT * mParentWindow; */
/* 	glv::Lazy<CONTEXT> mDetachedWindow; */
/* 	GLVInputControl<CONTEXT> mInputControl; */
/* 	GLVWindowControl<CONTEXT> mWindowControl; */
/* 	glv::Button mDetachedButton; */

/* 	void listenTo(CONTEXT& w); */
/* 	void init(); */
/* }; */


/* template<class CONTEXT> */
/* static void ntDetachedButton(const glv::Notification& n){ */
/* 	GLVDetachable<CONTEXT> * R = n.receiver<GLVDetachable<CONTEXT>>(); */
/* 	//if(R->mouse().isDown()) return; */
/* 	/1* if(n.sender<glv::Button>()->getValue()){ *1/ */
/* 	/1* 	R->detached(true); *1/ */
/* 	/1* } *1/ */
/* 	/1* else{ *1/ */
/* 	/1* 	R->detached(false); *1/ */
/* 	/1* } *1/ */
/* } */

/* template<class CONTEXT> */
/* void GLVDetachable<CONTEXT>::init(){ */
/* 	mDetachedButton.attach(ntDetachedButton<CONTEXT>, glv::Update::Value, this); */
/* 	mDetachedButton.disable(glv::Momentary); */
/* 	mDetachedButton.symbolOn(glv::draw::viewChild); */
/* 	mDetachedButton.symbolOff(glv::draw::viewSibling); */
/* 	mDetachedButton.disable(glv::DrawBorder); */
/* 	stretch(1,1); */
/* 	this->disable(glv::DrawBack); */
/* } */

/* template<class CONTEXT> */
/* void GLVDetachable<CONTEXT>::listenTo(CONTEXT& w){ */
/*     w.interface.addWindowEventHandler(&mWindowControl); */
/*     w.interface.addInputEventHandler(&mInputControl); */
/* } */

/* /1* template<class CONTEXT> *1/ */
/* /1* GLVDetachable& GLVDetachable::detached(bool v){ *1/ */
/* /1* 	if(v && !detached()){			// is not detached *1/ */
/* /1* 		if(mParentWindow){ *1/ */
/* /1* 			//remGUI(parentWindow()); *1/ */
/* /1* 		} *1/ */
/* /1* 		glv::Rect ru = unionOfChildren(); *1/ */
/* /1* 		enable(glv::DrawBack); *1/ */
/* /1* 		{ *1/ */
/* /1* 			glv::View * cv = child; *1/ */
/* /1* 			while(cv){ *1/ */
/* /1* 				cv->posAdd(-ru.l, -ru.t); *1/ */
/* /1* 				cv = cv->sibling; *1/ */
/* /1* 			} *1/ */
/* /1* 		} *1/ */
/* /1* 		//ru.print(); *1/ */
/* /1* 		//posAdd(-ru.l, -ru.t); *1/ */
/* /1* 		//detachedWindow().create(Window::Dim(ru.w, ru.h)); *1/ */
/* /1* 		int pl=0, pt=0; *1/ */
/* /1* 		if(mParentWindow){ *1/ */
/* /1* 			pl = parentWindow().dimensions().l; *1/ */
/* /1* 			pt = parentWindow().dimensions().t; *1/ */
/* /1* 			//printf("%d %d\n", pl, pt); *1/ */
/* /1* 		} *1/ */
/* /1* 		detachedWindow().create(Window::Dim(pl, pt, ru.w, ru.h)); *1/ */
/* /1* 		listenTo(detachedWindow()); *1/ */
/* /1* 	} *1/ */
/* /1* 	else if(detached()){ // is currently detached, attach back to parent, if any *1/ */
/* /1* 		remGUI(detachedWindow()); *1/ */
/* /1* 		detachedWindow().destroy(); *1/ */
/* /1* 		if(mParentWindow){ *1/ */
/* /1* 			disable(glv::DrawBack); *1/ */
/* /1* 			pos(0,0); *1/ */
/* /1* 			addGUI(parentWindow()); *1/ */
/* /1* 		} *1/ */
/* /1* 	} *1/ */

/* /1* 	// This is a hack to ensure all GLV mouse button states are "up" (false). *1/ */
/* /1* 	// Because the GLV changes windows between mouse down and mouse up calls, *1/ */
/* /1* 	// the mouse relative position gets into an inconsistent state on mouse up. *1/ */
/* /1* 	const int mx = mouse().x(); *1/ */
/* /1* 	const int my = mouse().y(); *1/ */
/* /1* 	for(int i=0; i<GLV_MAX_MOUSE_BUTTONS; ++i){ *1/ */
/* /1* 		glv::space_t x=mx,y=my; *1/ */
/* /1* 		setMouseUp(x,y, i, 1); *1/ */
/* /1* 	} *1/ */

/* /1* 	return *this; *1/ */
/* /1* } *1/ */

/* template<class CONTEXT> */
/* GLVDetachable<CONTEXT>& GLVDetachable<CONTEXT>::parentWindow(CONTEXT& v){ */
/* 	/1* if(&v != mParentWindow){ *1/ */
/* 	/1* 	if(!detached()){ *1/ */
/* 	/1* 		if(mParentWindow){ *1/ */
/* 	/1* 			remGUI(parentWindow()); *1/ */
/* 	/1* 		} *1/ */
/* 			mParentWindow = &v; */
/* 			disable(glv::DrawBack); */
/* 			listenTo(parentWindow()); */
/* 			/1* //printf("%d\n", parentWindow().created()); *1/ */
/* 		/1* } *1/ */
/* 		/1* else{ *1/ */
/* 			/1* mParentWindow = &v; *1/ */
/* 		/1* } *1/ */
/* 	/1* } *1/ */
/* 	return *this; */
/* } */


} // gfx::

#endif   /* ----- #ifndef glv_control_INC  ----- */
