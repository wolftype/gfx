/// @file Vertex Mesh Data structures (geometry, normals, colors, uv textures)
//    
//
//
//  GFX
//
//  Mesh.h
//
//  Created by Pablo Colapinto on 4/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef GFX_Mesh_h
#define GFX_Mesh_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>  

#include "gfx_matrix.h"  
#include "gfx_gl.h"

using namespace std;  
 

namespace gfx {

  /// Attributes Classification
  template<class T> // attribute types
  struct GLVertexData {
    struct Att { string type; string name; };
    static map<string, GLvoid*> Attribute; // map of attribute names and offsets into memory

    static GLVertexData& Init(){
      static GLVertexData TheGLVertexData;
      return TheGLVertexData;
    }
    private:
      GLVertexData(){ Make(); }
      static void Make();
  };
  template<class T>
  map<string,GLvoid*> GLVertexData<T>::Attribute;

  struct VertexPosition {
     Vec3f Pos;
     Vec3f Norm;

     VertexPosition( const Vec3f& pos, const Vec3f& norm=Vec3f(0,0,1)) : Pos(pos), Norm(norm) {}
     VertexPosition( float x, float y, float z ) : Pos(x,y,z), Norm(0,0,1) {}
 
     static GLvoid * on() { return (GLvoid*)sizeof(Vec3f); }

     float operator[] (int idx) { return Pos[idx]; }
  };

  struct VertexColor {
     Vec4f Col;
     Vec2f Tex;

     VertexColor( const Vec4f& col = Vec4f(1,1,1,1), const Vec2f& tex=Vec2f(0,0)) : Col(col), Tex(tex) {}
     VertexColor( float r, float g, float b, float a=1.0 ) : Col(r,g,b,a), Tex(0,0) {}
 
     static GLvoid * ot() { return (GLvoid*)sizeof(Vec4f); }

     //float operator[] (int idx) { return Col[idx]; }
     float * begin() { return &Col[0]; }

  };

   
  /*-----------------------------------------------------------------------------
   *  Position and texture
   *-----------------------------------------------------------------------------*/
  struct VertexTexture {
      Vec3f Pos;
      Vec2f Tex;

      VertexTexture( const Vec3f& p, const Vec2f& t) : Pos(p), Tex(t) {}
      VertexTexture( float x, float y, float z ) : Pos(x,y,z), Tex(0,0) {}

      static GLvoid * ot() { return (GLvoid*)sizeof(Vec3f); }
      float * begin() { return &Pos[0]; }
  
  };
     template<> void GLVertexData<VertexTexture>::Make(){
        Attribute["position"]=0;
        Attribute["texCoord"]=VertexTexture::ot();
     }

  /*-----------------------------------------------------------------------------
   *  Position and 3D texture
   *-----------------------------------------------------------------------------*/
  struct VertexTexture3D {
      Vec3f Pos;
      Vec3f Tex;

      VertexTexture3D( const Vec3f& p, const Vec3f& t) : Pos(p), Tex(t) {}
      VertexTexture3D( float x, float y, float z ) : Pos(x,y,z), Tex(0,0,0) {}

      static GLvoid * on() { return 0; }
      static GLvoid * oc() { return 0; }
      static GLvoid * ot() { return (GLvoid*)sizeof(Vec3f); }
 //     float operator[] (int idx) { return Pos[idx]; } 
      float * begin() { return &Pos[0]; }
  
  };

  /*!
   *  VERTEX DATA Interleaved With 3D textures
   */
    struct VertexTex3D {
        Vec3<float> Pos;        ///< 3d Position
        Vec3<float> Norm;       ///< 3d normal
        Vec4<float> Col;        ///< RGBA Color (could be uchar)
        Vec3<float> Tex;        ///< UV Coordinates

                
        VertexTex3D(const Vec3f& pos = Vec3f(0,0,0), 
               const Vec3f& norm = Vec3f(0,0,1), 
               const Vec4f& col = Vec4f(1,1,1,1), 
               const Vec3f& tex = Vec3f(0,0,0) )  
        
        : Pos(pos), Norm(norm),  Col(col), Tex(tex) 
        {}
        
        VertexTex3D(float x, float y, float z) : Pos(x,y,z), Norm(0,0,1), Col(1,1,1,1), Tex(0.0,0.0,0.0) {}
        
        float * pos() { return &Pos[0]; }
        float * norm() { return &Norm[0]; } 
        float * col() { return &Col[0]; }   
        float * tex() { return &Tex[0]; }
        
        static GLvoid * on() { return (GLvoid*)sizeof(Vec3f); }
        static GLvoid * oc() { return (GLvoid*)( 2 * sizeof(Vec3f) ) ; }
        static GLvoid * ot() { return (GLvoid*)( 2 * sizeof(Vec3f) + sizeof(Vec4f) ); }

        static size_t offsetColor(){ return 2*sizeof(Vec3f); }        
        static size_t offsetNormal(){ return sizeof(Vec3f); }        
        static size_t offsetTextureCoordinate(){ return 2*sizeof(Vec3f) + sizeof(Vec4f); }        

        float * begin() { return &Pos[0]; }
       // float operator[] (int idx) const { return ((float*)(&Pos[0]))[idx]; }
       // float& operator[] (int idx) { return ((float*)(&Pos[0]))[idx]; }//((float*)&(Pos[0]))[idx]; }

        void print() {
          cout << Pos << Norm << Col << Tex << endl;  
        }
     };
     
    
  /*!
   *  VERTEX DATA Interleaved
   */
    struct Vertex {
        Vec3<float> Pos;        ///< 3d Position
        Vec3<float> Norm;       ///< 3d normal
        Vec4<float> Col;        ///< RGBA Color (could be uchar)
        Vec2<float> Tex;        ///< UV Coordinates
                
        Vertex(const Vec3f& pos = Vec3f(0,0,0), 
               const Vec3f& norm = Vec3f(0,0,1), 
               const Vec4f& col = Vec4f(1,1,1,1), 
               const Vec2f& tex = Vec2f(0,0) )  
        
        : Pos(pos), Norm(norm),  Col(col), Tex(tex) 
        
        {}
        
        Vertex(float x, float y, float z) : Pos(x,y,z), Norm(0,0,1), Col(1,1,1,1), Tex(0.0,0.0) {}
        
