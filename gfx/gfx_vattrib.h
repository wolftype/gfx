//
//  vsr_gl_vattrib.h
//  Versor
//
/*

    Vertex Attribute Data

      id, 
*/

//  Created by Pablo Colapinto on 10/25/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef GFX_gl_vattrib_h
#define GFX_gl_vattrib_h

#include "gfx_shader.h"

namespace gfx {

   class VertexAttrib {
      
    public:
     
        VertexAttrib() : bNormal(GL_FALSE),  mData(NULL) {}
        
        //Pass in a ShaderProgramID, attribute name as written in shader, and packing
        VertexAttrib( GLuint s, string attrib, GLsizei stride, GLvoid * data = NULL ) 
        : bNormal(GL_FALSE), mStride(stride), mData(data) {            
            get(s, attrib);
            enable();
            pointer();
        }

        void set(GLuint s, string attrib, GLsizei stride, GLvoid * data = NULL){
            mStride = stride;
            mData = data;
            get(s, attrib);
            enable();
            pointer();
        }
        
        void att(const Attribute& a) { mAtt = a; }
        
        void get( GLuint sprog, string attrib) {

            mAtt.getAttrib(sprog, attrib.c_str());
            mSize = GL::cmp(mAtt.vectype);
            
            GL::error("get vertex attribute");
                        
        }
        
        void enable()  const {   glEnableVertexAttribArray( mAtt.id );  GL::error("enable vattrib"); }
        void disable() const {   glDisableVertexAttribArray( mAtt.id ); GL::error("disable vattrib");}

        //Data is not sent here, just pointed to
        void pointer() const {
            glVertexAttribPointer(mAtt.id, mSize, mAtt.datatype, bNormal, mStride, mData);  
            GL::error("pointer");
        }

        /* void pointer(GLuint offset){ */
        /*     glVertexAttribPointer(mAtt.id, mSize, mAtt.datatype, bNormal, mStride, (GLvoid*) offset); */  
        /*     GL::error("pointer"); */
        /* } */

        /* static void draw(int n, GL::MODE = GL::LL); */
        /* static void drawElements(int n, void * ptr, GL::MODE, GL::TYPE = GL::UBYTE); */
        
        GLuint id() const { return mAtt.id; }
        
        //setters
        void normal( GLboolean b)   { bNormal = b; }
        void stride(GLsizei s)      { mStride = s; }        
        void data(void * udata)     { mData = (GLvoid*) udata; }
        
    private:
        
        GLboolean bNormal;      //normalize?
        GLsizei mStride;
        GLint mSize;            //must be 1,2,3,4
        
        GLvoid * mData;
        
        Attribute mAtt;
    };

    struct VertexAttributes {
        
        vector<VertexAttrib> vatt;
        
        void add(GLuint id, string name, GLsizei stride, GLvoid * offset ){
          vatt.push_back( VertexAttrib(id,name,stride,offset) );
        }
              
          void enable() const{
            for (int i=0;i<vatt.size();++i){
              vatt[i].enable();
            }
          }

          void pointer() const {
            for (int i=0;i<vatt.size();++i){
              vatt[i].pointer();
            }
          }

          void disable() const{
            for (int i=0;i<vatt.size();++i){
              vatt[i].disable();
            }
          }
    };

    /* inline void VAttrib::draw(int n, GL::MODE mode) { */ 
    /*     glDrawArrays(mode, 0, n); */ 
    /*     GL::error("vattrib draw"); */
    /* } */

    /* inline void VAttrib::drawElements(int n, void * ptr, GL::MODE mode, GL::TYPE t ) { */ 
    /*     glDrawElements(mode, n, t, ptr ); */ 
    /*     GL::error("vattrib drawelements"); */
    /* } */
    
    /* struct VAttrib4{ */
    /*     VAttrib pos, norm, col, tex; */
    /*     void enable() { pos.enable(); norm.enable(); col.enable(); tex.enable(); } */
    /*     void disable() { pos.disable(); norm.disable(); col.disable(); tex.disable(); } */
    /*     void pointer() { pos.pointer(0);  norm.pointer(0); col.pointer(0); tex.pointer(0); } */
    /* }; */
    
} //ctl::

#endif
