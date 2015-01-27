
/*! @file
 *
 *  graphics processes (render to texture, etc)
 *
 * */
#ifndef  gfx_tprocess_INC
#define  gfx_tprocess_INC


//#include "gfx_pipe.h"
#include "gfx_glsl.h"
#include "gfx_vao.h"
#include "gfx_shader.h"
#include "gfx_texture.h"
#include "gfx_scene.h"
#include "gfx_control.h"


namespace gfx{

  using namespace gfx::GLSL;

  /*!
   *  A Rendering PROCESS is meant to throw together
   *  easy experiments in shader and rendering to textures, etc
   *  Usually contains a shader program and pre or post processes.
   */
  template<class APP>
  struct Process {      
      
      APP * app;

      /// Pass in width and height in pixels and optional parent
      Process(int w, int h, Process * r = NULL) : 
      bEnable(true), 
      width(w), height(h), 
      parent(r), 
      bES(false) {
        #ifdef GFX_USE_GLES
          bES = true;
        #endif  
      }

      virtual ~Process(){};

      Process& set(APP * a) { app = a; return *this; }

      Process * parent;                                           ///< Pointer to Parent Process
      bool bES;                                                   ///< Use GL or GLES
      int width, height;                                          ///< Pixels Width and Height
      bool bEnable;                                               ///< Enable this Process

      bool useES(){ return bES; }
     
      vector<Process*> mPre;                                      ///< List of Pre-Processing steps
      vector<Process*> mPost;                                     ///< List of Post-Processing steps

      void pre(Process& p) { mPre.push_back(&p); }                ///< Add a Pre-Processing step
      void post(Process& p) { mPost.push_back(&p); }              ///< Add a Post-Processing step

      void preProcess(){ for (auto& i : mPre ) (*i)(); }          ///< Execute Pre-Processing steps
      void postProcess(){ for (auto& i : mPost ) (*i)(); }        ///< Execute Post-Processing steps

      virtual void init() = 0;
      virtual void operator()() = 0;

  };

  
  /*-----------------------------------------------------------------------------
   *  BASIC RENDERER FOR OPENGLES2.0 (NO VERTEX ARRAY OBJECTS)
   *-----------------------------------------------------------------------------*/
  template<class APP>
  struct Renderer : public Process<APP> {

      Renderer(int w, int h) : Process<APP>(w,h) {}

      ShaderProgram * program;
      VertexAttributes vatt;

      ///Default initialization enables depth and blend
      virtual void init(){

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
          
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
        string V = Process<APP>::useES() ? DefaultVertES : DefaultVert;        ///< These basic shaders are defined in gfx_glsl.h 
        string F = Process<APP>::useES() ? DefaultFragES : DefaultFrag; 
              
        program = new ShaderProgram(V,F,0);                 ///< The 0 here means load directly from string, not from file                                                                   
        program->bind();                                                    

        
        bindAttributes();
      }

      //Bind all Attributes 
      void bindAttributes(){
        //shader id | name | size of vertex data container | offset into container of each parameter
        vatt.add(program->id(), "position", sizeof(Vertex), 0); 
        vatt.add(program->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
        vatt.add(program->id(), "normal", sizeof(Vertex), Vertex::on()); 
        vatt.add(program->id(), "texCoord", sizeof(Vertex), Vertex::ot()); 
      }

      void bindUniforms(XformMat& xf){
          program -> uniform("lightPosition", 2.0, 2.0, 2.0);  
          program -> uniform("projection",  xf.proj);
          program -> uniform("normalMatrix", xf.normal);  
          program -> uniform("modelView",  xf.modelView );
      }

      virtual void operator()(){
        auto& app = Process<APP>::app;
        app -> scene.updateMatrices();
        program -> bind();
          bindUniforms(app -> scene.xf);
          app -> onDraw();
        program -> unbind();
      }

  };


  /*!
   *  A SLAB billboards a texture to the screen
   *
   *  (note: the Slab does not need to know about parent process)
   */
  template<class APP>
  struct Slab : public Renderer<APP> {
    
    using THIS_ = Renderer<APP>;
       
    MBO * rect;
    Texture * texture;

    Slab(int w, int h) : Renderer<APP>(w,h) { init(); }

    virtual void init(){
      initShader();
      THIS_::bindAttributes();
    }

    virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
    }
    
    virtual void initTexture(){
      texture = new Texture( THIS_::width, THIS_::height );
    }

    virtual void initShader(){
      THIS_::program = new ShaderProgram( THIS_::useES() ? ClipSpaceVertES : ClipSpaceVert, 
                                          THIS_::useES() ? TFragES : TFrag, 0 );
    }

    virtual void operator()(){     
      THIS_::program->bind();
       texture -> bind();
          GFX::Render( *rect, THIS_::vatt );
       texture -> unbind();
      THIS_::program->unbind();
    }
  };


} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
