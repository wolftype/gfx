/*
 * =====================================================================================
 *
 *       Filename:  xGLFW.cpp
 *
 *    Description:  using glfw window library
 *
 *        Version:  1.0
 *        Created:  09/05/2014 13:01:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "util/glfw_window.h"
#include "gfx_app.h"
#include "gl_mesh.hpp"
#include "gfx_mesh.h"
#include "gfx_shader.h"
//#include "gfx_glm.h"

using namespace gfx;


/*-----------------------------------------------------------------------------
 *  SOME SHADER CODE
 *-----------------------------------------------------------------------------*/
const char * vert = GLSL(120,
  attribute vec4 position;    //note: "attribute" should be changed to "in" on versions 130 and above

  void main(){
    gl_Position = position;
  }
);

const char * frag = GLSL(120, 
  void main(){
    gl_FragColor = vec4(1.0,1.0,1.0,1.0);
  }
);

/*-----------------------------------------------------------------------------
 *  SHADER CODE
 *-----------------------------------------------------------------------------*/
const char * vert2 = GLSL(120,

  attribute vec4 position;
  attribute vec3 normal;
  attribute vec4 color;            
  attribute vec2 textureCoordinate;
  
  uniform mat4 projection;
  uniform mat4 view;
  uniform mat4 model;
  uniform mat3 normalMatrix; //<-- This will be used to calculate our normals in world space
  
  varying vec4 destColor;   
  varying vec2 texCoord; 
  varying float diffuse;        

  //the simplest function to calculate lighting 
  vec4 doColor(){
    vec3 norm  = normalize( normalMatrix * normalize(normal) );
    vec3 light = normalize( vec3(1.0, 1.0, 1.0) );
    diffuse = max(dot(norm,light),0.0 );

    return vec4( (color * diffuse).xyz, 1.0);
  }

  void main(void){
    destColor = doColor();
    
    texCoord = textureCoordinate;
    gl_Position = projection * view * model * position;
  }

);


const char * frag2 = GLSL(120,

  uniform sampler2D sampler;

  varying vec4 destColor;        
  varying vec2 texCoord;  
  varying float diffuse; 

  void main(void){
    gl_FragColor = mix(destColor, vec4(texture2D( sampler, texCoord ).rbg * diffuse, 1.0), .1) ;  
  }

);


struct MyApp : App<Window> {

  MBO mbo;
  Mesh mesh;
  ShaderProgram * shader;  

  MyApp() : App<Window>() {}

  virtual void init(){
    shader = new ShaderProgram(vert,frag,0);
    mesh = Mesh::Sphere();
    mbo.add( VertexAttribute(shader->id(),3,"position",sizeof(Vertex),0) );
    mbo.buffer( mesh.vertex(), mesh.index() );
  }

  virtual void onDraw(){
    shader->bind();
  
    /* Model model; */
    /* View view; */
    /* Projection proj; */
    /* Normal norm; */
    
     /* Mat4f tmodel =  XMat::Identity();//mvm(); */    
     /* Mat4f tproj  = XMat::fovy( );//camera.proj(); */  
     /* Mat4f tnorm  = norm(); */

     /* copy(tmodel.val(), tmodel.val() + 16, xf.model); */
     /* copy(tview.val(), tview.val() + 16, xf.view); */
     /* copy(tproj.val(), tproj.val() + 16, xf.proj); */
     /* copy(tnorm.val(), tnorm.val() + 16, xf.normal); */
       
    /* shader -> uniform( "model", xf.model ); */
    /* shader -> uniform( "view", xf.view ); */
    /* shader -> uniform( "projection", xf.proj ); */
    /* shader -> uniform( "normalMatrix", xf.normal ); */
       
    /* shader -> uniform( "model", model.ptr() ); */
    /* shader -> uniform( "view", view().ptr() ); */
    /* shader -> uniform( "projection", proj().ptr() ); */
    /* shader -> uniform( "normalMatrix", norm(view.matrix, model.matrix).ptr() ); */

    /* glUniformMatrix4fv( modelID, 1, GL_FALSE, model.ptr() ); */ 
    /* glUniformMatrix4fv( viewID, 1, GL_FALSE, view.ptr() ); */
    /* glUniformMatrix4fv( projectionID, 1, GL_FALSE, proj.ptr() ); */ 
    /* glUniformMatrix4fv( normalMatrixID, 1, GL_FALSE, norm.ptr() ); */ 

    mbo.bind();
    mbo.drawArrays( GL_TRIANGLES, mesh.vertex() );
    mbo.unbind();
    
    shader->unbind();
  }
};

int main(){

  MyApp app;
  app.start();

  return 0;
}
