/*
 * =====================================================================================
 *
 *       Filename:  gfx_renderer.h
 *
 *    Description:  context-blind renderer for OpenGL or OpenGLES
 *                  Subclassed Application or Window context must define onDraw and onFrame
 *
 *        Version:  1.0
 *        Created:  02/17/2014 19:44:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef GFX_RENDERER_H_INCLUDED
#define GFX_RENDERER_H_INCLUDED

#include "gfx_layout.h"
#include "gfx_scene.h" 
#include "gfx_pipe.h"
#include "gfx_fbo.h"
#include "gfx_gl.h"
#include "gfx_glsl.h"    
 
namespace gfx {
  
  using namespace GLSL; 

 // struct Process;

  struct Renderer { 

    enum {
      GL = 0,
      GLES = 1,
    };
    enum {
      BUFFERED = 0,
      IMMEDIATE = 1
    };

    bool bES;            ///< OpenGL or OpenGLES 
    bool bImmediate;     ///< Immediate Mode or Buffered Mode
  
    Vec4f background;    ///< Background Color
    
    // MATRIX TRANSFORMS:
    Mat4f mvm;           ///< Temporary memoization of scene's current transformation matrix 
    Scene scene;         ///< ModelViewProjection Transformation Matrices  
    Layout layout;       ///< Screen Layout (for creating multiple screen frusta)

    int contextWidth, contextHeight; ///< Pixel width and height of screen
     
    // GRAPHICS BINDING:     
    Pipe pipe;           ///< Default Graphics Pipeline Vertex Attribute Binding  
   // Process * process;   ///< Some optional EFFECTS (see gfx_process.h) 

    /// RENDER FOR SINGLE COMPUTER AND SCREEN
    Renderer (float w, float h, float z=30.0) : 
      background(0,0,0,1), layout(1,1, w, h, 0, 0) {
       setView(z, false); //CALCULATE CAMERA POSE FOR SINGLE SCREEN  
    }
    
    /// MULTIPLE COMPUTERS AND SCREENS
     Renderer(const Layout& l, float z=30.0 ) : 
      background(0,0,0,1), layout(l) {
       setView(z, true);
     } 
    
    ~Renderer(){}     
  
    //Boolean specifies GL or GLES pipeline ...
    void initGL(bool ES, bool Immediate, int pw = 0, int ph = 0){
      bES = ES; bImmediate = Immediate;
      contextWidth = pw;
      contextHeight = ph;
       
      glClearColor(1,1,1,1);
      srand( time(NULL) );  

      string V = bES ? DefaultVertES : DefaultVert;        ///< These shaders are defined in gfx_glsl.h 
      string F = bES ? DefaultFragES : DefaultFrag; 
            
      pipe.program = new ShaderProgram(V,F,0);             ///< The 0 here means load directly from string, not from file
      pipe.bindAll();                                      ///< Bind all vertex attributes

      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
        
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
      glLineWidth(5);
      
    }
       
    // Subclassed Application or Window must define onDraw and onFrame
    virtual void onDraw() = 0;
    virtual void onFrame() = 0;  
    
    virtual void onResize(int w, int h){
      if (!bImmediate){
        scene.resize(w, h);
        printf("renderer resize to: %d %d\n", w, h);
      }

        contextWidth = w;
        contextHeight =h;

    }
   
    void drawFunc(){
      if (bImmediate) onDraw();
      else onFrame();
    }

    virtual void render(){
        scene.updateMatrices();
        mvm = scene.xf.modelViewMatrixf();
        pipe.bind( scene.xf );
          onDraw();       
        pipe.unbind();  
    }

    void clear(){
       glViewport(0,0,contextWidth,contextHeight);
       glClearColor(background[0],background[1],background[2],background[3]);
       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  
    void setView(float z, bool isGrid, int row=0, int col=0){
            
      float w = layout.screenWidth;
      float h = layout.screenHeight;   

      float aspect = w / h;   
             
      scene.fit(w,h);

      Pose p( -w/2.0,-h/2.0, 0);

      layout.speakerL = Vec3f( -w/2.0, 0, 0);
      layout.speakerR = Vec3f( w/2.0, 0, 0);

      //If we're in multi-screen mode, RE-DO pose positions based on grid layout . . .
      if (isGrid) {
        p = layout.poseOf( row, col ); 

        layout.speakerL = Vec3f( 
          layout.left( row, col ), 
          layout.bottom( row, col ) + layout.screenHeight / 2.0, 0);

        layout.speakerR = Vec3f(
           layout.left( row, col ) + layout.screenWidth, 
          layout.bottom( row, col ) + layout.screenHeight / 2.0, 0);
       }


      scene.viewpose = p;
      scene.camera.pos() = Vec3f( 0, 0, z); 
      scene.camera.view = View( scene.camera.pos(), p, aspect, h );

    }  
   
    void modelview(){
      pipe.program -> uniform("modelView", scene.xf.modelView ); 
    }

    void modelview( const Mat4f& mat ){ 
      static float mv[16];
      (mvm * mat).fill(mv);
      pipe.program -> uniform("modelView", mv );   
    }
        
    float width() { return layout.screenWidth; }
    float height() { return layout.screenHeight; }

  }; 
  


} //gfx:: 

#endif

