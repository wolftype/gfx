

/*! @file
 *
 *  graphics processes (render to texture, etc)
 *
 * */
#ifndef  gfx_nprocess_INC
#define  gfx_nprocess_INC


//#include "gfx_pipe.h"
#include "gfx_mbo.h"
#include "gfx_glsl.h"
#include "gfx_vao.h"
#include "gfx_vattrib.h"
#include "gfx_shader.h"
#include "gfx_texture.h"
#include "gfx_scene.h"
#include "gfx_control.h"


namespace gfx{

  using namespace gfx::GLSL;

  /*!
   *  A GFXRenderNode is meant to throw together
   *  easy experiments in shader and rendering to textures, etc
   *  Usually contains a shader program and pre or post processes.
   */
  struct GFXRenderNode {      
      
      /// Pass in width and height in pixels and optional parent (which could be an application)
      GFXRenderNode(int w=0, int h=0, GFXRenderNode * r = NULL) : 
      bEnable(true), 
      width(w), height(h), 
      mParent(r), 
      bES(false) {
        #ifdef GFX_USE_GLES
          bES = true;
        #endif  
      }

      virtual ~GFXRenderNode(){};

      GFXRenderNode * mParent;                                              ///< Pointer to Parent Process
      
      bool bES;                                                             ///< Use GL or GLES
      int width, height;                                                    ///< Pixels Width and Height
      bool bEnable;                                                         ///< Enable this Process

      //Setters
      bool useES(){ return bES; }
      void parent(GFXRenderNode * p) { mParent = p; }

      vector<GFXRenderNode*> mPre;                                          ///< List of Pre-Processing steps
      vector<GFXRenderNode*> mPost;                                         ///< List of Post-Processing steps

      void pre(GFXRenderNode& p) { mPre.push_back(&p); p.parent(this); }    ///< Add a Pre-Processing step
      void post(GFXRenderNode& p) { mPost.push_back(&p); p.parent(this); }  ///< Add a Post-Processing step

      void preProcess(){ for (auto& i : mPre ) (*i)(); }                    ///< Execute Pre-Processing steps
      void postProcess(){ for (auto& i : mPost ) (*i)(); }                  ///< Execute Post-Processing steps


      /// Subclasses Must implement init and () and onProcess();
      virtual void init(){};
      virtual void process(){}
      virtual void onRender(){};
      virtual void operator()(){
        preProcess();
          process();
        postProcess();
      };
  };


  /*-----------------------------------------------------------------------------
   *  BASIC RENDERER FOR OPENGLES2.0 (NO VERTEX ARRAY OBJECTS)
   *-----------------------------------------------------------------------------*/
  struct GFXRenderer : public GFXRenderNode {
    
      Scene * mScene;
      void scene(Scene * s) { mScene = s; }  
      Scene& scene() { return *mScene; }   

      GFXRenderer(int w, int h, GFXRenderNode * r = NULL) : GFXRenderNode(w,h,r) {}

      ShaderProgram * program;
      VertexAttributes vatt;

      ///Default initialization enables depth and blend
      virtual void init(){

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
          
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
        string V = useES() ? DefaultVertES : DefaultVert;        ///< These basic shaders are defined in gfx_glsl.h 
        string F = useES() ? DefaultFragES : DefaultFrag; 
              
        program = new ShaderProgram(V,F,0);                      ///< The 0 here means load directly from string, not from file                                                                   
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

      virtual void process(){
        program -> bind();
          scene().updateMatrices();
          bindUniforms( scene().xf);
          mParent -> onRender();
        program -> unbind();
      }

  };


  /*!
   *  A SLAB billboards a texture to the screen
   *
   *  (note: the Slab does not need to know about parent process)
   */
  struct Slab : public GFXRenderer {
           
    MBO * rect;
    Texture * texture;

    Slab(int w, int h) : GFXRenderer(w,h) { init(); }

    virtual void init(){
      initShader();
      program->bind();
      bindAttributes();
      initRect();
      initTexture();
      cout << width << " " << height << " of shader " << endl;
    }

    virtual void initRect(){
      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
    }
    
    virtual void initTexture(){
      texture = new Texture( width, height );
    }

    virtual void initShader(){
      program = new ShaderProgram( useES() ? ClipSpaceVertES : ClipSpaceVert, 
                                   useES() ? TFragES : TFrag, 0 );
    }

    virtual void process(){     
      program->bind();
       texture -> bind();
          GFX::Render( *rect, vatt );
       texture -> unbind();
      program->unbind();
    }
  };


} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
