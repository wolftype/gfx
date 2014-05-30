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

#include "util/GlutWindow.h"
#include "gfx_app.h"
#include "gfx_mbo.h"
#include "gfx_process.h"

using namespace std;
using namespace gfx;


/*!
 *  A Displacement Method
 */
struct Displacement : public Process {

    MBO * grid;

    float spacing, amt;

    //pass in num points
    Displacement(int w, int h, float s, Renderer * r) : 
    Process(w,h,r), spacing(s) { init(); }

    virtual void init(){
     initShader();

  //  Mesh m = Mesh::UV(vec, w, h);
    grid = new MBO( Mesh::HexaGrid(width,height,spacing) );//Mesh::Grid2D(width,height,spacing) );//
    grid -> mode = GL::LS;
    // Mesh::Grid2D(w,h,.1) );//
   }

   virtual void initShader(){
    this->program = new ShaderProgram( 
                        DisplacementVert, 
                        DefaultFrag, 0);
    this->bindAll();
   } 


  virtual void update(){
     this->program->uniform("amt",amt);
  }

  virtual void enter(){
      this->bind();
       this->bind( renderer -> scene.xf );  
  }

  virtual void leave(){
    this->unbind();
  }

  virtual void operator()(){
      this->bind();
       this->bind( renderer -> scene.xf );  
       update();     
       this->line(*grid); 
      this->unbind();    
  }

  virtual void draw(){
    this->line(*grid);
  }

};


struct MyProcess : public Process {

  R2T r2t;
  Slab slab;
  Blur blur;

  Displacement dispmap;
  bool bDrawDispMap;

  MyProcess(int w, int h, Renderer * r) : 
  Process(w,h,r), r2t(w,h,r), dispmap(40,40,.1,r), 
  blur(w,h), slab(w,h), bDrawDispMap(false)
  { init(); } 

  virtual void init(){

    slab.initRect();                // Initialze a rectangle
    slab.texture = r2t.textureB;
    blur.rect = slab.rect;          // Share resources
    blur.texture = slab.texture;  

   // r2t.post(blur);

  }

  void operator()(){

   // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    r2t();        // Render to Offscreen Texture 

   // r2t.swap();
   // r2t.fbo.attach(*r2t.textureB, GL::COLOR);
    r2t.fbo.bind();
      blur();
    r2t.fbo.unbind();
    
    //switch to original viewport
    glViewport(0, 0, renderer -> contextWidth, renderer -> contextHeight ); 
    //renderer -> clear();

    if (bDrawDispMap){
      slab();
    }
    else {
      slab.texture->bind();
      //  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1);
        dispmap();
      slab.texture->unbind();
    }

    r2t.swap();   // Swap textures
  }

};



struct MyApp : public App<Window> {

  float time = 0;

  MBO * mbo;
  MBO * grid;
  MyProcess * process;

  MyApp(int w, int h) : App<Window>(w,h) { init(); }

  void init(){
    mbo = new MBO( Mesh::Circle() );
    grid = new MBO( Mesh::Grid2D(40,40,.1) );

    cout <<  Window::window().width() << " WIDTH " << endl;
    process = new MyProcess( Window::window().width()/10, Window::window().height()/10, this );
  }

  virtual void update(){
    time += .01;
    process -> blur.ux = sin(time) * .2;
    process -> blur.uy = cos(time) * .2;
    process -> blur.amt = .1;// fabs(sin(time));
    process -> dispmap.amt = sin(time) * 10;

  }
  
  virtual void onDraw(){
    pipe.line(*mbo);  
  }

  virtual void onFrame() { 
     update();          
     Renderer::clear();
    
     (*process)();
   // Renderer::render(); 
    
     Window::SwapBuffers(); 
  }    
};

int main(int argc, char ** argv){
  MyApp app(500,500);// argc, argv);
  app.start();
  return 0;
}

