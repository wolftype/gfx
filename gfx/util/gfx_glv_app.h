/*
 * =====================================================================================
 *
 *       Filename:  gfx_glv_app.h
 *
 *    Description:  an app with glv (gui)
 *
 *        Version:  1.0
 *        Created:  01/27/2015 15:51:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */

#ifndef gfx_glv_app_INC
#define gfx_glv_app_INC

#include "gfx/util/glut_window.hpp"
#include "gfx/util/glfw_window.hpp"
#include "gfx/gfx_app.h"
#include "gfx/util/glv_control.h"
#include "gfx/util/glv_gui.h"

namespace gfx {

using namespace glv;

template <class CONTEXT>
struct GFXAppGui : public GFXApp<CONTEXT>
{

  GFXAppGui (int w = 400, int h = 400, string name = "gfx with glv")
      : GFXApp<CONTEXT> (w, h, name)
  {
  }

  GLVBinding glv;
  Gui gui;

  void bindGLV ()
  {
    glv.listenTo (this->mContext);
    glv << gui;
    this->mContext.reshape();
  }

  virtual void _setup ()
  {
    bindGLV ();
    //get the extents to glv
//    this->mContext.interface.OnResize (this->io().viewdata.w, this->io().viewdata.h);
    this->setup ();
  }

  /* void text(string s, int ow = 50, int oh = 100){ */
  /*   int w = scene.camera.lens.width(); */
  /*   int h = scene.camera.lens.height(); */
  /*   //draw::enter2D(w, h); */
  /*     glTranslated(ow,h-oh,0); */
  /*     glColor3f(1,1,1); */
  /*     draw::text( s.c_str() ); */
  /* } */
};

}  //gfx::

#endif /* ----- #ifndef gfx_glv_app_INC  ----- */
