/*
 * =====================================================================================
 *
 *       Filename:  xTexture.cpp
 *
 *    Description:  make a texture and experiment with texture coordinate functions
 *
 *        Version:  1.0
 *        Created:  02/03/2015 13:04:01
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

#include "util/gfx_stat.h"
#include "gfx/gfx_map.h"

using namespace gfx;

typedef Vertex VERTEXTYPE;



struct MyApp : GFXApp<GlutContext> {

 //Some Texture
 Texture * tex;

 //Some Shader
 ShaderProgram * shader;
 
 //Some Mesh Buffer 
 MBO * mesh;


 virtual void setup(){

    shader = new ShaderProgram( ClipSpaceVert, TFragAlpha ); ///<-- these shaders are defined in gfx_glsl.h

    //make a texture
    int w = 10; int h = 10;
    tex = new Texture(w,h);

    //we will fill our texture with a function...
    //data::map takes a width height and depth maps a function with inputs in the range of [0,1)
    auto v = data::map<float>(w,h,4,[](float a, float b, float c){ 
        
        return c < .2 ? a : c < .4 ? b : c < .7 ? 0 : 1; 
    
    } );

    //update texture data
    tex->update(&v[0]);

    //make a mesh that maps into the texture coordinates
    auto m = mesh::uvtex<VERTEXTYPE>( 11, 11, .2, [](float a, float b){ return Vec2f(a,b); } );
    mesh = new MBO(m);

    //bind mesh vertex attributes to shader
    mesh->bindTo(*shader);

}

 virtual void onDraw(){
    
    static float counter = 0.0; counter+=.02;

    shader->bind();
    shader->uniform("alpha", (float)fabs(sin(counter)) );
      tex->bind();
      mesh->render();
      tex->unbind();
    shader->unbind();
 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
