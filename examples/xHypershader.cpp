/*
 * =====================================================================================
 *
 *       Filename:  xHypershader.cpp
 *
 *    Description:  hyperk
 *
 *        Version:  1.0
 *        Created:  06/04/2014 00:54:19
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

#include "gfx_hypershader.h"

using namespace std;
using namespace gfx;


struct MyProcess : Process {

  //  R2T r2t;
  //  Blur blur;
    float amt;
    
    MyProcess(int w, int h, Renderer *r ) : 
    Process(w,h,r)//, r2t(w,h,r), blur(w,h)
    { init(); }

    virtual void init(){
      initShader();
      amt = 0;
    }
    
   void initShader(){
    this->program = new ShaderProgram( hyper, DefaultFrag, 0 );
    this->bindAll(); 
   }

  virtual void update(){
     amt += .01;
     this->program->uniform("amt",amt);
  }

   virtual void operator()(){

    
    this->bind();
        update();
        this->bind( renderer -> scene.xf );
        renderer -> onDraw();

    this->unbind();   
  }

};

struct MyApp : public App<Window> {

  float time = 0;

  MBO * mbo;

  MyProcess *process;

  MyApp(int w, int h) : App<Window>(w,h) { init(); }

  void init(){

    Mesh mesh;
    
    Vertex v(1,1,0);

    v.Col = Vec4f(-1,-1,-1,1);   mesh.add(v).add();
    //v.Pos.set(1,-1,0);

    v.Col = Vec4f(-1,1,-1, 1);   mesh.add(v).add();
    //v.Pos.set(-1,-1,0);

    v.Col = Vec4f(1,1,-1,  1);   mesh.add(v).add();
    //v.Pos.set(-1,1,0);

    v.Col = Vec4f(1,-1,-1, 1);   mesh.add(v).add().add(0);
    

    v.Col = Vec4f(-1,-1,1, 1);   mesh.add(v).add();
    v.Col = Vec4f(-1,1,1,  1);   mesh.add(v).add();
    v.Col = Vec4f(1,1,1,   1);   mesh.add(v).add();
    v.Col = Vec4f(1,-1,1,  1);   mesh.add(v).add().add(4);

    v.Col = Vec4f(-1,-1,-1,-1);  mesh.add(v).add();
    v.Col = Vec4f(-1,1,-1, -1);  mesh.add(v).add();
    v.Col = Vec4f(1,1,-1,  -1);  mesh.add(v).add();
    v.Col = Vec4f(1,-1,-1, -1);  mesh.add(v).add().add(8);

    v.Col = Vec4f(-1,-1,1, -1);  mesh.add(v).add();
    v.Col = Vec4f(-1,1,1,  -1);  mesh.add(v).add();
    v.Col = Vec4f(1,1,1,   -1);  mesh.add(v).add();
    v.Col = Vec4f(1,-1,1,  -1);  mesh.add(v).add().add(12);

    mesh.mode(GL::LS);

    mbo = new MBO( mesh );
    process = new MyProcess( 0, 0,this);
  }

  virtual void update(){
    time += .01;

  }
  
  virtual void onDraw(){
    pipe.line(*mbo);  
  }

  virtual void onFrame() { 
     update();          
     Renderer::clear();
     scene.updateMatrices();
    
   (*process)();
  //  Renderer::render(); 
    
     Window::SwapBuffers(); 
  }    

};

int main(int argc, char ** argv){
  MyApp app(500,500);// argc, argv);
  app.start();
  return 0;
}

