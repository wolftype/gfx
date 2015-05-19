/*
 * =====================================================================================
 *
 *       Filename:  xDisplacementMap.cpp
 *
 *    Description:  render to a texture then use texture to displace vertices
 *
 *        Version:  1.0
 *        Created:  05/28/2014 17:14:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include "gfx/util/glut_window.hpp"

#include "gfx_app.h"
#include "gfx_mbo.h"
#include "gfx_render.h"

using namespace std;
using namespace gfx;


/*!
 *  A Displacement Method
 */
struct DisplacementSlab : public GFXShaderNode {

    MBO * grid;
    Texture * texture;

    float spacing=0.1;
    float amt=1.0;

    virtual void init(){

     string vert = ufloat("amt") + AVertex() + Varying() + VDisplaceCalcSimple() + MVert();

     this->program = new ShaderProgram( 
                          vert,//makeDisplacementVert(bES),
                          //ClipSpaceVert,
                          // DefaultVert,
                          DefaultFrag());


     bindAttributes(); //default

     grid = new MBO( Mesh::HexaGrid(40,40,spacing) );//new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); //
     grid -> mesh.mode( GL::L );
   }


  virtual void onRender(){ 
       for (auto& i : mUpstream) i->onRender();

       if (mDownstream) glViewport(0,0,mDownstream->width, mDownstream->height);

       program->bind();  
          this->program->uniform("amt",amt); 
          texture->bind();
          grid->render(vatt);
          texture->unbind(); 
       program->unbind();    
  }

};


struct Displace : public GFXRenderNode {

  RenderToTexture r2t;
  DisplacementSlab dispmap;
  bool bDrawDispMap=false;

  virtual void init(){

    r2t.set(width,height);
    r2t.init();
    dispmap.set(width,height);
    dispmap.init();
    dispmap.texture = r2t.texture;

    dispmap << r2t;
    bindUpstream(r2t);
    bindDownstream(dispmap); 
  }

  void onRender(){
    dispmap.onRender();
  }

};



struct MyApp : public GFXApp<GlutContext> {

  float time = 0;

  MBO * mbo;
  MBO * grid;
  Displace process;

  void setup(){
    mbo = new MBO( Mesh::Circle() );
    //grid =  new MBO( Mesh::HexaGrid(40,40,.1) );//new MBO( Mesh::Grid2D(40,40,.1) );//n

    mRenderer << process <<  mSceneRenderer;

    process.set(width,height);
    process.init();
  }

  virtual void onAnimate(){
    time += .01;
    /* process.blur.ux = sin(time) * .2; */
    /* process.blur.uy = cos(time) * .2; */
    /* process.blur.amt = .1;// fabs(sin(time)); */
    process.dispmap.amt = 1 + sin(time)*10;
    mesh::move(mbo->mesh,sin(time)*5,0,0);
    mbo->update();
  }
  
  virtual void onDraw(){
    glLineWidth(10);
    draw(*mbo);
   // draw(*grid);
  }

};

int main(int argc, char ** argv){
  MyApp app;  
  app.start();
  return 0;
}

