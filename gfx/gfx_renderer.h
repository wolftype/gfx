/*
 * =====================================================================================
 *
 *       Filename:  gfx_renderer.h
 *
 *    Description:  render to OpenGL or OpenGLES
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

#include "gfx/gfx_layout.h"
#include "gfx/gfx_scene.h" 
#include "gfx/gfx_pipe.h"
#include "gfx/gfx_fbo.h"
#include "gfx/gfx_gl.h"
#include "gfx/gfx_glsl.h"    
 
namespace gfx {
	
  using namespace GLSL; 

  struct Process;

  /// CONTEXT BLIND RENDERER (NO WINDOWING . . .)
	struct Renderer { 
	    
		Layout layout; 		  ///< Screen Layout
	    
		Scene scene;   		  ///< ModelViewProjection Transformation Matrices  
		 	
		Pipe pipe;	   		  ///< Graphics Pipeline Vertex Attribute Binding  
    
    Process * process;  ///< Some EFFECTS 
		 
		Vec4f background;  	///< Background Color
		
		Mat4f mvm; 			    ///< Temporary save of scene's transformation matrix 

    int contextWidth, contextHeight; ///< tbd pixel dimensions of screen (needs to be fed from context)  	    
         

    /// RENDER FOR SINGLE COMPUTER AND SCREEN
	  Renderer (float w, float h, float z=30.0) : layout(1,1, w, h, 0, 0),
     background(0,0,0,1)
		{
			
			// initialize ES graphics
      initGLES(); 

      //CALCULATE CAMERA POSE FOR SINGLE SCREEN   		
			setView(z, false); 

    }
    
		/// MULTIPLE COMPUTERS AND SCREENS
		 Renderer( const Layout& l, float z = 30.0 ) : background(0,0,0,1), layout(l) {
		  	initGLES();
        setView(z, true);
		 } 
		
    ~Renderer(){}     
	 
		void initGLES(){
             
			glClearColor(1,1,1,1);
	    srand( time(NULL) );  

		  string V = DefaultVertES;                   ///< These shaders are defined in gfx_glsl.h 
			string F = DefaultFragES; 
            
			pipe.program = new ShaderProgram(V,F,0);    ///< The 0 here means load directly from string, not from file
			pipe.bindAll();		                          ///< Bind all vertex attributes
			
		}
	     
    virtual void init() = 0; 
		virtual void update() = 0;
		virtual void onDraw() = 0;   
       
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
			
			scene.camera.pos() = Vec3f( 0, 0, z); 
			scene.camera.view = View( scene.camera.pos(), p, aspect, h );

    }  
    
    /// DEFAULT PROCESS just renders the content from ondraw (OVERLOAD THIS for FX...)
    virtual void doProcess(){
      render();
    }

    virtual void render(){
		   	mvm = scene.xf.modelViewMatrixf();
	      pipe.bind( scene.xf );
	        onDraw();			 
		    pipe.unbind();  
    }
	
    void clear(){
       glViewport(0,0, contextWidth, contextHeight); 
	     glClearColor(background[0],background[1],background[2],background[3]);
	     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /// NOTE context must have a swapBuffers() method
    template<typename T>
	  void onFrame(T& context){
   	  
       clear();               ///< Clear Window
                 
       scene.onFrame();       ///< Apply ModelView and Projection Transformations
              
       update();              ///< Update Scene Data

       doProcess();           ///< Draw Scene Data  
		
			 context.swapBuffers(); ///< Call context's swapBuffer() method (NOTE: ASSUMED TO EXIST!)
			
		}  
		
    float width() { return layout.screenWidth; }
    float height() { return layout.screenHeight; }

	}; 
	
	  
  /*!
   *  A Rendering PROCESS is a type of GRAPHICS PIPELINE
   */
  struct Process : public Pipe {
      
      Process(int w, int h, Renderer * r = NULL) : 
      Pipe(), renderer(r), bEnable(true), amt(1), width(w), height(h) {}

      int width, height;            ///< Number of Pixels Width and Height

      Renderer * renderer;          ///< Pointer to Parent Renderer (which has the onFrame<T>(T&context) method)

      bool bEnable;                 ///< Enable this Process
     
      vector<Process*> mPre;        ///< List of Pre-Processing steps
      vector<Process*> mPost;       ///< List of Post-Processing steps

      void pre(Process& p) { mPre.push_back(&p); }     ///< Add Pre-Processing step
      void post(Process& p) { mPost.push_back(&p); }   ///< Add Post-Processing step

      void preProcess(){ for (auto& i : mPre ) (*i)(); }            ///< Execute Pre-Processing steps
      void postProcess(){ for (auto& i : mPost ) (*i)(); }          ///< Execute Post-Processing steps

      virtual void init() = 0;
      virtual void operator()() = 0;

      float amt;                    ///< Some Variable

  };

  /*!
   *  A SLAB billboards a texture to the screen
   *
   *  (note: the Slab does not need to know about parent renderer)
   */
  struct Slab : public Process {
       
    MBO * rect;
    Texture * texture;

    Slab(int w, int h) : Process(w,h) { init(); }

    virtual void init(){
      initShader();
    }
    virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
    }
    virtual void initTexture(){
      texture = new Texture( width, height );
    }
    virtual void initShader(){
      this->program = new ShaderProgram( ClipSpaceVertES, TFragES, 0);
      this->bindAll();
    }

    virtual void operator()(){
      this->bind();
       texture -> bind();
          this->line( *rect );
       texture -> unbind();
      this->unbind();
    }
  };

  /*!
   *  An ALPHA billboards a transparent texture to the screen
   */
  struct Alpha : public Process {

      MBO * rect;
      Texture * texture;

    Alpha(int w, int h) : Process(w,h){ init(); }
     
    virtual void init(){
      initShader();
    }

    virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
    }
    virtual void initTexture(){
      texture = new Texture( width, height );
    }
    virtual void initShader(){
      this->program = new ShaderProgram( ClipSpaceVertES, TFragAlphaES, 0);
      this->bindAll();
    }

     virtual void operator()(){
        this->bind();      
         this->program->uniform("alpha",amt);
          texture -> bind();
            this->line( *rect );
          texture -> unbind();
        this->unbind(); 
     }

  };

   /*!
   *  BLUR Process takes a slab and blurs it in the fragment shader
   */
  struct Blur : public Process {

      MBO * rect;
      Texture * texture;

      float ux, uy;

      Blur(int w, int h) : Process(w,h), ux(.1), uy(.1) { init(); }
     
     virtual void init(){
       initShader();
     }

     virtual void initShader(){
      this->program = new ShaderProgram( ClipSpaceVertES, TFragBlurES, 0);
      this->bindAll();
     }

     virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
     }

     virtual void initTexture(){
       texture = new Texture( width, height );
     }

     virtual void operator()(){
        this->bind();      
         this->program->uniform("ux",ux);
         this->program->uniform("uy",uy);
         this->program->uniform("bluramt",amt);
          texture -> bind();
            this->line( *rect );
          texture -> unbind();
        this->unbind(); 
     }

  };



  /*!
   *  A RENDER TO TEXTURE Process renders into a texture bound to the framebuffer colorbuffer
   *
   *  NO shader required for this step . . .

   */
  struct R2T : public Process { 
    
    FBO fbo;                                    ///< A Framebuffer
    Texture * textureA;                         ///< Texture into which to render
    Texture * textureB;                         ///< Secondary Texture for swapping buffers

    R2T(int w, int h, Renderer * r) : Process(w,h,r) { init(); }
    
    virtual void init(){
      cout << "INITIALIZING RENDER TO TEXTURE: " << width << " " << height << endl; 
      //initialize texture
      textureA = new Texture( width, height );
      textureB = new Texture( width, height );

      // Attach texture to FrameBuffer's ColorBuffer  
      fbo.attach(*textureA, GL::COLOR);          
    }

    virtual void operator()(){
      fbo.attach(*textureA, GL::COLOR);  
      fbo.bind();               
      
        glViewport(0, 0, width, height ); 
        glClearColor(0,0,0,1);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //do any preprocessing in the stack (i.e. motion blur)
        preProcess();
        //And add a new frame on top
        renderer -> render(); 
        //do any postprocessing in the stack
        postProcess();
        
      fbo.unbind(); 
    }

    void swap() { Texture * tmp = textureA; textureA = textureB; textureB = tmp;  };
  };


  /*!
   *  MOTION TRACE Process combines a R2T and an Alpha Slab
   */
  struct MotionTrace : public Process {
      
      R2T   r2t;
      Alpha trace;
      Slab  slab;
     // Blur blur;

      MotionTrace(int w, int h, Renderer * r ) : 
      Process(w,h,r), r2t(w,h,r), trace(w,h), slab(w,h) { init(); } 
      
      virtual void init(){
        ///Add alpha as a PreProcess to the r2t  
        r2t.pre(trace);

        trace.initRect();

        trace.texture = r2t.textureB;
        slab.rect = trace.rect;        //share resources
        slab.texture = trace.texture;  //redirect texture

        trace.amt = .95;
      }

      virtual void operator()(){
        
        r2t();

        slab();

        r2t.swap();
        trace.texture = r2t.textureB;
      }
  };





} //gfx:: 

#endif

