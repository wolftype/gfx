/*
 * =====================================================================================
 *
 *       Filename:  gfx_map.h
 *
 *    Description:  mappings of functions
 *
 *        Version:  1.0
 *        Created:  02/03/2015 13:36:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  gfx_map_INC
#define  gfx_map_INC


namespace gfx {

  namespace data {
    
      template<class T, class Lambda>
      vector<T> map(int w, int h, int d, Lambda func){
          vector<T> result;
          for (int i=0; i<w; ++i){
            for (int j=0; j<h; ++j){
              for (int k=0;k<d;++k){
                result.push_back( func((float)i/w,(float)j/h,(float)k/d) );
              }
            }
          }
          return result;
      }

      template<class T, class Lambda>
      vector<T> map(int w, int h, Lambda func){
        vector<T> result;
        for (int i=0;i<w;++i){
          for (int j=0;j<h;++j){
            result.push_back( func((float)i/w,(float)j/h) );
          }
        }
        return result;
      }
      template<class T, class Lambda>
      vector<T> map(int w, Lambda func){
        vector<T> result;
        for (int i=0;i<w;++i){
          result.push_back( func((float)i/w) );
        }
        return result;
      }


  }

}

#endif   /* ----- #ifndef gfx_map_INC  ----- */
