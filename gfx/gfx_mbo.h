//  Created by Pablo Colapinto on 2/28/13.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef GFX_MBO_h
#define GFX_MBO_h

#include "gfx_vbo.h"
#include "gfx_vao.h"
#include "gfx_vattrib.h"
#include "gfx_mesh.h"

#include <sstream>

namespace gfx{

       /*!-----------------------------------------------------------------------------
        *   A Mesh Buffer Object contains both vertex buffer and element buffer, 
        *   and pointers to Vertex Array Object that binds it, as well as VertexAttributes
        *   for cases (such as OpenGLES 2) where VAO's don't exist
        *
        *   note: element array is currently fixed to static (not dynamic) ... should change this 
        *-----------------------------------------------------------------------------*/   
        struct MBO {

            
            static int mCount;
            int idx;
            
            VBO vertex;                 ///< vertex buffer container
            VBO index;                  ///< element buffer container
  
            Mesh mesh;                  ///< Actual mesh data on CPU

            GL::MODE mode;              ///< GL_LINES, etc

            bool bUpdate = true;       ///< whether to update mesh data
            bool shouldUpdate() const { return bUpdate; }
            MBO& setUpdate(bool v) { bUpdate=v; return *this; }

            MBO& set( GL::MODE m) {
              mode = m;
              mesh.mode(m);
              return *this;
            }
                  
            MBO(){} 

            MBO(const MBO& mbo){
              idx = mbo.idx;
              vertex = mbo.vertex;
              index = mbo.index; 
              mesh = mbo.mesh;
              mode = mbo.mode;  
              bUpdate = mbo.bUpdate;
              
              vertex.data( &mesh.vertices()[0].Pos[0] );
              index.data( &mesh.indices()[0] );      
            }
                     
            MBO operator = (const MBO& mbo){    
              if (this != &mbo ){
                idx = mbo.idx;
                vertex = mbo.vertex;
                index = mbo.index; 
                mesh = mbo.mesh;
                mode = mbo.mode;    
                bUpdate = mbo.bUpdate;
                 
                vertex.data( &mesh.vertices()[0].Pos[0] );
                index.data( &mesh.indices()[0] );
              }  
              return *this;      
            }           
      
            MBO( Mesh m, GL::USAGE usage = GL::STATIC, int id = -1 ) : mesh(m){
                mCount +=1;
                idx = id == -1 ? mCount : id;
                vertex = VBO( &mesh.vertices()[0].Pos[0], mesh.num(), mesh.num() * sizeof(Vertex), GL::VERTEXBUFFER, usage );
                index = VBO( &mesh.indices()[0], mesh.numIdx(), mesh.numIdx() * sizeof(Mesh::INDEXTYPE), GL::ELEMENTBUFFER, GL::STATIC );
                mode = mesh.mode();
                bind();
            }

             /// Call BEFORE Enabling Vertex Attributes
            void bind() const { 
                vertex.bind(); 
                index.bind(); 
            }


            void unbind() const { 
                index.unbind(); 
                vertex.unbind();   
            } 
                                 
            //Enable Vertex Attributes FIRST             
            void drawElements( int num = -1, int off = 0) const { // GLenum mode,
                index.drawElements(mode, num, off);
            }
            
            void drawArray() const{ //GLenum mode
                vertex.drawArray(mode);
            }

            void render(const VertexAttributes& vatt) const {
               bind(vatt);
                drawElements();
               unbind(vatt);
            }

            void render(const VAO& vao) const {
              vao.bind();
                drawElements();
              vao.unbind();
            }



            /// Here we can pass in vertex Attributes to point to . . . 
            // (useful for OPENGLES2.0 where there is no Vertex Array Object)
            void bind(const VertexAttributes& vatt) const { 
                bind(); 
                vatt.enable();
                vatt.pointer();
            }

            void unbind(const VertexAttributes& vatt) const{
               vatt.disable();
               unbind();
            }
             
            /// Update Vertex Info               
            void update(Vertex * val){
                vertex.update(val);
            }
            
            void update(){
              vertex.update();
            }
            
            ///Update Specific Vertex
            template< class T >
            void update(int idx, int num, const T& val){
              vertex.update(idx, num, &val);
            }

//            //static methods to grab quick int
//            static int Buffer( Mesh& mesh, int id = -1){
//                mCount +=1;
//                VBO( &mesh.vertices()[0].Pos[0], mesh.num(), mesh.num() * sizeof(Vertex), GL::VERTEXBUFFER );
//                VBO( &mesh.indices()[0], mesh.numIdx(), mesh.numIdx() * sizeof(GLuint), GL::ELEMENTBUFFER );
//                return tmp;
//            }


        };
        
        int MBO::mCount;


          /*-----------------------------------------------------------------------------
           *  STATIC CREATION OF MESH BUFFER SINGLETON OBJECTS FOR DIFFERENT ELEMENTS 
           *  User must specify this to define draw routines
           *
           *  template<> MakeMeshBuffer<CLASSNAME>::MakeMeshBuffer(){
           *      mMBO.push_back( meshes....);
           *  }
           *-----------------------------------------------------------------------------*/
          template<class T>
          struct MeshBuffer{

            static vector<MBO>& mbo() { return mMBO; }

            static MBO& mbo(const T& t) { 
                stringstream fs; fs << &t;
                return mMBOmap[fs.str()];
            }

            static void Add(const T& m){};//{ mMBO.push_back(m); }

            static bool Exists(const T& t){
              stringstream fs; fs << &t;
              return !(mMBOmap.find(fs.str())==mMBOmap.end() );
            }

            static MBO& Get(const T& t){
              if ( !Exists(t) ){
                Add(t);
              }
              return mbo(t);
            }

            static vector<MBO>& Get(){
              if (mMBO.empty()) Init();
              return mbo();
            }
          
            static MeshBuffer& Init(){
              static MeshBuffer m;
              return m;
            }
            private:
              MeshBuffer();
              static vector<MBO> mMBO;
              static map<string,MBO> mMBOmap;
          };

 
          template<class T> vector<MBO> MeshBuffer<T>::mMBO; 
          template<class T> map<string,MBO>  MeshBuffer<T>::mMBOmap;  

          //template<>  vector<MBO>& MeshBuffer<MBO>::Get
                   
}


#endif
