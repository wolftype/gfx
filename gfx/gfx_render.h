/*
 * =====================================================================================
 *
 *       Filename:  gfx_render.h
 *
 *    Description:  rendering pipelines
 *
 *    GFXRenderNode     organizes the list of pre and post processes. GFXApp inherits from this class
 *    GFXSceneRenderer  specifically manages modelviewprojection matrix uniform updates
 *    GFXShaderNode        
 *
 *        Version:  1.0
 *        Created:  01/20/2015 16:07:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome  
 *
 * =====================================================================================
 */


#ifndef  gfx_render_INC
#define  gfx_render_INC

#include "gfx_mbo.h"
#include "gfx_glsl.h"
#include "gfx_vao.h"
#include "gfx_fbo.h"
#include "gfx_vattrib.h"
#include "gfx_shader.h"
#include "gfx_texture.h"
//#include "temp/gl_texture.hpp"
#include "gfx_scene.h"
#include "gfx_control.h"


namespace gfx{

  using namespace gfx::GLSL;

  struct GFXSceneRenderer;
  
  /*-----------------------------------------------------------------------------
   *  Programmable Pipeline
   *-----------------------------------------------------------------------------*/
  template<class T>
  struct Renderable {
    
    ///Look up by type
    static vector<MBO>& Get(){
      return MeshBuffer<T>::Get();
    }

    /// Look up by address
    static MBO& Get(const T& t){
      return MeshBuffer<T>::Get(t);
    }

    static void SetUpdate(){
      vector<MBO>& vm = Get();
      for (auto& m : vm ){
          m.bUpdate=true;
      }
    }

    static void UpdateColor(float r, float g, float b, float a){
      vector<MBO>& vm = Get();
      for (auto& m : vm ){
        if (m.shouldUpdate()){
          m.mesh.color(r,g,b,a);
          m.update();
          m.bUpdate=false;
        }
      }
    }

    static void Update(){
      vector<MBO>& vm = Get();
      for (auto& m : vm ){
          m.update();
      }
    }


    static Mat4f ModelMatrix(const T& t){
      printf(R"(no matrix model defined for this type)");
    }

    static void UpdatePosition(const T& t){}

