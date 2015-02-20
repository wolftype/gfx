/*
 * =====================================================================================
 *
 *       Filename:  xApp.cpp
 *
 *    Description:  just an app with glut windowing and a shaderprogram
 *
 *        Version:  1.0
 *        Created:  10/08/2014 14:54:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "gfx_app.h"
#include "util/glut_window.hpp"

#include "gfx_vao.h"
#include "gfx_mbo.h"
#include "gfx_glsl.h"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

  VAO * vao;            //<-- Vertex Array Object
  MBO * mbo;            //<-- Vertex Buffer Objects

  ShaderProgram * shader;

  virtual void setup(){
  
   printf("My App Setup\n");

   //Compile and Link Shader 
   
   shader = new ShaderProgram( GLSL::DefaultVert, GLSL::DefaultFrag, 0);  
   
   //Generate and Bind Vertex Array Object
   vao = new VAO();
   vao->bind();
   
     //Generate Buffer and Bind Vertex Data
     mbo = new MBO( Mesh::Circle() );   
     mbo->bind();

     //Declare Attributes     
     mbo->bindAttributes(*shader); //this does basically what is commented out below

    //VertexAttrib(shader->id(), "position", sizeof(Vertex), 0); 
    //VertexAttrib(shader->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
    //VertexAttrib(shader->id(), "normal", sizeof(Vertex), Vertex::on()); 
    //VertexAttrib(shader->id(), "texCoord", sizeof(Vertex), Vertex::ot()); 
    
   //Unbind Vertex Array Object
   vao -> unbind();
   mbo -> unbind();
   mSceneRenderer.immediate(false);
  }

  virtual void onAnimate(){
    static float counter = 0;
    counter += .05;

    mesh::move( mbo->mesh,sin(counter) * 2.0,0,0);
    mbo->update();
  }

  virtual void onDraw(){
    shader->bind(); 
  
      shader -> uniform("lightPosition", 2.0, 2.0, 2.0);  
      shader -> uniform("projection",  scene.xf.proj);
      shader -> uniform("normalMatrix", scene.xf.normal);  
      shader -> uniform("modelView",  scene.xf.modelView ); 

      vao->bind();
        mbo->drawElements();
      vao->unbind();

    shader->unbind();

  }

};

int main(){

  MyApp app;
  app.start();
  return 0;
}
