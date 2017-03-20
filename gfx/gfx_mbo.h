/*
 * =====================================================================================
 *
 *       Filename:  gfx_mbo.h
 *
 *    Description:  mesh buffer objects (vertex data and indexing) 
 *
 *        Version:  1.0
 *        Created:  02/06/2015 18:56:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


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

        template<class T>
        struct TMBO {
          
          static int mCount;
          int idx;

          VBO vertex;
          VBO index;

          //problem with storing attributes here is that not all shaders will have all attributes
          //so what to do when one mbo goes through multiple shaders?
          //maybe best to keep this separate, but it is really useful for rapid prototyping
          //can call mbo.render() to use this bound attrib, or mbo.render(vatt) to call some other list
          //opengles 3.0 can just use vertex array objects . . .
          VertexAttributes attrib;
          
           //Bind Vertex Attributes with Program Variables
           void bindAttributes(const ShaderProgram& program){
            attrib.vatt.clear(); //clear list of vertex attributes
            //bind(); 
              for (auto& i : program.attributes()){
                attrib.add( program.id(), i.first, sizeof(T), GLVertexData<T>::Init().Attribute[i.first] );
              }
            //unbind();
          }

          //Mesh mesh;
          MeshData<T> mesh; 
          //Mesh mesh;

          bool bUpdate = true;

          bool shouldUpdate() const { return bUpdate; }
          TMBO& setUpdate(bool v) { bUpdate=v; return *this; }

          TMBO(){}

            /// Copy Constructor
            TMBO(const TMBO& mbo){
              idx = mbo.idx;
              vertex = mbo.vertex;
              index = mbo.index; 
              mesh = mbo.mesh;
              bUpdate = mbo.bUpdate;
              
              vertex.data( mesh.vertices()[0].begin() );
              index.data( &mesh.indices()[0] ); 
                
            }
                     
            /// Assignment Operator         
            TMBO operator = (const TMBO& mbo){    
              if (this != &mbo ){
                idx = mbo.idx;
                vertex = mbo.vertex;
                index = mbo.index; 
                mesh = mbo.mesh;   
                bUpdate = mbo.bUpdate;
                 
                vertex.data( mesh.vertices()[0].begin() );
                index.data( &mesh.indices()[0] );
              }  
              return *this;      
            }   


          TMBO( MeshData<T> m, GL::USAGE usage = GL::STATIC, int id = -1 ) : mesh(m){
              mCount +=1;
              idx = id == -1 ? mCount : id;
              vertex = VBO( &mesh.vertices()[0].Pos[0], mesh.num(), mesh.num() * sizeof(T), GL::VERTEXBUFFER, usage );
              index = VBO( &mesh.indices()[0], mesh.numIdx(), mesh.numIdx() * sizeof(typename MeshData<T>::INDEXTYPE), GL::ELEMENTBUFFER, GL::STATIC );
          }
        

             /// Call bind BEFORE Enabling Vertex Attributes
            void bind() const { 
                vertex.bind(); 
                index.bind(); 
            }


            void unbind() const { 
                index.unbind(); 
                vertex.unbind();   
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
 
                                 
            //Enable Vertex Attributes Before Calling DrawElements 
            void drawElements( int num = -1, int off = 0) const { // GLenum mode,
                index.drawElements(mesh.mode(), num, off);
            }
            
            void drawArray() const{ //GLenum mode
                vertex.drawArray(mesh.mode());
            }

            void render() const {
              bind(attrib);
                drawElements();
              unbind(attrib);
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
       
            /// Update Vertex Info               
            void update(Vertex * val){
                vertex.update(val);
            }
            
            void update(){
              vertex.update();
            }
            
            ///Update Specific Vertex
            template< class S >
            void update(int idx, int num, const S& val){
              vertex.update(idx, num, &val);
            }          

            

        };

        template<class T> int TMBO<T>::mCount;
        typedef TMBO<Vertex> MBO;

       /*!-----------------------------------------------------------------------------
        *   A Mesh Buffer Object contains both vertex buffer and element buffer, 
        *   and pointers to Vertex Array Object that binds it, as well as VertexAttributes
        *   for cases (such as OpenGLES 2) where VAO's don't exist
        *
        *   note: element array is currently fixed to static (not dynamic) ... should change this 
        *-----------------------------------------------------------------------------*/   


/*         struct MBO { */

            
/*             static int mCount; */
/*             int idx; */
            
/*             VBO vertex;                 ///< vertex buffer container */
/*             VBO index;                  ///< element buffer container */
  
/*             Mesh mesh;                  ///< Actual mesh data on CPU */

/*             GL::MODE mode;              ///< GL_LINES, etc */

/*             bool bUpdate = true;       ///< whether to update mesh data */
/*             bool shouldUpdate() const { return bUpdate; } */
/*             MBO& setUpdate(bool v) { bUpdate=v; return *this; } */

/*             MBO& set(GL::MODE m) { */
/*               mode = m; */
/*               mesh.mode(m); */
/*               return *this; */
/*             } */
                  
/*             MBO(){} */ 

/*             /// Copy Constructor */
/*             MBO(const MBO& mbo){ */
/*               idx = mbo.idx; */
/*               vertex = mbo.vertex; */
/*               index = mbo.index; */ 
/*               mesh = mbo.mesh; */
/*               mode = mbo.mode; */  
/*               bUpdate = mbo.bUpdate; */
              
