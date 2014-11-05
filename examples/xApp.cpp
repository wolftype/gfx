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

#include "gfx_scene.h" //matrix transforms

using namespace gfx;

struct MyApp : App<Window> {

  VAO * vao;            //<-- Vertex Array Object
  MBO * mbo;            //<-- Vertex Buffer Objects

  ShaderProgram * shader;

  Scene scene;

  virtual void init(){
  
   printf("My App Init\n");

   //Compile and Link Shader 
   shader = new ShaderProgram( GLSL::DefaultVert, GLSL::DefaultFrag, 0);  
   //Generate and Bind Vertex Array Object
   vao = new VAO();
   //Generate Buffer and Bind Vertex Data
   mbo = new MBO( Mesh::Circle() );   
   //Declare Attributes
   VertexAttrib(shader->id(), "position", sizeof(Vertex), 0); 
   VertexAttrib(shader->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
   VertexAttrib(shader->id(), "normal", sizeof(Vertex), Vertex::on()); 
   VertexAttrib(shader->id(), "texCoord", sizeof(Vertex), Vertex::ot()); 
   //Unbind Vertex Array Object
   vao -> unbind();

  }

  virtual void onDraw(){

    scene.updateMatrices();
    
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

  virtual bool onMouseDown(const Mouse& m){

    cout << m.x << " " << m.y << endl; 
    return false;
  }

};

int main(){

  MyApp app;
  app.start();
  return 0;
}