        float * pos() { return &Pos[0]; }
        float * norm() { return &Norm[0]; } 
        float * col() { return &Col[0]; }   
        float * tex() { return &Tex[0]; }
        
        static GLvoid * on() { return (GLvoid*)sizeof(Vec3f); }
        static GLvoid * oc() { return (GLvoid*)( 2 * sizeof(Vec3f) ) ; }
        static GLvoid * ot() { return (GLvoid*)( 2 * sizeof(Vec3f) + sizeof(Vec4f) ); }

        static size_t offsetColor(){ return 2*sizeof(Vec3f); }        
        static size_t offsetNormal(){ return sizeof(Vec3f); }        
        static size_t offsetTextureCoordinate(){ return 2*sizeof(Vec3f) + sizeof(Vec4f); }        

        float * begin() { return &Pos[0]; }
       // float operator[] (int idx) const { return ((float*)(&Pos[0]))[idx]; }
       // float& operator[] (int idx) { return ((float*)(&Pos[0]))[idx]; }//((float*)&(Pos[0]))[idx]; }

        void print() {
          cout << Pos << Norm << Col << Tex << endl;  
        }
     };

     template<> void GLVertexData<Vertex>::Make(){
        Attribute["position"]=0;
        Attribute["normal"]=Vertex::on();
        Attribute["sourceColor"]=Vertex::oc();
        Attribute["texCoord"]=Vertex::ot();
     }


  /*!
   *  Triangle of three Vertices
   */
  struct Triangle {
      Vertex a,b,c; //counterclockwise
   };
   

  /*!
   *  Mesh Data Container Templated on Data Type
   */
  template<class T>
  class MeshData {

    protected:

    GL::MODE mMode;         ///< Draw Mode
    vector<T> mVertex;      ///< Vertex Data
    vector<T> mStore;       ///< Original Stored Copy (for transforming by absolute coordinates)


  //  typedef unsigned short INDEXTYPE;   ///< Type of Index Data (max number of vertices . . .)
    vector<unsigned short> mIndex;           ///< Indices of Element Array Buffer

    public:
      typedef unsigned short INDEXTYPE;

      /// Default Draw Mode is Line Loop
      MeshData(GL::MODE m = GL::LL) : mMode(m) {}


      ///Copy Constructor
      MeshData(const MeshData& m){
        mMode = m.mMode;
        for (int i = 0; i < m.num(); ++i){
            mVertex.push_back( m[i] ); 
            mStore.push_back( m[i] );
        }
        for (int i = 0; i < m.mIndex.size(); ++i){
            mIndex.push_back(m.mIndex[i]);
        } 
      }  

      ///Assignment Operator
       MeshData operator = (const MeshData& m){
          
          if (this != &m ){
                mMode = m.mMode;   
                  for (int i = 0; i < m.num(); ++i){
                      mVertex.push_back( m[i] ); 
                      mStore.push_back( m[i] );      
                  }
                  for (int i = 0; i < m.mIndex.size(); ++i){
                      mIndex.push_back(m.mIndex[i]);
                  }      
              }     
          return *this;
        }

     /// Store a Copy for absolute transformations
     void store() {
       mStore = mVertex;
     }  

     /// GETTER AND SETTER of VERTEX DATA
     T& operator[] (int idx) { return mVertex[idx]; }
     T operator[] (int idx) const { return mVertex[idx]; }
     
     INDEXTYPE & idx(int ix) { return mIndex[ix]; }
     INDEXTYPE  idx(int ix) const { return mIndex[ix]; }

     T& store(int ix) { return mStore[ix]; }
     T store(int ix) const { return mStore[ix]; }
     
     int num() const { return mVertex.size(); }
     int numIdx() const { return mIndex.size(); }
    
     vector<INDEXTYPE>::iterator indices() { return mIndex.begin(); }        
     typename vector<T>::iterator vertices() { return mVertex.begin(); }

     vector<T>& vertex() { return mVertex; }
     vector<T>& original() { return mStore; }
     vector<INDEXTYPE>& index() { return mIndex; }
      
     T& last() { return mVertex[ mVertex.size() - 1 ]; }
      
     void clear() {
          mVertex.clear();
          mIndex.clear();
     }

     /// Set Draw Mode
     MeshData& mode( GL::MODE m) { mMode = m; return *this; }  
     /// Get Draw Mode
     GL::MODE mode() const { return mMode; }

     /// Add Indices List
     MeshData& add(int idx) {
         mIndex.push_back(idx); return *this;
     }
     /// Add N Indices
     MeshData& add(int * idx, int n) {
         for (int i = 0; i < n; ++i) { mIndex.push_back( idx[i] ); } 
         return *this;
     }
     /// Add Last Index 
     MeshData& add(){ add( num() - 1 ); return *this; }
        
     //Add additional vertices from another Mesh
     MeshData& add(const MeshData& m){
         for (int i = 0; i < m.num(); ++i) { add(m[i]); mStore.push_back( m.store(i) ); }
         for (int i = 0; i < m.numIdx(); ++i) { add( m.idx(i) ); }
         return *this;
     }

     /// Add a vertex
     MeshData& add(const T& v) { mVertex.push_back(v); return *this;}   
     MeshData& add(const Vec3f& v) { mVertex.push_back( T(v) ); return *this; }
     MeshData& add(const Vec3f& v, const Vec3f& n) { mVertex.push_back( T(v,n) ); return *this; }
     MeshData& add(float x, float y, float z) { mVertex.push_back( T( Vec3f(x,y,z) ) ); return *this; }

     /// ADD N VERTICES
     template<typename S>
     MeshData& add(typename vector<S>::iterator v, int n){
         for (int i = 0; i < n; ++i) { mVertex.push_back( Vertex( Vec3f( v[i][0], v[i][1], v[i][2] ) ) ); }
     }
     
     MeshData& add(Vec3f * v, int n) { 
         for (int i = 0; i < n; ++i) { mVertex.push_back( Vertex(v[i]) ); } 
         return *this;
     }


