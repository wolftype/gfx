/*
 *
 *       Filename:  gfx_scene.h
 *
 *    Description:  Matrix Transforms and Camera Object etc
 *
 *        Version:  1.0
 *        Created:  06/09/2014 17:50:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  AlloSphere
 *
 * =====================================================================================*/

#ifndef GFX_SCENE_INCLUDED
#define GFX_SCENE_INCLUDED   

#include "gfx_gl.h" 
#include "gfx_xfmatrix.h" 

namespace gfx{   
  
  /*-----------------------------------------------------------------------------
   *  Focal Length, Width, Near and Far Clipping Planes, ortho mode boolean ...
   *-----------------------------------------------------------------------------*/
  struct Lens {

      /* Lens Parameters */
      bool bOrtho;
      float mFocal, mNear, mFar, mWidth, mHeight; 

      Lens() :
      mFocal(60.0),                                         ///< in degrees
      mWidth(100),                                          ///< in pixels
      mHeight(100), 
      mNear(0.1), 
      mFar (100.0),
      bOrtho(0)
      {}

      /* Implicit Getters and Setters */
      void   width ( float w )  { mWidth = w;}              ///< set width
      void   height( float h )  { mHeight = h;}             ///< set height
      
      float   width()    const {return mWidth;}             ///< get width
      float   height()  const {return mHeight;}             ///< get height
  
      float&   width()     {return mWidth;}                 ///< width
      float&   height()  {return mHeight;}                  ///< height

      float   ratio()   const { return width()/ height(); }
      float   depth()     const { return mFar - mNear; }    

      float near() const { return mNear; }
      float far() const { return mFar; }
      float& near()  { return mNear; }
      float& far()  { return mFar; }

      void  focal(float f)  { mFocal = f; }                 ///< set focal length
      float& focal()      { return mFocal; }                ///< get focal length
      float  focal() const  { return mFocal; }

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
    
  
  /*-----------------------------------------------------------------------------
   *  Position and Orientation (vec3 and quat)
   *-----------------------------------------------------------------------------*/
  struct Pose {
    Pose(Vec3f p, Quat q = Quat(1,0,0,0)) : mPos(p), mQuat(q) {}// orient(); }
    Pose(float x, float y, float z) : mPos(x,y,z), mQuat(1,0,0,0) {}// orient(); }
    Pose() : mPos(0,0,0), mQuat(1,0,0,0) {}
     
    Vec3f mPos; //mX, mY, mZ, 
    Quat mQuat;
    
    Quat quat() const { return mQuat; }
    Quat & quat() { return mQuat; } 
    Quat rot() const { return mQuat; }
    Quat &rot() { return mQuat; }
    
    Pose& quat( Quat q ) { mQuat = q; return *this; }
    Pose& rot( Quat q ) { mQuat = q; return *this; }         

        
    Vec3f px()  const{ return mPos + x(); }
    Vec3f py()  const{ return mPos + y(); }
    Vec3f pz()  const{ return mPos + z(); }
    
     
    Vec3f x()  const{ return Quat::spin( Vec3f(1,0,0), mQuat); }
    Vec3f y()  const{ return Quat::spin( Vec3f(0,1,0), mQuat); }
    Vec3f z()  const{ return Quat::spin( Vec3f(0,0,1), mQuat); } 
    
    Vec3f pos()  const{ return mPos; }
    Vec3f& pos() { return mPos; }  
    Pose& pos(float x, float y, float z) { mPos.set(x,y,z); return *this; }

    ///DEPRECATED                        
    /* template<class T> */
    /* Pose& set( const T& t){ */

    /*   mPos = Vec3f( t.pos()[0], t.pos()[1], t.pos()[2] ); */ 
    /*   mQuat = Quat ( t.quat()[0], t.quat()[1],  t.quat()[2], t.quat()[3] ); */  

    /*   return *this;//-  t.rot()[3], t.rot()[2], t.rot()[1] );// */
    /* } */   

    //set from your library's vec and quat
    template<class P, class Q>
    Pose& set( const P& t, const Q& q){

      mPos.set( t[0], t[1], t[2] ); 
      mQuat.set( q[0], q[1], q[2], q[3] );  

      return *this;//-  t.rot()[3], t.rot()[2], t.rot()[1] );//
    }   
    
    Pose& reset(float x = 0.0, float y = 0.0, float z = 0.0, Quat q = Quat(1,0,0,0) ){   
      mPos.set(x,y,z); 
      mQuat = q;//Quat(1,0,0,0);
      return *this;
    }
    
