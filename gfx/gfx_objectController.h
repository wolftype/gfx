/*
 * =====================================================================================
 *
 *       Filename:  gfx_objectController.h
 *
 *    Description:  use map of address in memory to mark object as selected or not 
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
#include "gfx_xfmatrix.h"
#include "gfx_control.h"
#include "gfx_scene.h"

#include <sstream>
#include <map>

namespace gfx {

    class ObjectController : public InputEventHandler, public WindowEventHandler {

        //Pointer to IO data container
        GFXio * mIO;
        //Pointer to scene's transformations
        Scene * mScene;

        struct GenericObjectPtr {
          virtual void update()= 0;     ///< called on frame if selected
          virtual void store() = 0;     ///< called on mousedown if selected
          virtual void touch() = 0;     ///< called on mouse drag if selected

          Vec3f pos;                    ///< Position in OpenGL 3D Space

          bool select=false;            ///< is selected
          bool active=true;             ///< is active
          bool remote=false;            ///< is controlled by some other object

          GenericObjectPtr * remotePtr;    ///< some remote controller
        };

        //vector<GenericObjectPtr*> mObjects;

        typedef std::map<string,GenericObjectPtr*> SelectMap;   
        typedef SelectMap::iterator SelectMapIterator;     
        /// Map of object addresses and information about each
        SelectMap selectMap;   
        
        public:
        
        void io(GFXio * io) { mIO=io; }
        GFXio& io(){ return *mIO; }
        
        Scene& scene() { return *mScene; }
        void scene(Scene *s ){ mScene=s; } 
              
      //template <class A> Vec3f screenCoord(const A& p );
      //template <class A> Vec3f screenCoord2D(const A& p );
        template <class A> bool pntClicked(const A&, float rad );   

        /// Mouse Movement Relative to ModelView (belongs elsewhere
        Vec3f dragCat(){
          return Quat::spin( mIO->drag(), !mScene->cat() );
        }
        /// Axis of Mouse Movement  Relative to ModelView (belongs elsewhere
        Vec3f axisCat(){
          return Quat::spin( mIO->axis(), !mScene->cat() );
        }
        template <class A > bool isSelected( A * );
        template <class A > void select( A * );
        template <class A > void deselect( A * );
        template <class A > void toggleSelect( A * );  

        /// What happens when you touch an Object . . .  
        template<class A, class B> 
        void onTouch( A& s, const B& b, float t );

        template<class A, class B>
        void touch(A& s, const B& b);
      
        /// ObjectPtr Stores Address and Temp Copy of Object 
        //  (And ... to do: maybe some function for extracting position from object ...)
         template <class A>  
         struct ObjectPtr : GenericObjectPtr { 

          ObjectPtr( ObjectController * _i, A * a = NULL ) 
          : i (_i), mAddress(a), tObject() {}

          /// controller
          ObjectController * i;

          /// address
          A * mAddress;
  
          /// temp memory
          A tObject;
          
          /// User-defined Position of Object
          Vec3f worldPosition();

          /// User-defined Transformation of Object
          void transform(); 

          /// Update Object's Position in ObjectController's Map
          virtual void update(){
           // stringstream s; s << mAddress;  
            if (!remote){
              pos = worldPosition();
            } else {
              remotePtr->update();
              pos = remotePtr->pos;
            }
          }

          /// Store Object Temporarily
          virtual void store(){
            tObject = *mAddress;
          }

          virtual void touch(){
            transform();
          }

        };

        /// Attach a pointer to an object -- adds address of s to selection map
        template<class A>
        void attach(A *a ){          
          stringstream s; s << a;  
          selectMap[ s.str() ] = new ObjectPtr<A>(this,a); 
        }

        /// Attach a pointer to an object using another object to control it
        /// useful for linkage mechanisms where the local in-joint transform 
        //  is controlled by some the location of some final kinematic result
        template<class A, class B>
        void attach(A *a, B *b ){          
          stringstream sa; sa << a;  
          selectMap[ sa.str() ] = new ObjectPtr<A>(this,a); 
          stringstream sb; sb << b;  
          selectMap[ sb.str() ] = new ObjectPtr<B>(this,b); 

          selectMap[ sa.str() ] -> remote = true;
          selectMap[ sa.str() ] -> remotePtr = selectMap[sb.str()];

        }

        virtual void onMouseDown(const Mouse& m);
        virtual void onMouseDrag(const Mouse& m);

        /// Called at Frame Rate (i.e. for updating ObjectPtr data)
        virtual void onFrame();

    };


    
    /*-----------------------------------------------------------------------------
     *  Select Object by Address in Memory
     *-----------------------------------------------------------------------------*/
    template< class A > bool ObjectController :: isSelected ( A * a ){
        stringstream s; s << a;
        bool tmp = selectMap[ s.str() ] -> select ;
        return tmp;
    }

    template < class A > void ObjectController :: select( A * a) {
        stringstream s; s << a;  
        selectMap[ s.str() ]->select  = true;
    }

    template < class A > void ObjectController :: deselect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ]->select  = false;
    }
    
    template < class A > void ObjectController :: toggleSelect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ]->select  = !selectMap[ s.str() ]->select ;
    }


    /*-----------------------------------------------------------------------------
     *  Get Screen Coordinates of Object
     *-----------------------------------------------------------------------------*/
    /// Screen Coordinates of Target point
    /* template <class A> Vec3f ObjectController :: screenCoord(const A& p){ */
    /*    Vec3f sc = mScene->project(p);// XMat::Project( p, xf ); */
    /*    return sc; */
    /* } */ 
 
    /* /// Screen Coordinates of Target 2D point */
    /* template <class A> Vec3f ObjectController :: screenCoord2D(const A& p){ */
     
    /*     XformMat& xf = mScene->xf; */
    /*     Vec3f sc = GL::project( p[0], p[1], 0, xf ); */
    /*     sc[0] /= xf.viewport[2]; sc[1] /= xf.viewport[3]; sc[2] = 0; */     
    /*     return sc; */
    /* } */          

    template <class A> bool ObjectController :: pntClicked( const A& x, float rad ) {
      Vec3f v = io().click();  // [0,0] is bottom left corner [1.0,1.0] is top right
      Vec3f p = Vec3f( x[0], x[1], x[2] );
      Vec3f mp = io().mouse.click();
      Vec3f sc = mScene->project(p);//screenCoord( p );
      Vec3f dist = (v - sc);
      return (dist.len() < rad) ? true : false;
    } 

    void ObjectController :: onMouseDown(const Mouse& m){
      
      if (io().mode( ControlMode::Edit )){
        for (auto& i : selectMap){
          if (i.second->active) {
            if ( pntClicked(i.second->pos, .05 ) ) {
              i.second->select=true;
            }  
            if (i.second->select) i.second->store();      
          }
        }
      }
    }

    void ObjectController :: onMouseDrag(const Mouse& m){
      if (io().mode( ControlMode::Edit )){

        for (auto& i : selectMap){
          if (i.second->active) {
            if (i.second->select) i.second->touch();
          }
        }
      }
    }

    void ObjectController :: onFrame(){
      if (io().mode( ControlMode::Edit) ){
        for (auto& i : selectMap){
          if (i.second->active) {
            i.second->update();
          }
        }
      }
    }


    /* template<class A, class B> void ObjectController :: touch(A& s, const B& b){ */
    /*   if (isSelected(&s)) Object<A,B>(this).transform(&s,b); */
    /* } */


     /* //touch A s wrt B x note: add a radius test, and only call onInputEvent . . . */
    /* template <class A, class B> void ObjectController :: onTouch( A& s, const B& x, float t ){ */
         
        
     /*    if ( !io().keyboard.alt() && !io().keyboard.shift() ){ */
        
     /*        //touch physics */
     /*        static float dt = 1; */
     /*        static float acc = .9; */
     /*        dt *= acc; */

     /*        if ( io().mouse.isDown() ){ */
     /*            dt = t; // Reset acc */
     /*            if ( pntClicked( x, .05 ) ) { */
     /*                select( &s ); */
     /*            } */
     /*        } */
            
     /*        //user defined response to selection */
     /*        if ( isSelected( &s ) ){ */
     /*          X<A,B>(this).f(&s, x, dt); */
     /*        } */

     /*    } */
    /* } */


} //gfx:: 

#endif   /* ----- #ifndef gfx_objectController_INC  ----- */