     /*-----------------------------------------------------------------------------
      *  VIRTUAL (can be overloaded)
      *-----------------------------------------------------------------------------*/
      virtual MeshData& color(float r, float g, float b, float a = 1.0) { 
         // mColor.set(r,g,b,a);   
        
          /* for (int i = 0; i < mVertex.size(); ++i ){ */
          /*   mVertex[i].Col = Vec4f(r,g,b,a);//mColor; */
          /* } */   
        
          return *this;
        }
      

        virtual void drawElements() const {
              /* GL::Begin( mMode); */
              /* for (int i = 0; i < mIndex.size(); ++i){ */  
              /*     GL::normal( mVertex[ mIndex[i] ].Norm ); */
              /*     GL::vertex( mVertex[ mIndex[i] ].Pos ); */
              /* } */
              /* glEnd(); */
         }

  };

namespace mesh{

  template<class T>
  MeshData<T> grid2d(int w, int h, float spacing){
     MeshData<T> m; 
     int offsetU = -(w*spacing)/2.0;
     int offsetV = -(h*spacing)/2.0;
      for (int u=0; u<w; ++u){
          for (int v=0; v<h; ++v){
            T nv( offsetU + u * spacing, offsetV + v*spacing, 0 );
            nv.Tex[0] = (float)u/w;
            nv.Tex[1] = (float)v/h;
            m.add(nv).add();
        }
      }
      m.mode(GL::P);
      m.store();
      return m;
  }

  template<class T, class Func>
  MeshData<T> gridtex(int w, int h, float spacing, Func f){
     MeshData<T> m; 
     int offsetU = -(w*spacing)/2.0;
     int offsetV = -(h*spacing)/2.0;
      for (int u=0; u<w; ++u){
          for (int v=0; v<h; ++v){
            T nv( offsetU + u * spacing, offsetV + v*spacing, 0 );
            nv.Tex = f((float)u/w,(float)v/h);
            m.add(nv).add();
        }
      }
      m.mode(GL::P);
      m.store();
      return m;
  }

  template<class T, class TexFunc>
  MeshData<T> uvtex(int w, int h, float spacing, TexFunc f){
     vector<T> tv;
     int offsetU = -(w*spacing)/2.0;
     int offsetV = -(h*spacing)/2.0;
      for (int u=0; u<w; ++u){
          for (int v=0; v<h; ++v){
            T nv( offsetU + u * spacing, offsetV + v*spacing, 0 );
            nv.Tex = f((float)u/w,(float)v/h);
            tv.push_back(nv);
        }
      }

      MeshData<T> m; 
      bool bFlip = false;
      for (int i = 0; i < w-1; ++i){
        for (int j = 0; j < h; ++j){
         
          int a = bFlip ? (i+1) * (h) - j - 1: i * h + j;
          int b = a + h;

          T va = tv[a];
          T vb = tv[b];

          m.add( va ).add();
          m.add( vb ).add(); 
        }
        bFlip = !bFlip; //FLIP EACH COLUMN
      }

      m.mode(GL::TS);
      m.store();
      return m;
  }

  //absolute
  MeshData<Vertex>& move( MeshData<Vertex>& m, float x, float y, float z){
      for (int i = 0; i < m.vertex().size(); ++i ){
          m[i].Pos = m.store(i).Pos + Vec3f(x,y,z);
      }
      return m;   
  }

  MeshData<Vertex>& color( MeshData<Vertex>& m, float r, float g, float b, float a){
      for (int i=0; i<m.vertex().size(); ++i){
        m[i].Col = Vec4f(r,g,b,a);
      } 
      return m;
  }


        #ifdef GFX_IMMEDIATE_MODE
        
          //immediate mode!
          /* void drawVertices() const {//(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) { */
          /*     //glColor4f(r,g,b,a); */
          /*     GL::Begin( mMode); */
          /*     for (int i = 0; i < mVertex.size(); ++i){ */
          /*         GL::vertex( mVertex[i].Pos ); */
          /*     } */
          /*     glEnd(); */
          /* } */

          /* void drawElementsColor() const { */
          /*     GL::Begin( mMode); */
          /*     for (int i = 0; i < mIndex.size(); ++i){ */  
          /*         GL::color( mVertex[ mIndex[i] ].Col ); */
          /*         GL::normal( mVertex[ mIndex[i] ].Norm ); */
          /*         GL::vertex( mVertex[ mIndex[i] ].Pos ); */
          /*     } */
          /*     glEnd(); */
          /* } */

           void drawElements(const MeshData<Vertex>& m) {
              GL::Begin( m.mode());
              for (int i = 0; i < m.numIdx(); ++i){  
                  GL::normal( m[ m.idx(i) ].Norm );
                  GL::vertex( m[ m.idx(i) ].Pos );
              }
              glEnd();
          }
        
        #endif


} // mesh::

     
     //how to partially specialize from a base
     //MeshData<Vertex> : MeshData<> etc
  
  
   

  /*!
   *  MESH DATA CONTAINER FOR VERTEX AND INDEX INFORMATION
   */
    class Mesh : public MeshData<Vertex> { 
        
                
        /// Base Color
        Vec4f mColor;
          
    public:
        typedef unsigned short INDEXTYPE;
          
        /// Default Draw Mode is Line Loop
        Mesh(GL::MODE m = GL::LL) : MeshData(m) {}
 
       // template<class T>
        Mesh(const MeshData<Vertex>& m ) : MeshData<Vertex>(m) {}

        ///Move To absolute x,y,z
        Mesh& moveTo( double x, double y, double z ){
          
          for (int i = 0; i < mVertex.size(); ++i ){
            mVertex[i].Pos = mStore[i].Pos + Vec3f(x,y,z);
          }   
          
          return *this;
        }  

        ///Move to absolute Vec3f v
        Mesh& moveTo( const Vec3f& v ){
          
          for (int i = 0; i < mVertex.size(); ++i ){
            mVertex[i].Pos = mStore[i].Pos + v;
          }   
          
          return *this;
        }    
                
        /// Create Mesh from an OBJ file
        Mesh( string s) { load(s); }
                    
        Mesh& color(float r, float g, float b, float a = 1.0) { 
          mColor.set(r,g,b,a);   
        
          for (int i = 0; i < mVertex.size(); ++i ){
            mVertex[i].Col = mColor;
          }   
        
          return *this;
        }
                  
        
        //GL_IMMEDIATE_MODE DEFAULT = 1
        #ifdef GFX_IMMEDIATE_MODE
        
