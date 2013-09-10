#ifndef GFX_SCENE_INCLUDED
#define GFX_SCENE_INCLUDED   

#include "gfx_gl.h" 
#include "gfx_xfmatrix.h" 
#include "gfx_glsl.h" 


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
    
	struct Pose {
		Pose(Vec3f p, Quat q = Quat(1,0,0,0)) : mPos(p), mQuat(q) { orient(); }
		Pose(float x, float y, float z) : mPos(x,y,z), mQuat(1,0,0,0) { orient(); }
		Pose() : mX(1,0,0), mY(0,1,0), mZ(0,0,1), mPos(0,0,0), mQuat(1,0,0,0) {}
	   
	 	Vec3f mX, mY, mZ, mPos;
		Quat mQuat;
		
		Quat quat() const { return mQuat; }
		Quat & quat() { return mQuat; }
		
		Pose& quat( Quat q ) { mQuat = q; return orient(); }

		Pose& orient() {
		    mX = Quat::spin( Vec3f(1,0,0), mQuat);
		    mY = Quat::spin( Vec3f(0,1,0), mQuat);
		    mZ = Quat::spin( Vec3f(0,0,1), mQuat);			
			return *this;
		}
        
		Vec3f px()  const{ return mPos + mX; }
		Vec3f py()  const{ return mPos + mY; }
		Vec3f pz()  const{ return mPos + mZ; }
		
		Vec3f x()  const{ return mX; }
		Vec3f y()  const{ return mY; }
		Vec3f z()  const{ return mZ; }
		

 		Vec3f& x() { return mX; }
		Vec3f& y() { return mY; }
		Vec3f& z() { return mZ; }

		Vec3f pos()  const{ return mPos; }
		Vec3f& pos() { return mPos; }
	};

	struct Camera : public Pose {     
		
		Lens lens;
		
		Vec3f eye(){ return mPos; }
		Vec3f up() { return mY; }
		Vec3f forward() { return -mZ; }

		
		template<class T>
		Camera& set(const T& t){
			mPos = t.pos();
			mY = t.y();
			mX = t.x();
			mZ =  t.z();  
			return *this;
		}    
		
		//lookat etc
	}; 
	
	struct Scene {

		 Camera cam;
		 Pose model;
		 XformMat xf; 
		 Pipe pipe; 
      
		void fit(int w, int h){
			cam.lens.width( w ); 
			cam.lens.height( h ); 
		} 

	  // FIXED FUNCTION PIPELINE ONLY 
	
	  #ifdef GL_IMMEDIATE_MODE
	  void push(){
		
			GL :: enablePreset();    
			
		    glMatrixMode(GL_MODELVIEW);
		    glPushMatrix();
		    glLoadIdentity();
		    gluLookAt( cam.eye().x, cam.eye().y, cam.eye().z, cam.forward().x, cam.forward().y, cam.forward().z, cam.up().x, cam.up().y, cam.up().z);

		}

	  void pop(){
            glPopMatrix();  
			GL :: disablePreset();  
	   }  
	 
	  #endif  
	


	   	//Mat4f mod() { return model.image(); }
        //Mat4f mvm() { return  XMat::lookAt( camera.x(), camera.y(), camera.z() * -1, camera.pos()) * XMat::rot( model.rot() ) ; }

          Mat4f mod() { return XMat::rot( model.quat() ); }

		 
		//	Mat4f mvm() { return XMat::trans(0,0,-2); }// XMat::identity() * mod(); }
			Mat4f mvm() { 
				return XMat::lookAt( cam.x(), cam.y(), cam.z(), cam.pos() ) * mod(); 
			}

			 // Mat4f proj() { return XMat::identity(); }
            
			Mat4f proj() {    
				Lens& tl = cam.lens;
                return XMat::fovy( tl.mFocal * PI/180.0, tl.mWidth/tl.mHeight, tl.mNear, tl.mFar ); 
	         }
	
			// Mat4f frust() {
			// 	View& v = camera.view();
			// 	Lens& tl = camera.lens();
			//                 return XMat::frustum2( v.l * tl.mNear, v.r*tl.mNear, v.b*tl.mNear, v.t*tl.mNear, tl.mNear, tl.mFar ); 
			//   	         }
            
			Mat4f norm(){
                return (!(mvm().transpose()) );
            }
        
        //ADVANCED PIPELINE -> Update Shader Uniforms
        void updateMatrices(){
        
            Mat4f tmod = mod();
            Mat4f tview = XMat::lookAt( cam.x(), cam.y(), cam.z() * -1, cam.pos());
            Mat4f tmvm = mvm();
            Mat4f tproj = proj();
            Mat4f tnorm = norm();
            
            copy(tmod.val(), tmod.val() + 16, xf.model);
            copy(tview.val(), tview.val() + 16, xf.view);
            copy(tmvm.val(), tmvm.val() + 16, xf.modelView);
            copy(tproj.val(), tproj.val() + 16, xf.proj);
            copy(tnorm.val(), tnorm.val() + 16, xf.normal);
         
            xf.toDoubles();
        }   

		void onFrame(){
			updateMatrices();     		
		}                               	

	};
	
}     

#endif