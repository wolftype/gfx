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

/* struct TexMap : GFXRenderNode { */

/*   MBO mbo; */

/*   void init(){ */
/*                                       // w | h | spacing | lambda for assigning texture coordinates */
/*     auto m = mesh::grid<Vertex>( 10, 10, .2, [](int a, int b){ return Vec2f( (float)a/10, (float)b/10); } ); */
/*     mbo = m; */

/*   } */

/*   void onRender(){ */
    
/*     mbo.render( mDownstream -> vatt ); */

/*   } */

/* }; */


template<class T>
struct GFXMeshNode : GFXShaderNode {

  TMBO<T> mbo;

  void init(){
    for( auto& i : downstream().program->attributes() ){
      cout << i.first << endl;
      vatt.add(downstream().program->id(), i.first, sizeof(T), offset( i.first ) );
    }
  }

  GLvoid * offset(string s){
    if (s == "normal") return T::on();
    if (s == "sourceColor") return T::oc();
    if (s == "texCoord") return T::ot();
    return 0;
  }

  void onRender(){
    mbo.render( vatt );
  }
  
};


struct MyApp : GFXApp<GlutContext> {

 Texture * tex;
 
 GFXShaderNode shader;
 GFXMeshNode<VertexTexture> mesh;

 virtual void setup(){

    shader.program = new ShaderProgram( ClipSpaceVert, TFragAlpha, 0);

    //make a texture
    int w = 10; int h = 10;
    tex = new Texture(w,h);
    auto v = data::map<float>(w,h,4,[](float a, float b, float c){ 
        
        return c < .2 ? a : c < .4 ? b : c < .7 ? 0 : 1; 
    
    } );
    tex->update( &v[0]);

    //make a map into the texture coordinates
    auto m = mesh::uvtex<VertexTexture>( 11, 11, .2, [](float a, float b){ return Vec2f(a,b); } );
    mesh.mbo = m;

    shader << mesh;
    mesh.init();

    mRenderer.immediate(false);
}

 virtual void onDraw(){
    shader.program->bind();
    shader.program->uniform("alpha",1.f);
      tex->bind();
      mesh.onRender();
      // mbo.render( vatt );
    tex->unbind();
    shader.program->unbind();
 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
