/*
 * =====================================================================================
 *
 *       Filename:  gfx_layout.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/05/2015 15:42:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */



#ifndef  gfx_layout_INC
#define  gfx_layout_INC

#include "gfx_scene.h"

namespace gfx{

     /// MONITORS Row-Column GRID layout, use 1 x 1 for a single screen
    struct Layout{  
  
      ///Constructor defaults width of 21.5 and height of 14.5 (asus)  
      //
      Layout( int M, int N, float w = 21.5, float h = 14.5, float wb = 1.0, float hb = 1.0) :
      numRow(M), numCol(N), screenWidth(w), screenHeight(h), wPitch(wb), hPitch(hb) {}
    
      int numRow; int numCol;                 ///< Number of Rows and Columns in Multidisplay
      float screenWidth; float screenHeight;   ///< Width of individual screens in inches
      float wPitch; float hPitch;              ///< Bezel

      Vec3f speakerL, speakerR;               ///< Speaker positions      
    
      //Total Width of MxN Display
      float totalWidth(){
        return numCol * screenWidth + (numCol-1) * wPitch;
      }  
      
      //Total Height of MxN Display
      float totalHeight(){
        return numRow * screenHeight + (numRow-1) * hPitch;
      } 
      
      //gets Pose (bottom left corner) of Mth Row and Nth Col screen
      Pose poseOf( int M, int N){
        cout << "BOTTOM " << left(M,N) << " " << bottom(M,N) << endl; 

        return Pose( 
          left(M,N),
          bottom(M,N), 
          0 );
      }  
      
      //Absolute (and GL coord) position of left corner of M, N monitor 
      float left(int M, int N){
        return - totalWidth() / 2.0 + N * screenWidth + (N) * wPitch;
      }  
         
      //absolute (and GL coord) position of right corner of M, N monitor
      float bottom(int M, int N){
        return totalHeight() / 2.0 - ( M * screenHeight + (M) * hPitch ) - screenHeight;
      }
    
  };


} //gfx::

#endif   /* ----- #ifndef gfx_layout_INC  ----- */
