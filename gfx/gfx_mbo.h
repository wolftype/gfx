//  Created by Pablo Colapinto on 2/28/13.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef GFX_MBO_h
#define GFX_MBO_h

#include "gfx_vbo.h"
#include "gfx_vao.h"
#include "gfx_vattrib.h"
#include "gfx_mesh.h"

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

            VAO * vao;                  ///< pointer to simple binding
            VertexAttributes * vatt;    ///< pointer to vertex attributes


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

                       
            //Enable Vertex Attributes FIRST             
            void drawElements( int num = -1, int off = 0){ // GLenum mode,
                index.drawElements(mode, num, off);
            }
            
            void drawArray(){ //GLenum mode
                vertex.drawArray(mode);
            }

            /// Call BEFORE Enabling Vertex Attributes
            void bind() { 
                vertex.bind(); 
                index.bind(); 
            }


            void unbind() { 
                index.unbind(); 
                vertex.unbind();   
            }

            /// Here we can pass in vertex Attributes to point to . . . 
            // (useful for OPENGLES2.0 where there is no Vertex Array Object)
            void bind(VertexAttributes& vatt) { 
                vertex.bind(); 
                index.bind(); 
                vatt.enable();
                vatt.pointer();
            }

            void unbind(VertexAttributes& vatt){
               vatt.disable();
               index.unbind();
               vertex.unbind();
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
         *  RENDERING ROUTINES
         *-----------------------------------------------------------------------------*/
        struct GFX {
            
            static void Render(MBO& mbo, VertexAttributes& vatt) {
               mbo.bind();
                vatt.enable();
                vatt.pointer();
                  mbo.drawElements();
                vatt.disable();
               mbo.unbind();
            }

            static void Render(MBO& mbo, VAO& vao) {
               vao.bind();
                  mbo.drawElements();
               vao.unbind();
            }            
        };

}


#endif