    static void Draw(const T& t, GFXSceneRenderer * _i ){
      printf(R"(Renderable<T>::Draw routine not yet defined for this type: 
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
    }

    static void Draw(const T& t, const Mat4f& model, GFXSceneRenderer * _i) {
      printf(R"(Renderable<T>::Draw (with ModelMatrix) routine not yet defined for this type: 
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
    }
  };

  
  
  /*-----------------------------------------------------------------------------
   *  Fixed Pipeline for Some type T
   *
   *  you must define your own draw method for your type T:
   *
   *  template<> void Drawable<T>::Immediate(const T& t){ ... }  
   *
   *  or specialize the whole Drawable class itself for your own generic types:
   *
   *  template<class T>
   *  struct Drawable< YourType<T> >{
   *    static void Immediate(const YourType<T>& m){ ... }
   *  };
   *
   *  [notes: cannot just use methods to find best fit 
   *  (generalized template, partially generalized template, specialized)
   *  cannot even just use a namespace here
   *  though it is easier to overload free functions than methods, ADL doesn't work
   *  for generic types later on... ]
   *  *-----------------------------------------------------------------------------*/
  template<class T>
  struct Drawable {
    
    static void Draw(const T& t){
       printf("Drawable<T>::Immediate routine is not yet specialized\n");
    }

    template<class B>
    static void Draw(const B& b){
      Drawable<B>::Draw(b);
    }

  };

  template<> void Drawable<MBO>::Draw(const MBO& m){
#ifdef GFX_IMMEDIATE_MODE
    m.mesh.drawElements();
#endif
  }

  namespace render{
#ifdef GFX_IMMEDIATE_MODE
    void begin(float r=1.0,float g=1.0,float b=1.0,float a=1.0){
      glNormal3f(0,0,1);
      glColor4f(r,g,b,a);
    }

    void color(float r=1.0,float g=1.0, float b=1.0, float a=1.0){
      glColor4f(r,g,b,a);
    }

    template<class A, class B>
    void drawAt(const A& a, const B& p){
      glPushMatrix(); 
      glTranslatef( p[0], p[1], p[2] );
      Drawable<A>::Draw(a);
      glPopMatrix();
    }

    template<typename A>
    void draw(const A& a){
      glPushMatrix(); 
      Drawable<A>::Draw(a);
      glPopMatrix();      
    }

#else
   void begin(float r=1.0,float g=1.0, float b=1.0, float a=1.0){
     printf("a fixed functionality draw routine has been specified but OpenGL ES does not allow it\n");
   }
   void color(float r=1.0,float g=1.0, float b=1.0, float a=1.0){
     printf("a fixed functionality draw routine has been specified but OpenGL ES does not allow it\n");
   }
   template<class A, class B>
   void drawAt(const A& a, const B& p){
     printf("a fixed functionality draw routine has been specified but OpenGL ES does not allow it\n");
   }
   template<typename A>
   void draw(const A& a){
     printf("a fixed functionality draw routine has been specified but OpenGL ES does not allow it\n");
   }
#endif 

} //render::
  

  /*!
   *  A GFXRenderNode is the root node of all processes
   *  Usually contains a shader program and pre or post processes.
   */
  struct GFXRenderNode {      
      
      GFXRenderNode * mChild = NULL;                                        ///< Pointer to another process
     
      Vec3f light = Vec3f(1,1,4);                                           ///< Light Position  

      bool bImmediate=true;                                                 ///< Use Fixed Function or Programmable
      void immediate(bool b) { bImmediate = b; }
      bool immediate() const { return bImmediate; } 

      bool bES = false;                                                     ///< Use OpenGL ES or Plain Open GL
      int width = 100;
      int height = 100;                                                     ///< Pixels Width and Height
      bool bEnable = true;                                                  ///< Enable this Process
      
      void set(int w, int h) { width=w; height=h; }

      //Setters
      bool useES(){ return bES; }
      void child(GFXRenderNode * p) { mChild = p; }
 //     void parent(GFXRenderNode * c) { mParent(c); }


      /*-----------------------------------------------------------------------------
       *  Pre and Post Processing
       *-----------------------------------------------------------------------------*/
  //    vector<GFXRenderNode*> mPre;                                          ///< List of Pre-Processing steps
  //    vector<GFXRenderNode*> mPost;                                         ///< List of Post-Processing steps
     // vector<GFXRenderNode*> mOuter;                                        ///< List of Outer-Processing steps

   //   void pre(GFXRenderNode& p) { mPre.push_back(&p); p.parent(this); }    ///< Add a Pre-Processing step
   //   void post(GFXRenderNode& p) { mPost.push_back(&p); p.parent(this); }  ///< Add a Post-Processing step

  //    void preProcess(){ for (auto& i : mPre ) (*i)(); }                    ///< Execute Pre-Processing steps
  //    void postProcess(){ for (auto& i : mPost ) (*i)(); }                  ///< Execute Post-Processing steps
    //  void outerProcessEnter()

      /// Subclasses can implement their own init and process();
      virtual void init(){}


      virtual void enter(){}
      virtual void exit(){}
      virtual void update(){}
      
      /* virtual void process(){ */ 
      /*   onRender(); */ 
      /* } */
      
      virtual void onRender(){}
    
      /*-----------------------------------------------------------------------------
       *  Root Render Node Operator
       *-----------------------------------------------------------------------------*/
      virtual void operator()(){
#ifndef GFX_USE_GLES       
        if (bImmediate) GL::lightPos( light[0], light[1], light[2] );
#endif
        //if(mParent) mParent->enter();
          onRender();
          //process();
        //if(mParent) mParent->leave(); 
        //postProcess();
      }

      GFXRenderNode& operator << (GFXRenderNode * r){
          child(r);
          return *r;
      }
  };



/*-----------------------------------------------------------------------------
 *  Basic Shader Code.  Default Version 
 *-----------------------------------------------------------------------------*/
struct GFXShaderNode : GFXRenderNode {
   
   ShaderProgram * program;
   VertexAttributes vatt;

   virtual void init(){

        string V = useES() ? DefaultVertES : DefaultVert;        ///< These basic shaders are defined in gfx_glsl.h 
        string F = useES() ? DefaultFragES : DefaultFrag; 
              
        program = new ShaderProgram(V,F,0);                      ///< The 0 here means load directly from string, not from file                                                                   
        program->bind();                                                    
      
          bindAttributes();

        program->unbind();
    }


   //Default Attributes include position, sourceColor, normal, and texCoord
   virtual void bindAttributes(){
        //shader id | name | size of vertex data container | offset into container of each parameter
        vatt.add(program->id(), "position", sizeof(Vertex), 0); 
        vatt.add(program->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
        vatt.add(program->id(), "normal", sizeof(Vertex), Vertex::on()); 
        vatt.add(program->id(), "texCoord", sizeof(Vertex), Vertex::ot()); 
   }

   virtual void updateUniforms(){}

   virtual void enter(){
      program->bind();
      updateUniforms();
   }

   virtual void exit(){
     program->unbind();
   }

    virtual void onRender(){
        if (bImmediate) {
          mChild->onRender();
        } else {
          program -> bind();
            updateUniforms();
            mChild -> onRender();
          program -> unbind();
        }    
    }

};


  /*-----------------------------------------------------------------------------
   *  BASIC RENDERER FOR RENDERABLES IN A SCENE (works with OPENGLES2.0 so  VERTEX ARRAY OBJECTS)
   *-----------------------------------------------------------------------------*/
  struct GFXSceneRenderer : public GFXShaderNode {
    
      //GFXRenderer(int w, int h, GFXRenderNode * r = NULL) : GFXRenderNode(w,h,r) {}
        Scene * mScene;
        void scene(Scene * s) { mScene = s; }  
        Scene& scene() { return *mScene; }                        ///< Access matrix transforms

       virtual void updateUniforms(){
            program -> uniform("lightPosition", light[0], light[1], light[2] ); //2.0, 2.0, 2.0);  
            program -> uniform("projection",  scene().xf.proj);
            program -> uniform("normalMatrix", scene().xf.normal);  
            program -> uniform("modelView",  scene().xf.modelView );
       }

       void updateModelView(){
            static float mv[16];
            (mScene->mvm()).fill(mv);
            program -> uniform("modelView", mv);
       }

       void updateModelView( const Mat4f& mat ){ 
           static float mv[16];
           (mScene->mvm()*mat).fill(mv);
            program -> uniform("modelView", mv);   
      }

      template<class T>
      void update(const T& t){
        Renderable<T>::Update(t);
      }

      template<class T>
      void draw(const T& t, float r = 1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate=false){
         if (bImmediate) {
           render::begin(r,g,b,a);
           render::draw(t); 
         }
         else {
          if (bUpdate) {
            Renderable<T>::SetUpdate();
          }
          Renderable<T>::UpdateColor(r,g,b,a); ///
          Renderable<T>::Draw(t,this);  ///< pass "this" to Renderable<T>::Draw in order to access current model view and vertex attributes
         }        
      }

      void draw(MBO& m, float r=1.0, float g=1.0, float b=1.0, float a=1.0){
        if (bImmediate){
           render::begin(r,g,b,a);
           render::draw(m); 
        }else {
            updateModelView( ); ///< identity matrix
            m.render(vatt); 
        }
      }

      //untested, draw a bunch of things
      /* template<class T> */
      /* void draw(T * ptr, int num, float r=1.0,float g=1.0,float b=1.0,float a=1.0){ */
      /*   MBO& m = Renderable<T>::Get()[0]; //or more ... */
      /*   m.bind(vatt); */
      /*   for (int i=0;i<num;++i){ */
      /*     updateModelView( Renderable<T>::ModelMatrix( ptr[num] ) ); */
      /*     m.drawElements(); */
      /*   } */
      /*   m.unbind(vatt); */
      /* } */

      /* template<class T, class B> */
      /* void drawAt(const T& t, const B& p, float r = 1.0, float g=1.0, float b=1.0, float a=1.0){ */
      /*    if (bImmediate) { */
      /*     render::begin(r,g,b,a); */ 
      /*     render::drawAt(t,p); */
      /*    } */
      /*    /1* else { *1/ */
      /*    /1*  Renderable<T>::UpdateColor(t,r,g,b,a); ///< currently updates mesh twice -- (once here and once in Draw routine if positions are modified) *1/ */
      /*    /1*  Renderable<T>::Draw(t,this);  ///< pass "this" to Renderable<T>::Draw in order to access current model view and vertex attributes *1/ */
      /*    /1* } *1/ */        
      /* } */

  };


  template<>
  void Renderable<MBO> :: Draw(const MBO& m, GFXSceneRenderer * _i){   
      // shader is already bound at this point
      _i->updateModelView(); ///< identity matrix
      m.render(_i->vatt);
  }

  template<>
  void Renderable<MBO> :: Draw(const MBO& m, const Mat4f& model, GFXSceneRenderer * _i){
      _i->updateModelView(model); ///< modelview * submodel matrix
      m.render(_i->vatt);
  }

  /* template<> */
  /* void Renderable<MBO> :: UpdateColor(const MBO& m, float r, float g, float b, float a){ */
  /*       /1* if (m.shouldUpdate()){ *1/ */
  /*       /1*   m.mesh.color(r,g,b,a); *1/ */
  /*       /1*   m.update(); *1/ */
  /*       /1* } *1/ */
  /* } */



  /*!
   *  A SLAB billboards a texture to the screen
   *
   */
  struct Slab : public GFXShaderNode {
           
    MBO * rect;
    Texture * texture;

   // int width, height;
   // Slab(int w=100, int h=100) : width(w), height(h) {}

    virtual void init(){
      program = new ShaderProgram( useES() ? ClipSpaceVertES : ClipSpaceVert, 
                                   useES() ? TFragES : TFrag, 0 );

      program->bind();

      bindAttributes();

      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
      texture = new Texture( width, height );
    }

    virtual void onRender(){// process(){     
      program->bind();
       texture -> bind();
          rect ->render( vatt );
       texture -> unbind();
      program->unbind();
    }

  };


  /*!
   *  BLUR Process takes a slab and blurs it in the fragment shader
   */
  struct Blur : public GFXShaderNode {

      MBO * rect;
      Texture * texture;

      float ux=.1;
      float uy=.1;
      float amt=1;

    //  int width, height;
    //  Blur(int w=100, int h=100) : width(w), height(h) {}

     
      virtual void init(){
      
        program = new ShaderProgram( bES ? ClipSpaceVertES : ClipSpaceVert, 
                                   bES ? TFragBlurES : TFragBlur, 0);
        program->bind();

        bindAttributes();
      
        rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
        texture = new Texture( width, height );

     }


    virtual void updateUniforms(){
         this->program->uniform("ux",ux);
         this->program->uniform("uy",uy);
         this->program->uniform("bluramt",amt);
    }


     virtual void onRender(){//process(){
       program->bind();
        updateUniforms();
        texture->bind();
            //don't repeat edge pixels
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

           rect ->render( vatt );
        texture -> unbind();
       program->unbind(); 
     }

  };

  /*!
   *  A RENDER TO TEXTURE Process renders into a texture bound to the framebuffer colorbuffer
   *
   *  NO shader required for this step . . .

   */
  struct RenderToTexture : public GFXRenderNode { 
    
    FBO fbo;                                    ///< A Framebuffer
    Texture * textureA;                         ///< Texture in which to render
    Texture * textureB;                         ///< Secondary Texture for swapping buffers

    
    virtual void init(){
      //initialize texture
      textureA = new Texture( width, height );
      textureB = new Texture( width, height );

      // Attach texture to FrameBuffer's ColorBuffer  
      fbo.attach(*textureA, GL::COLOR);          
    }


    virtual void enter(){
      fbo.attach(*textureA, GL::COLOR);  
      fbo.bind();               
     
       // glColorMask(GL_TRUE,GL_TRUE,GL_FALSE,GL_TRUE); //test
        glViewport(0, 0, width, height ); 
        glClearColor(0,0,0,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
    }      
        
     virtual void exit(){
      fbo.unbind(); 
     }

     virtual void onRender(){//process(){
       enter();
        mChild->onRender();
       exit();
     }

    void swap() { Texture * tmp = textureA; textureA = textureB; textureB = tmp;  };

  };

} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
