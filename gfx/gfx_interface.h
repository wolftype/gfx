// Interface container and event handling   THIS SHOULD BE IN A SEPARATE LIBRARY  > > >>

#ifndef GFX_INTERFACE_H_INCLUDED
#define GFX_INTERFACE_H_INCLUDED

#include "gfx_scene.h"
#include "gfx_glu.h" 
#include <sstream> 
#include <map>

namespace gfx{
  
   struct MouseData {

        enum  {
            Up = 1, Down, Left, Right
        };

		float x, y, dx, dy, ddx, ddy, xrel, yrel; //< 2D Mouse Position, first and second derivatives of motion, position relative to TL corner

        Vec3f origin, click, pos, move, accel, cat, drag, dragAccum, dragCat, projectFar, projectNear, projectMid;

		Vec3f biv, bivCat, dragBiv, dragBivCat;

        int gesture;  // stores major direction of mouse movement

		bool isDown, isMoving, newClick;		
		void toggle() { newClick = !newClick; }
	};

	struct ViewData  {
		float w, h; 
        Vec3f z; 
        Vec3f ray, clickray; 
	};

	struct KeyboardData {
		bool shift, alt, caps, ctrl, meta, f, down;
		int code;
		KeyboardData() : shift(0), alt(0), caps(0), ctrl(0), code(0){}
	};    

    /* Mapped Key values ripped from GLV */
    namespace Key{
        enum {

            // Standard ASCII non-printable characters
            Enter		=3,		/**< */
            Backspace	=8,		/**< */
            Tab			=9,		/**< */
            Return		=13,	/**< */
            Escape		=27,	/**< */
            Delete		=127,	/**< */

            // Non-standard, but common keys.
            F1=256, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
            Insert, Left, Up, Right, Down, PageDown, PageUp, End, Home
        };
    }

    /*! Abstract Interface Class */  
    class Interface {

        int mMode;  ///< Edit Mode State (not implemented)   
		float mRotVel, mVel, mModelRotVel;

    public:     
	
        Scene * scene; 

        typedef std::map<string,bool> SBMap;      

        /// Map of object addresses and whether they are selected
        SBMap selectMap;                            


        /*! Abstract View Implementation Data to store info from Windowing System */ 
 
        struct Impl {                      

            Interface * interface;  
			
            Impl(Interface * i) : interface(i) {}          // App must register with Interface  upon construction       

            virtual ~Impl(){}
            
			// MPose * camera;
			// MPose * model; 

            // All implementations should define fullScreenToggle method and getData methods
            virtual void fullScreenToggle() {};            
            virtual void getViewData( void * udata){};  
            virtual void getKeyboardData( void * udata){};
            virtual void getMouseData( void * udata){};
			virtual void getHIDData( void * udata ){}; //other input devic
        };

		
        Interface( Scene * s = NULL ) : scene(s), mRotVel(.02), mVel(.1), mModelRotVel(.1) {};     

        virtual void init() = 0;

        void inputCalc();        ///< Calculate Mouse Movements based on x and dx
        void viewCalc();         ///< Calculate Window Matrices, Screen Coordinates of Mouse

        Impl * impl;       		 ///< Implementation of Window Information (Width, Height) and Inputs (Keyboard, Mouse)  

		ViewData		view;
        MouseData       mouse;
        KeyboardData    keyboard; 

		ViewData& vd() { return view; }//vimpl -> data; }
        //ViewData vd() const { return vimpl -> data; }

        KeyboardData& kd() { return keyboard; }
        MouseData& md() { return mouse; }

		Camera& camera() { return scene -> camera; }		
		MPose& model() { return scene -> model; }
        
		template <class A> static Vec3f screenCoord(const A& p, const XformMat& );
        template <class A> bool pntClicked(const A&, float rad = .05);  

        Vec3f click(){ return mouse.click;   }
        Vec3f pos(){ return mouse.pos;     } 

        /*! Set Mode */
        int& mode() { return mMode; }
        /*! Get Mode */
        int mode() const { return mMode; }
        /// Check Interface Mode
        bool mode(int q) { return mMode & q; }  
       /// Enable Mode    
        void enable(int bitflag) { mMode |= bitflag; }
        void disable(int bitflag) { mMode &= ~bitflag; }
        void toggle(int bitflag)  { mMode & bitflag ? disable(bitflag) : enable(bitflag); }  

        template <class A > bool isSelected( A * );
        template <class A > void select( A * );
        template <class A > void deselect( A * );
        template <class A > void toggleSelect( A * );  

