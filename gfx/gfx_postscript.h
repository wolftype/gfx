/*
 * =====================================================================================
 *
 *       Filename:  gfx_postscript.h
 *
 *    Description:  wrapper around gl2ps
 *
 *        Version:  1.0
 *        Created:  01/14/2016 16:21:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

 
#ifndef  gfx_postscript_INC
#define  gfx_postscript_INC

#include "gl2ps/gl2ps.h"
#include <sstream>

namespace gfx {

struct PostScript {

  /*! @todo move output settings to separate io header  */
  bool bShadedOutput        = false; ///< default for output
  bool bSortOutput          = true;  ///< default 
  bool bOffsetOutput        = false; 
  bool bOccludeOutput       = false;
  bool bTightBounds         = true;
 
  // GL2PSrgba red = {1.0,0.0,0.0,1.0};
  // GL2PSrgba blue = {0.0,0.0,1.0,1.0};

  // GL2PSrgba table[2] = {red,blue};
//  GL2PSrgba blue[] = {{0.0,0.0,1.0,1.0}};

  template<class APPLICATION> 
  void print( APPLICATION& app ){
      static int id = 0;
      stringstream os; os << "output_" << id << (bShadedOutput ? ".eps" : ".pdf");
      id++;
      
      FILE *fp;
      int state = GL2PS_OVERFLOW, buffsize = 0;
      
      string name = os.str();
      fp = fopen(name.c_str(), "wb");
      
      printf("writing %s to %s\n", os.str().c_str(), name.c_str() );
      GLint tv[4];
      glGetIntegerv(GL_VIEWPORT, tv);
      
      
      while(state == GL2PS_OVERFLOW){

         buffsize += 1024*1024;

         gl2psEnable(GL2PS_BLEND);
         gl2psBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         gl2psPointSize(10);
         gl2psLineWidth(1);

         if (bShadedOutput){
          gl2psBeginPage("test", "gl2psTestSimple", tv, 
                         GL2PS_EPS,
                         bSortOutput ? GL2PS_SIMPLE_SORT : GL2PS_NO_SORT,
                         GL2PS_BEST_ROOT
                         | (bTightBounds ? GL2PS_TIGHT_BOUNDING_BOX : 0)
                         | GL2PS_SIMPLE_LINE_OFFSET
                         | GL2PS_OCCLUSION_CULL,
                         GL_RGBA, 0, NULL,
                         0, 0, 0,
                         buffsize, fp, "out.eps");
         } else {
          gl2psBeginPage("test", "gl2psTestSimple", tv,
                        GL2PS_PDF,
                        bSortOutput ? GL2PS_SIMPLE_SORT : GL2PS_NO_SORT,
                        GL2PS_NO_PS3_SHADING
                        | GL2PS_BEST_ROOT
                        | (bOffsetOutput ? GL2PS_SIMPLE_LINE_OFFSET : 0)
                        | (bTightBounds ? GL2PS_TIGHT_BOUNDING_BOX : 0)
                        | (bOccludeOutput ? GL2PS_OCCLUSION_CULL : 0),
                        //GL_COLOR_INDEX, 2, table,
                        GL_RGBA, 0, NULL,
                        0, 0, 0,
                        buffsize, fp, "out.eps");
        }
          

        app.onDraw();
        state = gl2psEndPage();
      }
      
      fclose(fp);
      printf("Done!\n");
  }

};

} //gfx::

#endif   /* ----- #ifndef gfx_postscript_INC  ----- */