    void print(){
      cout << mPos << mQuat << endl;
    }
     
  }; 
  

  
/*-----------------------------------------------------------------------------
 *  A moving pose (acceleration, velocity, rotational velocity)
 *-----------------------------------------------------------------------------*/
  struct MPose : public Pose {
    float aBiv, aVec; 
    Vec3f dVec, dBiv;  
    
    float& ab() { return aBiv; }
    float ab() const { return aBiv; } 
    float& ax() { return aVec; }
    float ax() const { return aVec; }
    Vec3f& db() { return dBiv; }
    Vec3f db() const { return dBiv; } 
    Vec3f& dx() { return dVec; }
    Vec3f dx() const { return dVec; }
    
    MPose(Vec3f p, Quat q = Quat(1,0,0,0)) : Pose(p,q) {}// orient(); }
    MPose(float x, float y, float z) : Pose(x,y,z) {}// orient(); }
    MPose() : Pose() {}  
    
    void move(){     
      mPos += dVec;
      dVec *= aVec; 
    }  
    
    void spin(){     
      mQuat = Quat( dBiv.len(), dBiv.unit() ) * mQuat;
      dBiv *= aBiv; 
    }  
    
    void step(){
      move(); spin();
    }
     
  };

    
/*-----------------------------------------------------------------------------
 * VIEW: Floating points relative to (0,0) at center Left, Top, Right, Bottom, and methods
 *-----------------------------------------------------------------------------*/
  struct View {   

    //Default Clipspace
    View(double _l=0, double _t=1, double _r=1, double _b=0) : l(_l), t(_t), r(_r), b(_b) {}

    //Views for multiscreen environments will typically have same eye 
    //CONSTRUCT (called on resize)
    View( Vec3f eye, const Pose& p, float aspect, float height){
      
      Vec3f bl = p.pos();
      Vec3f br = p.pos() + p.x() * height * aspect; 
      Vec3f tl = p.pos() + p.y() * height ;
      Vec3f tr = tl + p.x() * height * aspect;

      bottomleft = bl; 
      bottomright = br;
      topleft = tl;
      topright = tr;
      
      set( eye, bl, br, tl, p.z() );  

    }

    View ( Vec3f eye, Vec3f bl, Vec3f br, Vec3f tl, Vec3f normal  = Vec3f(0,0,1) ) {
      set( eye,bl,br,tl,normal );
    }  

    //SET FUNC (arguments are in world space coordinates)
    View& set ( Vec3f eye, Vec3f bl, Vec3f br, Vec3f tl, Vec3f normal = Vec3f(0,0,1) ) {

      Vec3f ta = bl - eye; Vec3f tb = (br - eye); Vec3f tc = (tl -eye);
      float d =  -(normal.dot(ta));

      Vec3f x = (tb - ta).unit();
      Vec3f u = (tc - ta).unit();

      l = x.dot(ta) / d;  //*n/d;
      r = x.dot(tb) / d;  //*n/d;
      b = u.dot(ta) / d;
      t = u.dot(tc) / d;

      return *this;
    }      

    float width(){ return r-l; }
    float height(){ return t-b; }
    float l, t, r, b;
    Vec3f topleft, topright, bottomleft, bottomright;

    void print(){
      printf("%f\t%f\t%f\t%f\n", l, t, r, b);
    }

  };

  
/*-----------------------------------------------------------------------------
 *  CAMERA: A moving pose with a view and a lens
 *-----------------------------------------------------------------------------*/
  struct Camera : public MPose {     
    
    Lens lens;
    View view;  
    
    bool bUseFrust;
    
    Camera(float x, float y, float z) : MPose(x,y,z), bUseFrust(true){}
    Camera(const Vec3f& v, const Quat& q = Quat(1,0,0,0)) : MPose(v,q), bUseFrust(true){} 
    
    Vec3f eye(){ return mPos; }
    Vec3f up() { return y(); }
    Vec3f right() { return x(); }  
    Vec3f forward() { return -z(); }
    
    Mat4f fovy() {
      return XMat::fovy( lens.mFocal * PI/180.0, lens.mWidth/lens.mHeight, lens.mNear, lens.mFar );
    }  
    
    Mat4f frust(){
       return XMat::frustum2( 
        view.l * lens.mNear, 
        view.r * lens.mNear, 
        view.b * lens.mNear, 
        view.t * lens.mNear, 
        lens.mNear, lens.mFar ); 
    } 
    
