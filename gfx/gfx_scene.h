#ifndef GFX_SCENE_INCLUDED
#define GFX_SCENE_INCLUDED   

#include "gfx_gl.h" 
#include "gfx_xfmatrix.h"

namespace gfx{   

    struct Slab{
        Vec3f blu, bru, tru, tlu;
        Vec3f& operator[] (int i ) { return (&blu)[i]; }
    };

    struct Volume{
        Vec3f bl, br, tr, tl, blb, brb, trb, tlb; 
        Vec3f& operator[] (int i ) { return (&bl)[i]; }
    }; 

	struct Frustrum{
	    float width, height, depth;
	    float bwidth, bheight;

	    Slab dir;
	    Volume box;

	    void calc(){
	        dir.blu = (box.blb - box.bl).unit();
	        dir.tru = (box.trb - box.tr).unit();
	        dir.bru = (box.brb - box.br).unit();
	        dir.tlu = (box.tlb - box.tl).unit();
	        width = box.br[0] - box.bl[0];
	        height = box.tr[1] - box.br[1];
	        depth = box.blb[2] - box.bl[2];
	        bwidth = box.brb[0] - box.blb[0];
	        bheight =box.trb[1] - box.brb[1];

	    }

	    // Pln left(){ return Ro::null(box.tl) ^ Ro::null(box.bl) ^ Ro::null(box.blb) ^ Inf(1); }
	    // Pln right(){ return Ro::null(box.tr) ^ Ro::null(box.br) ^ Ro::null(box.brb) ^ Inf(1); }
	    // Pln top(){ return Ro::null(box.tl) ^ Ro::null(box.tr) ^ Ro::null(box.trb) ^ Inf(1); }
	    // Pln bottom(){ return Ro::null(box.bl) ^ Ro::null(box.br) ^ Ro::null(box.brb) ^ Inf(1); } 
	};

	struct Lens {


	    /* Lens Parameters */
	    bool bOrtho;
	    float mFocal, mNear, mFar, mWidth, mHeight; 

	    Lens() :
	    mFocal(60.0), 
	    mWidth(100), 
	    mHeight(100), 
	    mNear(0.1), 
	    mFar (100.0),
	    bOrtho(0)
	    {}

	    /* Implicit Getters and Setters */
	    void 	width ( float w )	{ mWidth = w;}						///< set width
	    void 	height( float h )	{ mHeight = h;}						///< set height
	    
		float 	ratio() 	const { return width()/ height(); }
	    float 	width()		const {return mWidth;}							///< get width
	    float 	height()	const {return mHeight;}							///< get height
	    float 	depth()   	const { return mFar - mNear; }     
	
	    float& 	width()	 	{return mWidth;}							///< get width
	    float& 	height()	{return mHeight;}							///< get height

	    float near() const { return mNear; }
	    float far() const { return mFar; }
	    float& near()  { return mNear; }
	    float& far()  { return mFar; }

	    void	focal(float f)	{ mFocal = f; }						///< set focal length
	    float& focal()			{ return mFocal; }					///< get focal length
	    float	focal() const	{ return mFocal; }

	    void  ortho(bool b) { bOrtho = b; }
	    bool& ortho()  {return bOrtho; }
	    bool  ortho() const {return bOrtho; }

	    friend ostream& operator << (ostream& os, const Lens& l){
	        os << "focal: " << l.focal() << "\n";
	        os << "width: " << l.width();
	        os << " height: " << l.height() << "\n";
	        os << "near: " << l.near();
	        os << " far: " << l.far() << "\n";

	        return os;
	    }    

	};

	struct Camera{     
		
		Lens lens;
		
		Vec3f eye;
		Vec3f up;
		Vec3f forward; 
		
		template<class T>
		Camera& set(const T& t){
			eye = t.pos();
			up = t.y();
			forward = -t.z();  
			return *this;
		}    
		
		//lookat etc
	}; 
	
	struct Scene {

	  Camera cam;
      
		void fit(int w, int h){
			cam.lens.width( w ); 
			cam.lens.height( h ); 
		}

	  void push(){
			
			GL :: enablePreset();    
				
		    glMatrixMode(GL_MODELVIEW);
		    glPushMatrix();
		    glLoadIdentity();
		    gluLookAt( cam.eye.x, cam.eye.y, cam.eye.z, cam.forward.x, cam.forward.y, cam.forward.z, cam.up.x, cam.up.y, cam.up.z);

		}

		void pop(){
            glPopMatrix();  
			
			GL :: disablePreset();  
	   }                                           

	};
	
}     

#endif