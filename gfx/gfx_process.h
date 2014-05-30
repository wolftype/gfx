/*! @file
 *
 *  graphics processes (render to texture, etc)
 *
 * */
#ifndef  gfx_process_INC
#define  gfx_process_INC


#include "gfx_renderer.h"

namespace gfx{
    
  /*!
   *  A Rendering PROCESS is a type of GRAPHICS PIPELINE meant to throw together
   *  easy experiments in shader and rendering to textures, etc
   */
  struct Process : public Pipe {
      
      Process(int w, int h, Renderer * r = NULL) : 
      Pipe(), bEnable(true), width(w), height(h), renderer(r), bES(false) {
        #ifdef GFX_USE_GLES
          cout << "PROCESS USES GLES" << endl;
          bES = true;
        #endif  
      }

      virtual ~Process(){};

      bool bES;                     ///< Boolean use GL or GLES

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
      this->program = new ShaderProgram( bES ? ClipSpaceVertES : ClipSpaceVert, 
                                         bES ? TFragES : TFrag, 0 );
      this->bindAll();
    }

    virtual void draw(){
      this->line( *rect );
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
      float amt;

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
      this->program = new ShaderProgram( bES ? ClipSpaceVertES : ClipSpaceVert, 
                                         bES ? TFragAlphaES : TFragAlpha, 0);
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

      float ux, uy, amt;

      Blur(int w, int h) : Process(w,h), ux(.1), uy(.1), amt(1) { init(); }
     
     virtual void init(){
       initShader();
     }

     virtual void initShader(){
      this->program = new ShaderProgram( bES ? ClipSpaceVertES : ClipSpaceVert, 
                                         bES ? TFragBlurES : TFragBlur, 0);
      this->bindAll();
     }

     virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
     }

     virtual void initTexture(){
       texture = new Texture( width, height );
     }

    virtual void update(){
         this->program->uniform("ux",ux);
         this->program->uniform("uy",uy);
         this->program->uniform("bluramt",amt);
    }


     virtual void operator()(){
       this->bind();
        update();
        texture->bind();
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

    //Renderer * renderer;

    R2T(int w, int h, Renderer * r = NULL) : Process(w,h,r) { init(); }
    
    virtual void init(){
      cout << "INITIALIZING RENDER TO TEXTURE: " << width << " " << height << endl; 
      //initialize texture
      textureA = new Texture( width, height );
      textureB = new Texture( width, height );

      // Attach texture to FrameBuffer's ColorBuffer  
      fbo.attach(*textureA, GL::COLOR);          
    }

    void operator()(){
      fbo.attach(*textureA, GL::COLOR);  
      fbo.bind();               
      
        glViewport(0, 0, width, height ); 
        glClearColor(0,0,0,1);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //do any preprocessing in the stack (i.e. motion blur)
        preProcess();
        //And add a new frame on top
        renderer->render(); 
        //do any postprocessing in the stack (i.e. gaussian blur)
        postProcess();
        
      fbo.unbind(); 
    }

    void swap() { Texture * tmp = textureA; textureA = textureB; textureB = tmp;  };
  };


    /*!
   *  A ALT RENDER TO TEXTURE Process renders some other process into a texture bound to the framebuffer colorbuffer
   *
   *  NO shader required for this step . . .

   */
  /* struct RP2T : public Process { */ 
    
  /*   FBO fbo;                                    ///< A Framebuffer */
  /*   Texture * textureA;                         ///< Texture into which to render */
  /*   Texture * textureB;                         ///< Secondary Texture for swapping buffers */

  /*   //Renderer * renderer; */
  /*   Process * process; */

  /*   RP2T(int w, int h, Process * p = NULL) : Process(w,h), process(p) { init(); } */
    
  /*   virtual void init(){ */
  /*     cout << "INITIALIZING RENDER TO TEXTURE: " << width << " " << height << endl; */ 
  /*     //initialize texture */
  /*     textureA = new Texture( width, height ); */
  /*     textureB = new Texture( width, height ); */

  /*     // Attach texture to FrameBuffer's ColorBuffer */  
  /*     fbo.attach(*textureA, GL::COLOR); */          
  /*   } */

  /*   void operator()(){ */
  /*     fbo.attach(*textureA, GL::COLOR); */  
  /*     fbo.bind(); */               
      
  /*       glViewport(0, 0, width, height ); */ 
  /*       glClearColor(0,0,0,1); */
  /*       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); */
        
  /*       //do any preprocessing in the stack (i.e. motion blur) */
  /*       preProcess(); */
  /*       //And add a new frame on top */
  /*       (*process)(); */ 
  /*       //do any postprocessing in the stack */
  /*       postProcess(); */
        
  /*     fbo.unbind(); */ 
  /*   } */

  /*   void swap() { Texture * tmp = textureA; textureA = textureB; textureB = tmp;  }; */
  /* }; */


  /*!
   *  MOTION TRACE Process combines a R2T and an Alpha Slab
   */
  struct MotionTrace : public Process {
      
      R2T r2t;
      Alpha trace;
      Slab  slab;
     // Blur blur;

      MotionTrace(int w, int h, Renderer * r = NULL) : 
      Process(w,h,r), r2t(w,h,r), trace(w,h), slab(w,h) { init(); } 
      
      virtual void init(){
        ///Add alpha as a PreProcess to the r2t  
        r2t.pre(trace);

        trace.initRect();

        trace.texture = r2t.textureB;
        slab.rect = trace.rect;        //share resources
        slab.texture = trace.texture;  //redirect texture

        trace.amt = .99;
      }

     // template<class REND>
      void operator()(){
        r2t();
        slab();
        r2t.swap();
        trace.texture = r2t.textureB;
      }
  };


} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