/*               vertex.data( &mesh.vertices()[0].Pos[0] ); */
/*               index.data( &mesh.indices()[0] ); */ 
                
/*             } */
                     
/*             /// Assignment Operator */         
/*             MBO operator = (const MBO& mbo){ */    
/*               if (this != &mbo ){ */
/*                 idx = mbo.idx; */
/*                 vertex = mbo.vertex; */
/*                 index = mbo.index; */ 
/*                 mesh = mbo.mesh; */
/*                 mode = mbo.mode; */    
/*                 bUpdate = mbo.bUpdate; */
                 
/*                 vertex.data( &mesh.vertices()[0].Pos[0] ); */
/*                 index.data( &mesh.indices()[0] ); */
/*               } */  
/*               return *this; */      
/*             } */           
      
/*             /// Constructor, default usage is static_draw */
/*             template<class T> */
/*             MBO( MeshData<T> m, GL::USAGE usage = GL::STATIC, int id = -1 ) : mesh(m){ */
/*                 mCount +=1; */
/*                 idx = id == -1 ? mCount : id; */
/*                 vertex = VBO( &mesh.vertices()[0].Pos[0], mesh.num(), mesh.num() * sizeof(T), GL::VERTEXBUFFER, usage ); */
/*                 index = VBO( &mesh.indices()[0], mesh.numIdx(), mesh.numIdx() * sizeof(typename MeshData<T>::INDEXTYPE), GL::ELEMENTBUFFER, GL::STATIC ); */
/*                 mode = mesh.mode(); */
/*             } */

/*             /1* MBO( Mesh m, GL::USAGE usage = GL::STATIC, int id = -1 ) : mesh(m){ *1/ */
/*             /1*     mCount +=1; *1/ */
/*             /1*     idx = id == -1 ? mCount : id; *1/ */
/*             /1*     vertex = VBO( &mesh.vertices()[0].Pos[0], mesh.num(), mesh.num() * sizeof(Vertex), GL::VERTEXBUFFER, usage ); *1/ */
/*             /1*     index = VBO( &mesh.indices()[0], mesh.numIdx(), mesh.numIdx() * sizeof(Mesh::INDEXTYPE), GL::ELEMENTBUFFER, GL::STATIC ); *1/ */
/*             /1*     mode = mesh.mode(); *1/ */
/*             /1*     bind(); *1/ */
/*             /1* } *1/ */
            

/*              /// Call bind BEFORE Enabling Vertex Attributes */
/*             void bind() const { */ 
/*                 vertex.bind(); */ 
/*                 index.bind(); */ 
/*             } */


/*             void unbind() const { */ 
/*                 index.unbind(); */ 
/*                 vertex.unbind(); */   
/*             } */ 
 
/*              /// Here we can pass in vertex Attributes to point to . . . */ 
/*             // (useful for OPENGLES2.0 where there is no Vertex Array Object) */
/*             void bind(const VertexAttributes& vatt) const { */ 
/*                 bind(); */ 
/*                 vatt.enable(); */
/*                 vatt.pointer(); */
/*             } */

/*             void unbind(const VertexAttributes& vatt) const{ */
/*                vatt.disable(); */
/*                unbind(); */
/*             } */
 
                                 
/*             //Enable Vertex Attributes Before Calling DrawElements */ 
/*             void drawElements( int num = -1, int off = 0) const { // GLenum mode, */
/*                 index.drawElements(mode, num, off); */
/*             } */
            
/*             void drawArray() const{ //GLenum mode */
/*                 vertex.drawArray(mode); */
/*             } */

/*             void render(const VertexAttributes& vatt) const { */
/*                bind(vatt); */
/*                 drawElements(); */
/*                unbind(vatt); */
/*             } */

/*             void render(const VAO& vao) const { */
/*               vao.bind(); */
/*                 drawElements(); */
/*               vao.unbind(); */
/*             } */
       
/*             /// Update Vertex Info */               
/*             void update(Vertex * val){ */
/*                 vertex.update(val); */
/*             } */
            
/*             void update(){ */
/*               vertex.update(); */
/*             } */
            
/*             ///Update Specific Vertex */
/*             template< class T > */
/*             void update(int idx, int num, const T& val){ */
/*               vertex.update(idx, num, &val); */
/*             } */

/*         }; */
        
/*         int MBO::mCount; */


          /*-----------------------------------------------------------------------------
           *  STATIC CREATION OF MESH BUFFER SINGLETON OBJECTS FOR DIFFERENT ELEMENTS 
           *  User must specify this to define draw routines
           *
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
              cout << "initializing mesh buffer" << endl;
              return m;
            }
            private:
              MeshBuffer();
              static vector<MBO> mMBO;
              static map<string,MBO> mMBOmap;
          };

 
          template<class T> vector<MBO> MeshBuffer<T>::mMBO; 
          template<class T> map<string,MBO>  MeshBuffer<T>::mMBOmap;  

         namespace meshbuffer{
           
           template<class T>
           MBO& make(const T& t){
              MeshBuffer<T>::Add(t);
              return MeshBuffer<T>::mbo(t);
           }
           
         } //meshbuffer::
}


#endif