          //immediate mode!
          void drawVertices() const {//(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) {
              //glColor4f(r,g,b,a);
              GL::Begin( mMode);
              for (int i = 0; i < mVertex.size(); ++i){
                  GL::vertex( mVertex[i].Pos );
              }
              glEnd();
          }

          void drawElementsColor() const {
              GL::Begin( mMode);
              for (int i = 0; i < mIndex.size(); ++i){  
                  GL::color( mVertex[ mIndex[i] ].Col );
                  GL::normal( mVertex[ mIndex[i] ].Norm );
                  GL::vertex( mVertex[ mIndex[i] ].Pos );
              }
              glEnd();
          }

           void drawElements() const {//float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0) const {
            //  glColor4f(1,1,1,1);  
              GL::Begin( mMode);
              for (int i = 0; i < mIndex.size(); ++i){  
                  GL::normal( mVertex[ mIndex[i] ].Norm );
                  GL::vertex( mVertex[ mIndex[i] ].Pos );
              }
              glEnd();
          }
        
        #endif


                
        Mesh& translate(float x, float y, float z){
            
            for (int i = 0; i < num(); ++i){
                mVertex[i].Pos[0] += x;
                mVertex[i].Pos[1] += y;
                mVertex[i].Pos[2] += z;
            }    
            return *this;
        }  

        Mesh& translateA(float x, float y, float z){
            
            for (int i = 0; i < num(); ++i){
                mVertex[i].Pos[0] = mStore[i].Pos[0] + x;
                mVertex[i].Pos[1] = mStore[i].Pos[1] + y;
                mVertex[i].Pos[2] = mStore[i].Pos[2] + z;
            }    
            return *this;
        }
        
        Mesh& scale(float s){
            
            for (int i = 0; i < num(); ++i){
               mVertex[i].Pos *= s;
            } 
             
          return *this;
        }


        Mesh& scaleA(float s){
            
          for (int i = 0; i < num(); ++i){
              mStore[i].Pos *= s;
            }  
          
          return *this;
        }  

        Mesh& rotate( const Quat q ){
           for (int i = 0; i < num(); ++i){
            mVertex[i].Pos = Quat::spin( mVertex[i].Pos, q);
                }  
              return *this; 
        } 
        
        Mesh& rotateA( const Quat q ){
           for (int i = 0; i < num(); ++i){
            mVertex[i].Pos = Quat::spin( mStore[i].Pos, q);
                }  
              return *this; 
        }
        
        //LOAD FROM OBJ FILE
        void load(string filepath) {
            
            string line;
            //string fname = s;

            std::fstream myfile;
       
            //search for file by going up file directory tree up to 5 times
            int attempts = 0;
            string nfilepath = filepath;
            while (!myfile.is_open() && attempts < 5) {
                myfile.open( nfilepath.c_str(), std::ios::in );
                nfilepath = "../" + nfilepath;   
                attempts += 1;
            }
            if (!myfile.is_open()) throw std::invalid_argument("Error: File Not Found.");  

         //   ifstream myfile(fname.c_str());
            
         //   cout << "loading " << s << " from " << fname << " . . . " <<  endl;
            
            if (myfile.is_open()){
                printf("file open . . .\n");
                while (!myfile.eof()){
                    
                    getline(myfile, line);
                    int in = line.find_first_of(" ");
                    string a = line.substr(0,in);
                    
                    if (a=="v"){
                        
                        stringstream s; s <<  string(line.substr(2));
                        Vec3<float> v;
                        s >> v[0];  
                        s >> v[1]; 
                        s >> v[2];
                        add(v);
                        
                    }
                    
                    if (a=="f"){
                        
                        
                    }
                    
                }
                myfile.close();            
            }
            
            printf("# of vertices: %d\n", (int)mVertex.size() );  
            store();
        }    
    
        template<class T>
        static Mesh UV( T* p, int w, int h, float tw =1, float th =1);

        static Mesh Point(float x, float y, float z);

        template<class T>
        static Mesh Point(const T& p);

        template<class T>
        static Mesh Points(T* p, int num);  
        
        static Mesh Points(int num);      
        
        template<class T>
        static Mesh Points(std::vector<T> p);
        
        //3d vecs in 3d space . . .
        template<class T, class S>
        static Mesh Points2(T* p, S*q, int num); 
        
        //2d vecs in 3d space . . .
        template<class T, class S>
        static Mesh Points2D(T* p, S*q, int num);  

        static Mesh Grid(int w = 10, int h = 10, float spacing = .2);

        static Mesh Sphere(double rad = 1.0, int slices = 20, int stacks = 20);

  //    static Mesh Line( Vec3f a, Vec3f b);

        template<class T>
        static Mesh Line( T a, T b);

    // template<class T>
    // static Mesh Lines( T * a, int num);

        static Mesh Cone( double rad = 1.0 , double h = 1.0, int slices = 10, int stacks = 4);
        static Mesh Dir();
        static Mesh Circle(double radius = 1, double res=50);//double res = 10);
        static Mesh Disc(double scale = 1);        
        static Mesh Rect(float w, float h);
        static Mesh IRect(float w, float h);  
        static Mesh Cylinder(float r = 1.0, float h = 2.0, int slices = 20, int stacks = 1);
        
        static Mesh Frame(float size = 1.0);
        
        static Mesh Contour(int res);
        static Mesh Contours(int num, int res);   
        static Mesh Grid2D(int, int, float);
        
        static Mesh Num(int n);

        static Mesh HexaGrid(int w, int h, float spacing);
    
    };              

