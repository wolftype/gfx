
#include "gfx_app.h"
#include "util/glut_window.hpp"

using namespace gfx;

struct MyApp : GFXApp<GlutContext> {

 MBO mbo;

 virtual void setup(){

    printf("MYAPP SETUP\n");
    mbo = Mesh::Circle();

    mRenderGraph.immediate(true);


    Quat qa( PI*1.25 / 2.0, Vec3f(0.0,1.0,0.0) );
    Quat qb( PI*-.25 / 2.0, Vec3f(0.0,1.0,0.0) );

    Vec3f nz = Quat::spin( Vec3f(0.0,0.0,1.0), qa); 
  
    cout << qa << endl;
    cout << qb << endl;

    Quat qc( PI*-.25 / 2.0, nz );

    cout << nz << endl;
    cout << qc << endl;

    Quat nq = qc * qa;
    Vec3f nx = Quat::spin(Vec3f(1.0,0.0,0.0),nq);

    cout << nq << endl;
    
    
    cout << nq * Quat(0,1,0,0) << endl;
    cout << nx << endl;


    
    
 }

 virtual void onDraw(){

    draw(mbo,1,0,0);
    //render::begin(1,1,1,1);
    //mesh::drawElements( mbo.mesh );
 }


};


int main(){

  MyApp app;
  app.start();
  return 0;
}
