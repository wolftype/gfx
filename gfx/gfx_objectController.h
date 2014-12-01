/*
 * =====================================================================================
 *
 *       Filename:  gfx_objectController.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/11/2014 15:09:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  gfx_objectController_INC
#define  gfx_objectController_INC

#include "gfx_lib.h"
#include "gfx_gl.h"
#include "gfx_glu.h"
#include "gfx_xfmatrix.h"
#include "gfx_control.h"
#include "gfx_scene.h"

#include <sstream>
#include <map>

namespace gfx {


    class ObjectController{

        //Pointer to IO data container
        GFXio * mIO;
        //Pointer to scene's transformations
        Scene * mScene;

        /// Map of object addresses
        typedef std::map<string,bool> SBMap;      
        /// Map of object addresses and whether they are selected
        SBMap selectMap;   
        
        public:
        
        void io(GFXio * io) { mIO=io; }
        GFXio& io(){ return *mIO; }
        
        Scene& scene() { return *mScene; }
        void scene(Scene *s ){ mScene=s; } 
              
        template <class A> static Vec3f screenCoord(const A& p, const XformMat& );
        template <class A> static Vec3f screenCoord2D(const A& p, const XformMat& );
        template <class A> bool pntClicked(const A&, float rad, const XformMat& );   

        template <class A > bool isSelected( A * );
        template <class A > void select( A * );
        template <class A > void deselect( A * );
        template <class A > void toggleSelect( A * );  

        /// What happens when you touch an Object . . .  
        template<class A, class B> 
        void onTouch( A& s, const B& b, float t ); 
      
        /// Define this transformation rule somewhere in your implementation
        template <class A, class B> 
         struct X { 
          ObjectController * i;
          X( ObjectController * _i) : i (_i ) {}
          void f ( A * s, const B& pos, float t );
        };

    };
 
    template< class A > bool ObjectController :: isSelected ( A * a ){
        stringstream s; s << a;
        bool tmp = selectMap[ s.str() ];
        return tmp;
    }

    template < class A > void ObjectController :: select( A * a) {
        stringstream s; s << a;  
        selectMap[ s.str() ] = true;
    }

    template < class A > void ObjectController :: deselect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ] = false;
    }
    
    template < class A > void ObjectController :: toggleSelect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ] = !selectMap[ s.str() ];
    }
    /// Screen Coordinates of Target point
    template <class A> Vec3f ObjectController :: screenCoord(const A& p, const XformMat& xf){
       Vec3f sc = GL::project( p[0], p[1], p[2], xf );
       sc[0] /= xf.viewport[2]; sc[1] /= xf.viewport[3]; sc[2] = 0;     
       return sc;
    } 
 
    /// Screen Coordinates of Target 2D point
    template <class A> Vec3f ObjectController :: screenCoord2D(const A& p, const XformMat& xf){
         Vec3f sc = GL::project( p[0], p[1], 0, xf );
         sc[0] /= xf.viewport[2]; sc[1] /= xf.viewport[3]; sc[2] = 0;      
         return sc;
    }          

    template <class A> bool ObjectController :: pntClicked( const A& x, float rad, const XformMat& xf ) {
      Vec3f v = io().click();  // [0,0] is bottom left corner [1.0,1.0] is top right
      Vec3f p = Vec3f( x[0], x[1], x[2] );
      Vec3f sc = screenCoord( p, xf );
      Vec3f dist = (v - sc);
      return (dist.len() < rad) ? true : false;
    } 

//touch A s wrt B x note: add a radius test, and only call onInput . . .
template <class A, class B> void ObjectController :: onTouch( A& s, const B& x, float t ){
        
        if ( !io().keyboard.alt() && !io().keyboard.shift() ){
        
            //touch physics
            static float dt = 1;
            static float acc = .9;
            dt *= acc;

            if ( io().mouse.isDown() ){
                
                dt = t; // Reset acc
                if ( pntClicked( x, .05, mScene->xf ) ) {
                    select( &s );
                }
            }
            
          //user defined response to selection
          if ( isSelected( &s ) ){
            X<A,B>(this).f(&s, x, dt);
          }
        }
    }
} //gfx:: 

#endif   /* ----- #ifndef gfx_objectController_INC  ----- */
