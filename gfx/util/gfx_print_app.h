/*
 * =====================================================================================
 *
 *       Filename:  gfx_print_app.h
 *
 *    Description:  an app with printout function
 *
 *        Version:  1.0
 *        Created:  01/27/2015 15:51:04
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome  
 *
 * =====================================================================================
 */

#ifndef  gfx_print_app_INC
#define  gfx_print_app_INC

//#include "gfx/util/glut_window.hpp"
#include "gfx/gfx_app.h"
#include "gfx/gfx_postscript.h"


namespace gfx {

  template<typename CONTEXT>
  struct GFXPrintApp : public GFXApp<CONTEXT> {  

    PostScript ps;

    GFXPrintApp(int w=400, int h=400, string name="gfx print out")
      : GFXApp<CONTEXT>(w,h,name) {}

    /// Called when a keyboard key is pressed
    virtual void onKeyDown(const gfx::Keyboard& k) {
      switch(k.code){
        case 'o':
          printf("o\n");
          ps.bTightBounds = true;
          GL::enablePreset();
          this->scene.camera.stereo(false);
          this->scene.push(true);
           ps.print(*this);
          this->scene.pop(true);
          GL::disablePreset();
          break;
        case 'p':
          printf("p\n");
          ps.bTightBounds = false;
          GL::enablePreset();
          this->scene.camera.stereo(true);
          this->scene.camera.left(true);
          this->scene.push(true);
           ps.print(*this);
          this->scene.pop(true);

          this->scene.camera.left(false);
          this->scene.push(true);
           ps.print(*this);
          this->scene.pop(true);
          
          GL::disablePreset();
          break;
      }
    }

  };

} //gfx::

#endif   /* ----- #ifndef gfx_glv_app_INC  ----- */