  inline Mesh Mesh::Num(int n){ 
    Mesh m;  
    m.mode(GL::LS); 
    switch(n){
      case 0:
        m.add(-1,-1,0).add();
        m.add(-1,1,0).add();
        m.add(1,1,0).add();
        m.add(1,-1,0).add();
        m.mode(GL::LL);
        break;
      case 1:
        m.add(0,-1,0).add();
        m.add(0,1,0).add();

        break;
      case 2:
        m.add(-1,1,0).add();
        m.add(1,1,0).add();
        m.add(-1,-1,0).add();
        m.add(1,-1,0).add();
        break; 
      case 3:
        m.add(-1,1,0).add();
        m.add(1,1,0).add(); 
        m.add(0,0,0).add();
        m.add(1,-1,0).add(); 
        m.add(-1,-1,0).add();    
        break;
    } 
    
    m.store();  
    return m;
  }
    template<class T>
    inline Mesh Mesh::UV( T* p, int w, int h, float tw, float th){
      Mesh m;

      //MESH ORDER FOR TRIANGLE STRIP
      bool bFlip = false;
      for (int i = 0; i < w-1; ++i){
        for (int j = 0; j < h; ++j){
          int a = bFlip ? (i+1) * (h) - j - 1: i * h + j;
          int b = a + h;

          Vertex va(p[a][0], p[a][1], p[a][2]);
          va.Tex = Vec2f(( p[a][0] + tw/2.0) / tw, (p[a][1] +th/2.0) / th );
          va.Col = Vec4f(( p[a][0] + tw/2.0) / tw, (p[a][1] +th/2.0) / th, 1- ( p[a][0] + tw/2.0) /tw, 1 );
          Vertex vb(p[b][0], p[b][1], p[b][2]);
          va.Tex = Vec2f(( p[b][0] + tw/2.0) / tw, (p[b][1] +th/2.0) / th );
          va.Col = Vec4f(( p[b][0] + tw/2.0) / tw, (p[b][1] +th/2.0) / th, 1- ( p[b][0] + tw/2.0) /tw, 1 );

          m.add( va ).add();
          m.add( vb ).add(); 
        }
        bFlip = !bFlip; //FLIP EACH COLUMN
        //m.add().add().add();//.add(i*h);//.add(i*h);
      }

    //NORMALS
    bFlip = true; int ct = 0;
     for (int i = 0; i < m.num(); i+=3){
        int a = i; int b = i + 1; int c = i + 2;
        Vec3f v = (m[b].Pos - m[a].Pos).cross( m[c].Pos - m[a].Pos).unit();
        m[a].Norm = bFlip ? v : -v;
        m[b].Norm =  bFlip ? v : -v;
        m[c].Norm =  bFlip ? v : -v;
         bFlip = !bFlip;
        if ( ct == (h-1) ) {
          //bFlip = !bFlip;
         // m[a].Col.set(.2,0,0,1);
          ct = 0;
        }
        ct++;
      }
      
      m.mode(GL::TS);
      m.store();
      return m;
    }
    

   inline Mesh Mesh::HexaGrid(int w, int h, float spacing){

   // Mesh m;

    float tw = spacing * w;
    float th = spacing * h;

    Vec3f * vec = new Vec3f[w*h];

    bool bFlip =0;
    for (int i=0; i<w; ++i){
      for (int j=0; j<h; ++j){
       float u = -tw/2.0 + (float)i/w * tw + (bFlip?spacing/2.0:0);
       float v = -th/2.0 + (float)j/h * th;
       vec[i*h+j] = Vec3f(u,v,0);
       bFlip = !bFlip;
      }  
    }

    Mesh m = Mesh::UV(vec, w, h,tw+spacing/2.0,th);
    delete[] vec;
    return m;
  }

    
    inline Mesh Mesh::Point(float x, float y, float z){
        Mesh m;
        m.add(x,y,z).add();
        m.mode( GL::P );
    m.store();
        return m;
    }
  template<class T>
    inline Mesh Mesh::Point(const T& p){
        Mesh m;
        m.add(p[0],p[1],p[2]).add();
        m.mode( GL::P );
    m.store();
        return m;
    }
  
  template<class T>
  inline Mesh Mesh::Points(T * p, int num){
    Mesh m;
    for (int i = 0; i < num; ++i){
      m.add(p[i][0],p[i][1],p[i][2]).add();
    }
    m.mode( GL::P );    
    m.store();    
    return m;
  } 

  inline Mesh Mesh::Points(int num){
    cout << "NUM POINTS: " << num << endl; 
    Mesh m;
    for (int i = 0; i < num; ++i){
      float u = (float)i/num * 10;
      m.add(u,5,0).add();
    }
    m.mode( GL::P );    
    m.store();    
    return m;
  } 

  
  inline Mesh Mesh::Contour(int num){
    Mesh m;
    for (int i = 0; i < num; ++i){
      m.add(0,0,0).add();
    }
    m.mode( GL::LS );    
    m.store();    
    return m;
  } 
  
  inline Mesh Mesh::Contours(int num, int res){
    Mesh m;
    for (int i = 0; i < num; ++i){
        
      for (int j = 0; j < res-2; ++j){
          m.add(0,0,0).add(); 
      }       
    }
    m.mode( GL::LS );    
    m.store();    
    return m;
  }              
  
  template<class T>
  inline Mesh Mesh::Points(std::vector<T> p){
    Mesh m;
    for (int i = 0; i < p.size(); ++i){
      m.add(p[i][0],p[i][1],p[i][2]).add();
    }
    m.mode( GL::P );    
    m.store();    
    return m;
  }
  
  
  template<class T, class S>
  inline Mesh Mesh::Points2(T * p, S * q, int num){
    Mesh m;
    for (int i = 0; i < num; ++i){
      m.add(p[i][0],p[i][1],p[i][2]).add();
      m.add(q[i][0],q[i][1],q[i][2]).add();
    }
    m.mode( GL::P );    
    m.store();    
    return m;
  } 
  
  template<class T, class S>
  inline Mesh Mesh::Points2D(T * p, S * q, int num){
    Mesh m;
    for (int i = 0; i < num; ++i){
      m.add(p[i][0],p[i][1],p[i][2]).add();
      m.add(q[i][0],q[i][1], 0).add();
    }
    m.mode( GL::P );    
    m.store();    
    return m;
  }
  

    inline Mesh Mesh::Grid(int w, int h, float spacing){
        Mesh m;
        float tw = spacing * w;
        float th = spacing * h;
        
    //Vertical Lines
        for (int i = 0; i <= w; ++i){
            float tx = (-1.0 + 2.0 * i/w ) * tw/2.0;
            float ty = th/2.0;
            m.add(tx,-ty,0).add();
            m.add(tx,ty,0).add();
        }
        
    //Horizontal Lines
        for (int i = 0; i <= h; ++i){
            float ty = (-1.0 + 2.0 * i/h ) * th/2.0;
            float tx = tw/2.0;
            m.add(-tx,ty,0).add();
            m.add(tx,ty,0).add();
        }

        m.mode(GL::L);
        m.store();
       return m;
    }