	        //NAVIGATION
	        void keyboardCamSpin(float acc, bool trigger);
	        void keyboardCamTranslate(float acc, bool trigger);
	        void keyboardModelTransform(float acc, bool trigger);
	        void mouseModelTransform(float acc, bool trigger);
	        void mouseCamTranslate(float acc, bool trigger);
	        void mouseCamSpin(float acc, bool trigger);  
	
			void mouseNavigate();
			void mouseNavigateStop();  
			void keyboardNavigate();
			void keyboardNavigateStop();

	        virtual void onMouseMove();        
	        virtual void onMouseDown();
	        virtual void onMouseDrag();
	        virtual void onMouseUp();        
	        virtual void onKeyDown();
	        virtual void onKeyUp();  
	
			virtual void windowTransform(){}     
			
			
			template<class A, class B> 
			void touch( A& s, const B& b, float t = .1 ); 
			
			//Define this transformation rule somewhere
		    template <class A, class B> 
		   	struct X { 
				Interface * i;
				X( Interface * _i) : i (_i ) {}
				void f ( A * s, const B& pos, float t );
			};
		// void xf ( A * s, const B& pos, float t );
			
			
};   

      template< class A > bool Interface :: isSelected ( A * a ){
        stringstream s; s << a;
        bool tmp = selectMap[ s.str() ];
        return tmp;
    }

    template < class A > void Interface :: select( A * a) {
        stringstream s; s << a;  
		//cout << "selected: " << s.str() << endl; 
        selectMap[ s.str() ] = true;
    }

    template < class A > void Interface :: deselect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ] = false;
    }
    
    template < class A > void Interface :: toggleSelect( A * a) {
        stringstream s; s << a;
        selectMap[ s.str() ] = !selectMap[ s.str() ];
    }
 /// Screen Coordinates of Target point
 template <class A> Vec3f Interface :: screenCoord(const A& p, const XformMat& xf){
     Vec3f sc = GL::project( p[0], p[1], p[2], xf );
     sc[0] /= xf.viewport[2]; sc[1] /= xf.viewport[3]; sc[2] = 0;
     
     return sc;
 }       

 template <class A> bool Interface :: pntClicked( const A& x, float rad ) {
     Vec3f v = mouse.click;  // [0,0] is bottom left corner [1.0,1.0] is top right
     Vec3f p = Vec3f( x[0], x[1], x[2] );
     
   // cout << "mouse: " << v << endl; 
  //  cout << "x: " << p << endl; 

     Vec3f sc = screenCoord( p, scene -> xf );
   // cout << "Screen: " << sc << endl ;
     Vec3f dist = (v - sc);
     return (dist.len() < rad) ? true : false;
 } 

template <class A, class B> void Interface :: touch( A& s, const B& x, float t){
        
        if ( !keyboard.alt && !keyboard.shift && !keyboard.alt ){
        
            //physics
            static float dt = 1;
            static float acc = .9;
            dt *= acc;
            
            if ( mouse.isDown ){
                dt = t; // Reset acc
                if ( pntClicked( x ) ) {
                    select( &s );
                }
            }
            
            if ( isSelected( &s ) ){
				 X<A,B>(this).f(&s, x, dt);
				//xf(&s, x, dt);
            }
        
        }
    }
 