    Mat4f proj(){
      return bUseFrust ? frust() : fovy();
    }


  }; 
  

/*-----------------------------------------------------------------------------
 *  SCENE: Camera, Model pose, matrix containers, whether to use immediate mode
 *-----------------------------------------------------------------------------*/
  struct Scene {

     Camera camera;
     MPose model;
     XformMat xf;

     Pose viewpose;  
      
     Scene() : camera(0,0,5) {} 
      
     void fit(int w, int h){
       camera.lens.width( w ); 
       camera.lens.height( h );   
     } 
    
    void resize(int _w, int _h){

      camera.lens.width( _w );
      camera.lens.height( _h ); 

      float w = (float)_w;///100; //< dividing pixels by 100 here?
      float h = (float)_h;///100; //< dividing pixels by 100 here? 

      Pose pose(-w/2.0,-h/2.0, 0); //<-- pose based on bottom left of screen
      //<--------------------------eye | pose | aspect | height 
      camera.view = gfx::View( camera.pos(), pose, (float)w/h, h);
    }

      Quat cat() { return camera.quat() * model.quat(); } 

      Mat4f mod() { return XMat::rot( model.quat() ); }

      Mat4f mvm() {
        return XMat::lookAt( camera.x(), camera.y(), camera.z(), camera.pos() ) * mod(); 
      }
            
      Mat4f norm(){
        return ( !(mvm().transpose() ) );
      }

      //into clip space
      Vec3f project(const Vec3f& v){

         Vec4f tp = mvm() * Vec4f(v[0],v[1],v[2],1.0);
         Vec4f vp = camera.proj() * tp;

         vp[0] /= vp[3];
         vp[1] /= vp[3];
         vp[2] /= vp[3];
                     
        Vec3f sc(
            (vp[0]+1)/2.0,//camera.view.l + camera.view.width() * 
            (vp[1]+1)/2.0,//camera.view.b + camera.view.height() * 
            0 //(vp[2]+1)/2.0 
        ); 

        return sc;
               
      }

      //cast view coords into 3D world coordinates (not yet tested)
      Vec3f unproject(const Vec3f& v){

        Mat4f ipm = !(camera.proj()*mvm());
        Vec4f sc ( 
                      (2*(v[0]))-1,//-camera.view.l))/camera.view.width() -1, 
                      (2*(v[1]))-1,//-camera.view.b))/camera.view.height() -1,
                      (2*v[2]) -1,
                      1
                    ); 

        Vec4f vp = ipm * sc;

         vp[0] /= vp[3];
         vp[1] /= vp[3];
         vp[2] /= vp[3];

        return Vec3f(vp[0],vp[1],vp[2]);
      }

    void push(bool bImmediate){        
      updateMatrices();
      if(bImmediate) pushMatrices();
    }

    void pop(bool bImmediate){
      if(bImmediate) popMatrices();
    }

    void step(){
      model.step(); camera.step();
    }

    /* Mat4f modelviewmult( const Mat4f& m){ */
    /*   return mvm() * m; */
    /* } */

    void updateMatrices(){     
      Mat4f tmvm =  mvm();    
      Mat4f tproj = camera.proj();  
      Mat4f tnorm = norm();

      copy(tmvm.val(), tmvm.val() + 16, xf.modelView);
      copy(tproj.val(), tproj.val() + 16, xf.proj);
      copy(tnorm.val(), tnorm.val() + 16, xf.normal);
     
      xf.toDoubles();
    } 

    void pushMatrices(){
       
     // FIXED FUNCTION PIPELINE  
    #ifdef GFX_IMMEDIATE_MODE

        Vec3f look = camera.pos() + camera.forward(); 
      
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glLoadIdentity();

        //! replaced gluPerspective with glLoadMatrix( xf.proj)
        if ( camera.lens.bOrtho ){
            float oz =  camera.pos()[2];
            float aspect = camera.lens.mWidth/camera.lens.mHeight;
            glOrtho(-aspect* oz, aspect* oz,-1.0 * oz, 1.0 * oz, -50.0, 50.0);
        } else {
            glLoadMatrixf( xf.proj );
        }
      
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        gluLookAt( 
        camera.pos().x, camera.pos().y, camera.pos().z, 
        look.x, look.y, look.z, 
        camera.up().x, camera.up().y, camera.up().z );   
      
        Vec4<> tr = model.quat().axan(); 
      
        glRotatef ( tr[3], tr[0], tr[1], tr[2]  );
    #endif
    }

    void popMatrices(){
    #ifdef GFX_IMMEDIATE_MODE

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    
    #endif
     }       

  };
      

}     

#endif