    inline Mesh Mesh::Grid2D(int w, int h, float spacing){
     Mesh m; 
     int offsetU = -(w*spacing)/2.0;
     int offsetV = -(h*spacing)/2.0;
      for (int u=0; u<w; ++u){
          for (int v=0; v<h; ++v){
            Vertex nv( offsetU + u * spacing, offsetV + v*spacing,0 );
            nv.Tex = Vec2f((float)u/w, (float)v/h);
            m.add(nv).add();
        }
      }
      m.mode(GL::P);
      m.store();
      return m;

    }

       
      inline Mesh Mesh::Sphere(double rad, int slices, int stacks){
          Mesh m;
                  
          for (int i = 0; i <= stacks; ++i){
          
              float v = -1.0 + 2.0 * i/stacks;
              
              for (int j = 0; j < slices; ++j){
                  
                  float u = 1.0* j/slices;

                   //angle around y axis
                   Quat qu = Quat( PI*u, Vec3f(0,1,0));
                   //angle around z axis
                   Quat qv = Quat( PIOVERFOUR * v, Vec3f(0,0,1) ) ;
        
                   Vec3f tv = Quat::spin( Vec3f(1,0,0),  qu * qv ) ;
                   m.add( tv * rad, tv.unit() );

                  //only one point at each pole: 
                  if (i == 0 || i == stacks) {
                      break;
                  }
                                  
              }
          }
          
          //BOTTOM 
        //  m.add(0).add(1).add(2);
          for (int j = 1; j <= slices; j+=1){
              m.add(0).add(j).add(j+1).add(0);
          }
         // m.add(0).add(1);
          
          //m.add(1);
          
          for (int i = 0; i < stacks -1; ++i){
              static bool color = 0;
              color = !color;
              int a, b;
              for (int j = 0; j < slices; ++j){
                  static bool xcolor = 0;
                  xcolor = !xcolor;   
                               
                  a = 1 + i * slices + j;
                  //if (a == 0) continue;
                  
                  b =  ( i < stacks - 2) ? a + slices : m.num() - 1;  // Next Higher Latitude or North Pole
                  
                 // int c = ( j < slices - 1 ) ? a + 1 : 1 + i * slices;
                  
                  m.add(a).add(b); 
                
                  m[a].Col.set(color,xcolor,1,1);
              }
              a= 1 + i * slices ;
              b =  ( i < stacks - 2) ? a + slices : m.num() - 1;
              m.add(a).add(b); 

          }
          
          m.last().Col.set(0,1,0,1);
          
          m.mode(GL::TS);
          m.store(); 
         return m;
      }
    
  template<class T>
    inline Mesh Mesh::Line (T a, T b){
    
        Mesh m;
        m.add(a[0], a[1], a[2]).add(b[0], b[1], b[2]);
        m.add(0).add(1);
        m.mode( GL::LS );
        m.store();
        return m;
    }
    
    inline Mesh Mesh::Circle (double radius, double _res){
        
        //int res = floor( _res * 100);
        int res = _res;
        Mesh m;
        
        for (int i = 0; i <= res; ++i){
            float rad = 2.0 * PI * i / res;
            float x = cos(rad) * radius;
            float y = sin(rad) * radius;
            
            m.add( Vec3f(x,y,0) );
            m.add(i);
            
        }
        
        m.mode(GL::LL);
        m.store();
        return m;
    }
    

    inline Mesh Mesh::Cone(double r, double h, int slices, int stacks){
        
        Mesh m;
        
        for (int i = 0; i <= stacks; ++i){
            float z = h * i / stacks;
            for (int j = 0; j < slices; ++j){
                float rad = 2.0 * PI * j / slices;
                float x = cos(rad) * (h-z) * r;
                float y = sin(rad)  * (h-z) * r;
                m.add( Vertex( Vec3f(x,y,z), Vec3f(x,y,z),Vec4f(1,1,1,1), Vec2f(z, 1.0* j/slices) ) );
                m.add(i * slices + j);
            }
            m.add(i*slices);
        }
        int peak = m.index().back();

        for (int j=0; j<slices; ++j){
            m.add(j).add(peak);      
        }
        
        m.mode(GL::LL); 
        m.store(); 
        return m;
    
    }


    inline Mesh Mesh::Dir(){
        Mesh m = Cone(.3);
        int n = m.num();
        m.add( Vec3f(0,0,-1) );
        m.add( n );
        m.translate(0,0,1);
        m.mode(GL::LS); 
    m.store(); 
        return m;
    
    } 



    inline Mesh Mesh::Disc (double scale){
        
        int res = floor( scale * 100);
        Mesh m;
        m.add( Vec3f(0,0,0) );
        
        for (int i = 0; i < res; ++i){
        
            float rad = 2.0 * PI * i / res;
            float x = cos(rad) * scale;
            float y = sin(rad) * scale;
            
            m.add( Vec3f(x,y,0) );
            
        }

        for (int i = 1; i < res; ++i){    
            int idx[2] = {i,0};
            m.add( idx, 2);
        }


        m.add(1);
        m.mode(GL::TS);
        m.store(); 
        return m;
    }
    
    inline Mesh Mesh::IRect (float w, float h) {
        
        Mesh m;
        
        Vec3f lb (-w/2.0, -h/2.0, 0 );
        Vec3f rb = lb + Vec3f(w,0,0);
        Vec3f rt = rb + Vec3f(0,h,0);
        Vec3f lt = rt - Vec3f(w,0,0);
        
        Vec3f normal(0,0,1);
        Vertex va( lt, normal, Vec4f(1,1,1,1), Vec2f(0.,0.));
        Vertex vb( rt, normal,  Vec4f(1,1,1,1), Vec2f(1.,0.));
        Vertex vc( rb, normal, Vec4f(1,1,1,1), Vec2f(1.,1.));
        Vertex vd( lb,  normal, Vec4f(1,1,1,1), Vec2f(0.,1.));
        
        m.add(va).add(vb).add(vc).add(vd);
        
        int idx[4] = {0,1,3,2};
        m.add(idx,4);
                
        m.mode(GL::TS);  
    m.store(); 
        return m;
    }   