inline void Interface::viewCalc(){
        
        //Assumes data has been copied to (or created from) scene transformation matrices (xf) 
        vd().z  = Quat::spin( Vec3f(0,0,1), !model().quat() * camera().quat() ); 
         
        /// Bottom Left (0,0) to top right (1,1)
        mouse.pos  = Vec3f( mouse.x / vd().w, 1 - mouse.y / vd().h, 0 ) ;
        mouse.move = Vec3f( mouse.dx / vd().w, - mouse.dy / vd().h, 0 ) ;
        
        mouse.cat     = Quat::spin( mouse.move * -1, !scene->cat() );
        mouse.bivCat = vd().z.cross( mouse.cat );

        //GLU FUNCS
        Vec3f v1 = gfx::GL::unproject( mouse.x, vd().h - mouse.y , 1.0,  scene->xf );
        Vec3f v2 = gfx::GL::unproject( mouse.x, vd().h - mouse.y , 0.0,  scene->xf );
        Vec3f v3 = gfx::GL::unproject( mouse.x, vd().h - mouse.y , 0.5,  scene->xf );   

        //OWN FUNCS (in progress, seem to give results scaled differently)
//        Vec3f tv1 = XMat::UnProject( Vec3f( mouse.x, vd().h - mouse.y , 1.0), scene().xf );
//        Vec3f tv2 = XMat::UnProject( Vec3f( mouse.x, vd().h - mouse.y , 0.0),scene().xf );
//        Vec3f tv3 = XMat::UnProject( Vec3f( mouse.x, vd().h - mouse.y , 0.5),scene().xf );
//        
//        Vec v1(tv1.x, tv1.y, tv1.z);
//        Vec v2(tv2.x, tv2.y, tv2.z);
//        Vec v3(tv3.x, tv3.y, tv3.z);
        
   
        //Get vec of Mouse Position into Z Space 
		vd().ray	 = ( v3 - v2 ).unit();
        
		mouse.projectFar	= v1 ;
		mouse.projectNear	= v2 ;
		mouse.projectMid	= v3 ;

        mouse.biv     = mouse.pos.cross( vd().ray ); //not used?

        //Point on Line Closest to Origin (moved to versor)
        //mouse.origin = Ro::null( Fl::loc( vd().ray, Ori(1), true ) );        

    }



	    inline void Interface :: onKeyDown(){
            
	        keyboard.down = true;  
	
	    }


	   inline void Interface :: onKeyUp(){

	        keyboard.down = false;



	    }

	   inline void Interface :: keyboardModelTransform(float acc, bool trigger){

	        model().ab() = acc;

	        if (trigger) {
			   //printf("1\n");
	            // Get Rotor Ratio between camera and model view
				Quat ryz = Quat::Rotor( model().x(), camera().x() );
	            Quat rxz = Quat::Rotor( model().y(), camera().y() );

	            // Rotate By said Rotor
	            Vec3f tyz = Quat::spin( model().x(), ryz );
	            Vec3f txz = Quat::spin( model().y(), rxz );

	            switch( keyboard.code ){
	                case Key::Up:
	                {    
	                    // camera().modelView().yz()
	                    model().db() += tyz * mModelRotVel;	
	                    break;
	                }
	                case Key::Down:  
	                {
	                    model().db() -= tyz * mModelRotVel;	
	                    break;
	                }
	                case Key::Left:
	                {
	                    model().db() -= txz * mModelRotVel;	
	                    break;
	                }
	                case Key::Right: 
	                {
	                    model().db() += txz * mModelRotVel;	
	                    break;
	                }
	            }
	        }
	    }

	    inline void Interface :: keyboardCamTranslate(float acc, bool trigger){
	        camera().ax() = acc;
	        if (trigger){
	            switch(keyboard.code){
	                case Key::Up:
	                    if (keyboard.ctrl) camera().dx() += camera().up() * mVel;
	                    else camera().dx() += camera().forward() * mVel;	
	                    break;
	                case Key::Down:
	                    if (keyboard.ctrl) camera().dx() -= camera().up() * mVel;  
	                    else camera().dx() -= camera().forward() * mVel; 
	                    break;
	                case Key::Left:  camera().dx() -= camera().right() * mVel; break;
	                case Key::Right: camera().dx() += camera().right() * mVel; break;							
	            }
	        }
	    }

	    inline void Interface :: keyboardCamSpin(float acc, bool trigger){
	        camera().ab() = acc; 
	        if (trigger){
	            switch(keyboard.code){
	                case Key::Up:    camera().db() += camera().x() * mRotVel;	break;
	                case Key::Down:  camera().db() -= camera().x() * mRotVel; break;
	                case Key::Left:  camera().db() += camera().y() * mRotVel; break;
	                case Key::Right: camera().db() -= camera().y() * mRotVel; break;							
	            }
	        }
	    } 
	
	  inline void Interface :: mouseModelTransform(float acc, bool trigger){
	        model().ab() = acc;
	        if (trigger) model().db() += mouse.dragBiv * mModelRotVel;
	    }

	  inline void Interface :: mouseCamTranslate(float acc, bool trigger){
	        camera().ax() = acc;
	        if(trigger) {

	            switch(mouse.gesture){
	                case MouseData::Up:
	                    if (keyboard.ctrl) camera().dx() += camera().up() * mVel;
	                    else camera().dx() += camera().forward() * mVel * .5;	
	                    break;
	                case MouseData::Down:
	                    if (keyboard.ctrl) camera().dx() -= camera().up() * mVel;  
	                    else camera().dx() -= camera().forward() * mVel * .5; 
	                    break;
	                case MouseData::Left:
	                   camera().dx() += camera().right() * mVel * .5; break;
	                case MouseData::Right:
	                    camera().dx() -= camera().right() * mVel * .5; break;            
	            }

	        }
	    }

	   inline void Interface :: mouseCamSpin(float acc, bool trigger){
	        camera().ab() = acc; 
	        if (trigger){
	            switch(mouse.gesture){
	                case MouseData::Up:    camera().db() += camera().x() * mRotVel * .5;	break;
	                case MouseData::Down:  camera().db() -= camera().x() * mRotVel * .5; break;
	                case MouseData::Left:  camera().db() -= camera().y() * mRotVel * .5; break;
	                case MouseData::Right: camera().db() += camera().y() * mRotVel * .5; break;							
	            }
	        }
	    }  
	
		   inline void Interface :: onMouseMove(){

		    }


		   inline void Interface :: onMouseDown(){

		        vd().clickray = vd().ray;
		        mouse.click = mouse.pos;				
		        mouse.isDown  = 1;
		        mouse.newClick = 1;

		    }


		   inline void Interface :: onMouseUp(){
		        mouse.isDown = 0; 
		        mouse.newClick = 0;
		    }


		   inline void Interface :: onMouseDrag(){			
         
			        static float nx;
		            static float ny; 

		            static float tdx;
		            static float tdy; 
		                                                 
		            //relative top left (0.0 - .002 or so);
		            float dx = mouse.dx;
		            float dy = mouse.dy; //changed	

		            //current position relative to top left (0.0 - 1.0)
		            float cx = mouse.pos[0];//mouse.xrel;
		            float cy = mouse.pos[1];//mouse.yrel;//changed

		            //Accumulated Movement
		            nx += dx; //+ or -?
		            ny += dy;

		            //Temporary Accumulated Movement
		            tdx = mouse.newClick ? 0 : tdx + dx; //+ or -?
		            tdy = mouse.newClick ? 0 : tdy + dy;

		            mouse.dragAccum = Vec3f(nx,ny,0);							//total dvector since program launch
		            mouse.drag		= Vec3f(tdx,tdy,0);							//vector from last click and hold to current position

		            mouse.dragCat = Quat::spin( mouse.drag, !scene->cat() );     //rotate drag by inverse concatenated orientation
		            mouse.dragBivCat = vd().z.cross( mouse.dragCat );

		            mouse.dragBiv = Vec3f(0,0,1).cross(mouse.drag); 

		            Vec3f v1  = mouse.pos;
		            Vec3f v2  = mouse.move;
		            Vec3f v = v1 - camera().pos();

		            Vec3f rel ( .5-cx, .5-cy, 0.0 );
		            Vec3f nn  ( nx, ny, 0.0 );					

		            mouse.newClick = 0;


		             int mdir = 0;
		             if ( fabs(tdx) > fabs(tdy) ) mdir = tdx > 0 ? MouseData::Right : MouseData::Left;
		             else if ( fabs(tdx) < fabs(tdy) ) mdir = tdy > 0 ? MouseData::Up : MouseData::Down; 

		             mouse.gesture = mdir;
   
		}          
		
		inline void Interface :: keyboardNavigate(){
			//Camera Controls			               
			
			//stateTransform();
	        switch(keyboard.code){
	            case 'c':
	                camera().reset(0,0,5); 
	                model().reset();
	                break;

	        }
			
	        if (keyboard.alt) {	
	            keyboardModelTransform(1.0, true);
	        } else if (keyboard.shift){   
	            keyboardCamTranslate(1.0, true);
	        } else {                         
	            keyboardCamSpin(1.0, true);
	        }
		}  
		
		inline void Interface :: keyboardNavigateStop(){
	        keyboardModelTransform(.7, false);
	        keyboardCamTranslate(.7, false);
	        keyboardCamSpin(.7, false);
		}
		
		inline void Interface :: mouseNavigate(){
		        if (keyboard.alt) {	
	                mouseModelTransform(1.0, true);
	            } else if (keyboard.shift) {	
	                mouseCamTranslate(1.0, true);
	            } else if (keyboard.ctrl){
	                mouseCamSpin(1.0,true);
	            }  
		} 
		
		inline void Interface :: mouseNavigateStop(){
			mouseModelTransform(.7, false);
	        mouseCamTranslate(.7, false);
	        mouseCamSpin(.7, false);
		}
	
} //gfx::  


#endif
