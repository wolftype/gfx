/*
 * =====================================================================================
 *
 *       Filename:  gfx_layout.h
 *
 *    Description:  layout of monitors and speakers
 *
 *        Version:  1.0
 *        Created:  02/18/2014 17:18:26
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
  
      ///Constructor defaults width of 1920 and height of 1080 (asus)  
      //
      Layout( int M=4, int N=4, float w = 1920, float h = 1080, float wb = 100, float hb = 100) :
      numRow(M), numCol(N), screenWidth(w/100), screenHeight(h/100), wPitch(wb/100), hPitch(hb/100) {}

      void set(int M, int N, float w, float h, float wb, float hb){
        numRow = M; numCol = N; screenWidth = w/100; screenHeight=h/100; wPitch = wb/100; hPitch = hb/100;
      }
    
      int numRow; int numCol;                 ///< Number of Rows and Columns in Multidisplay
      float screenWidth; float screenHeight;  ///< Width of individual screens in pixels
      float wPitch; float hPitch;             ///< Bezel width and height in pixels

      Vec3f speakerL, speakerR;               ///< Speaker positions      
    
      //Total Width of MxN Display IN PIXELS or INCHES?
      float totalWidth(){
        return numCol * screenWidth + (numCol-1) * wPitch;
      }  
      
      //Total Height of MxN Display IN PIXELS or INCHES?
      float totalHeight(){
        return numRow * screenHeight + (numRow-1) * hPitch;
      } 
      
      //gets Pose (bottom left corner) of Mth Row and Nth Col screen
      Pose poseOf( int M, int N){
        //cout << "BOTTOM " << left(M,N) << " " << bottom(M,N) << endl; 

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