    inline Mesh Mesh::Rect (float w, float h) {
        
        Mesh m;
        
        Vec3f lb (-w/2.0, -h/2.0, 0 );
        Vec3f rb = lb + Vec3f(w,0,0);
        Vec3f rt = rb + Vec3f(0,h,0);
        Vec3f lt = rt - Vec3f(w,0,0);
        
        Vec3f normal(0,0,1);
        Vertex va( lt, normal, Vec4f(1,1,1,1), Vec2f(0.,1.));
        Vertex vb( rt, normal,  Vec4f(1,1,1,1), Vec2f(1.,1.));
        Vertex vc( rb, normal, Vec4f(1,1,1,1), Vec2f(1.,0.));
        Vertex vd( lb,  normal, Vec4f(1,1,1,1), Vec2f(0.,0.));
        
        m.add(va).add(vb).add(vc).add(vd);
        
        int idx[4] = {0,1,3,2};
        m.add(idx,4);
                
        m.mode(GL::TS);  
    m.store(); 
        return m;
    }
    
    inline Mesh Mesh::Cylinder (float r, float height, int slices, int stacks){
   
        Mesh m;
        m.add( Vec3f(0,-height/2.0,0), Vec3f(0,-1,0) );

        for (int i = 0; i <= stacks; ++i){
                  
          float y = -height/2.0 + height * i/stacks;

          for (int j = 0; j < slices; ++ j){
            float rad = TWOPI * j / slices;
            float x = cos(rad) * r;
            float z = -sin(rad) * r;
                
            m.add( Vec3f(x,y,z), Vec3f(x, 0,z ).unit() );
                
          }
        }

        m.add( Vec3f(0, height/2.0, 0), Vec3f( 0,1,0 ) );
        

        //TOP
        int last = m.vertex().size() -1;
        m.add( last - slices ).add( last); 
        for (int j = last - slices + 1; j < last; ++j){
          int idx[2] = {j,last};
          m.add(idx,2);
        }
        m.add( last - slices).add(last).add(last); //add twice to finish

        //BOTTOM
        m.add(1).add(1).add(0);                     //add twice to start
        for (int j = slices; j > 1; --j){
            int idx[2] = {j,0};
            m.add(idx, 2);
        }
        m.add(1).add(0).add(0); 

        for(int i = 0; i < stacks; ++i){
          m.add(i*slices+1).add(i*slices+1);
          for (int j = 0; j < slices; ++j){
            int ix = i * slices + j + 1;
            int ixn = (j < slices - 1) ? ix + 1 : i * slices + 1;
            int nx = ix + slices;
            int nxn = (j < slices -1 ) ? nx + 1: ixn + slices;
            int idx[2] = { nxn, ixn };
            m.add(idx,2);
          }
          m.add(i*slices+2+slices).add(i*slices+2+slices);
        }
        
        m.mode(GL::TS);
        m.store(); 
        return m;
    }
    
    inline Mesh Mesh::Frame(float size){
    Mesh m;
    m.add( Vertex( Vec3f(0,0,0), Vec3f(0,0,1), Vec4f(1,1,1,1) ) ).add();
      m.add( Vertex( Vec3f(size,0,0), Vec3f(0,0,1), Vec4f(1,0,0,1) )).add().add(0);
      m.add( Vertex( Vec3f(0,size,0), Vec3f(0,0,1), Vec4f(0,1,0,1) )).add().add(0); 
      m.add( Vertex( Vec3f(0,0,size), Vec3f(0,0,1), Vec4f(0,0,1,1) )).add().add(0);
    m.mode( GL::LS );
    m.store();
    return m;
  }
    

    // inline Mesh Mesh::Skin (Cir * cir, int num, int res){
    //      
    //      Mesh m;
    //      
    //      for (int i = 0;  i < res; ++i){
    //          double t= 1.0 * i/res;
    //          
    //          for (int j = 0; j < num; ++j){
    //              double jt = 1.0 * j/num;
    //              Vec v = Ro::pnt_cir( cir[j], TWOPI * t );
    //              m.add( v[0], v[1], v[2] );
    //              m.last().Col = Vec4f(t,jt,1-t,1.0);
    //              //cout << v << endl; 
    //          }
    //      }
    //      
    //      //Calc Indices (FOR TRIANGLE STRIP)
    //      int a,b,c,d;
    //      for (int i = 0; i < num-1; ++i){
    //          for (int j = 0; j < res; ++j){
    //          
    //              a = j * num + i;                //<-- current idx                        
    //              b = a + 1;                      //<-- next circle
    //              if (j>=res-1) { 
    //                 // cout << "has looped around" << endl; 
    //                  c = i + 1;
    //                  d = i;
    //              }
    //              else {
    //                  c = a + 1 + num ;           //<-- next on next circle
    //                  d = a + num ;               //<-- next on same circle
    //              }
    //              int idx[2] = {a,c};
    //              m.add(idx,2);
    //          }
    //          m.add(i).add(i+1+num);
    //      }                
    //      
    //      //calc normals
    //      for (int i = 0; i < res; ++i){
    //          for (int j = 0; j < num-1; ++j){
    //              a = i * num + j;        //<-- current idx
    //              b = a + 1;              //<-- same on next circle
    //              
    //              if (i>=res-1) { 
    //                  c = j + 1;
    //                  d = j;
    //              }
    //              else {
    //                  c = a + 1 + num ;           //<-- next on next circle
    //                  d = a + num ;           //<-- next on same circle
    //              }
    //              Vec3f ta = m[b].Pos - m[a].Pos;
    //              Vec3f tb = m[d].Pos - m[a].Pos;
    //              Vec3f tc = -ta.cross(tb);
    //              m[a].Norm = tc.unit();
    //              //cout << m[a].Norm[0] << " " << m[a].Norm[1] << " " <<  m[a].Norm[2] << endl;
    //          }
    //      }
    //      
    //      return m;
    //  
    //  }














    
    ///////////////
    ///////////////
    ///////////////
    ///////////////
    ///////////////
    
// struct CirMesh{
// 
//     vector<Cir> vc;
//     vector<Pnt> vp;
//     int res;
//     
//     CirMesh& add(const Cir& c) { vc.push_back(c); return *this; }
// 
//     void skin() { 
//         res = vc.size();
//         for (int i = 0; i <= res; ++i){
//             double t= 1.0 * i/res;
//             for (int j = 0; j < res; ++j){
//                 vp.push_back( Ro::pnt_cir(vc[j], t * TWOPI) );
//             }
//         }
//     }
//     
//     void draw(float r = .5, float g=.5, float b=.5, float a = 1.0) {
//         for(int i = 0; i < res; ++i){
//             for(int j = 0; j < res-2; ++j){
//                 glColor4f(r,g,b,a);
//                 glBegin(GL_QUADS);
//                 int a = i * (res) + j;
//                 int b = a + 1;
//                 int c = b + res ;
//                 int d = c - 1 ;
//                 Vec n = Ro::dir( vp[a] ^ vp[b] ^ vp[c] ).dual();
//                 GL::normal( (n.unit() * -1.0).w() );
//                 GL::Quad( vp[a], vp[d], vp[c], vp[b] );
//                // GL::vertex(vp[a].w());
//                 glEnd();
// 
//                 //ENABLE for PRINTING
// //                glColor3f(.7,.7,.7);
// //                glNormal3f(0,0,1);
// //                glBegin(GL_LINE_STRIP);
// //                GL::Tri( vp[a], vp[b], vp[c] );
// //                GL::Tri( vp[a], vp[d], vp[c] );
// //                glEnd();
//             }
//             
//         }
//     }
// };

///Skin a bunch of Circles
//        void skinCirc (Cir * s, int num, int res = 10){
//
//           int mU = res;
//           int mV = num;
//                                    
//            if (!mVertex.empty() ) mVertex.clear();  
//
//            for (int j = 0; j < mU; ++j){
//                double t = 2 * PI * j/mU;
//                for (int i = 0 ; i < mV; ++i){          
//                    Par p = Ro::par_cir( s[ i ], t );
//                    Pnt tp =  Ro::cen( Ro::split(p)[0] );
//                    mVertex.push_back( Vertex( Vec3f(tp[0], tp[1], tp[2]) ) ) ;
//                }
//            }
//            
//            //store quad indices and normals
//
//            if (!mIndex.empty()) mIndex.clear();
//            
//            for (int j = 0; j < mU-1; ++j){
//                for (int i = 0; i < mV-1; ++i){
//                    int idx = j * mV + i;
//                    int idx2 = idx + mV;
//                    mIndex.push_back( idx);
//                    mIndex.push_back( idx+1);
//                    mIndex.push_back( idx2+1);
//                    mIndex.push_back( idx2);
//                }
//            }
//            //last strip
//            for (int i = 0; i < mV-1; ++i){
//                int idx = (mU-1) * mV + i; 
//                int idx2 = i;
//                 mIndex.push_back( idx);
//                 mIndex.push_back( idx+1);
//                 mIndex.push_back( idx2+1);
//                 mIndex.push_back( idx2);
//            }
//
//        }


// struct UVMesh{
//     int u, v;
// 
//     vector<Pnt> vp; ///<-- VECTORS
//     vector<Vec> np; ///<-- NORMALS
//     bool bFlipNormals;
//     UVMesh(int _u=0, int _v=0) : u(_u), v(_v), bFlipNormals(0) {}
//     void add(const Pnt& p) { vp.push_back(p); }
//     void add(const Vec& v) { vp.push_back(v.null()); }
//     
//     Vec normal(int a, int b, int c){
//             Vec dir  = bFlipNormals ?  Ro::dir( vp[a] ^ vp[c] ^ vp[b] ).dual() : Ro::dir( vp[a] ^ vp[b] ^ vp[c] ).dual();
//             return dir.unit();
//     }
//     
//     void draw(float rr = 1, float gg = 1, float bb = 1, float aa = 1){
//     
//         glColor4f(rr,gg,bb,aa);
//         glBegin(GL_QUADS);
//         for (int i = 0; i < u-1; ++i){
//             for (int j = 0; j < v-1; ++j){
//                 int a = i * v + j;
//                 int b = a + 1;
//                 int c = b + v;
//                 int d = c - 1;
//                 
//                 GL::normal(normal(a,b,c).w());
//                 GL::Quad(vp[a], vp[b], vp[c], vp[d]);
//             
//             }
//         }
//         glEnd();
//     }
//     
//     void drawTri(float rr=1, float gg=1, float bb=1, float aa =1){
//     
//      glColor4f(rr,gg,bb,aa);
//       for (int i = 0; i < u-1; ++i){
//             for (int j = 0; j < v-1; ++j){
//                 int a = i * v + j;
//                 int b = a + 1;
//                 int c = b + v;
//                 int d = c - 1;
//                 
//                 
//                 glBegin(GL_LINE_STRIP);
//                 GL::normal(normal(a,b,c).w());
//                 GL::Tri( vp[a], vp[b], vp[c]);
//                 GL::Tri(vp[a], vp[d], vp[c]);
//                 glEnd();
//             }
//         }
//     }
//     
//     void drawNormals(float rr = 1, float gg = 1, float bb = 1, float aa = 1){
//     
//         glColor4f(rr,gg,bb,aa);
//         glBegin(GL_QUADS);
//         for (int i = 0; i < u-1; ++i){
//             for (int j = 0; j < v-1; ++j){
//                 int a = i * v + j;
//                 int b = a + 1;
//                 int c = b + v;
//                 int d = c - 1;
//                 
//                 Vec mid = Interp::surface<Vec>( vp[a], vp[b], vp[c], vp[d], .5, .5);
//                 GL::Glyph::Line( mid, mid + normal(a,b,c) );
//                 //GL::Quad(vp[a], vp[b], vp[c], vp[d]);
//             
//             }
//         }
//         glEnd();
//     
//     }
// };



// struct CubeMesh{
//      vector<Pnt> vp;
//      void add(const Pnt& p ) { vp.push_back(p); }
//      
//      void draw(int dim){
//         
// //        IT3V(dim)
// //            int a = i * (dim+1) * (dim+1) + j * (dim+1) + k;
// //            int b = a  
// //        END 
//      }
// };
} //vsr

#endif
