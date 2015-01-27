/*
 * =====================================================================================
 *
 *       Filename:  gfx_render.h
 *
 *    Description:  rendering pipelines
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
#include "gfx_vattrib.h"
#include "gfx_shader.h"
#include "gfx_texture.h"
#include "gfx_scene.h"
#include "gfx_control.h"


namespace gfx{

  using namespace gfx::GLSL;

  struct GFXRenderer;
  
  /*-----------------------------------------------------------------------------
   *  Programmable Pipeline
   *-----------------------------------------------------------------------------*/
  template<class T>
  struct Renderable {
    
    static vector<MBO>& Get(){
      return MeshBuffer<T>::Get();
    }

    static MBO& Get(const T& t){
      return MeshBuffer<T>::Get(t);
    }

    static void UpdateColor(const T& t, float r, float g, float b, float a){
      vector<MBO>& vm = Get();
      for (auto& m : vm ){
        if (m.shouldUpdate()){
          m.mesh.color(r,g,b,a);
          m.update();
        }
      }
    }

    static Mat4f ModelMatrix(const T& t){
      printf(R"(no matrix model defined for this type)");
    }

    static void UpdatePosition(const T& t){}

    static void Draw(const T& t, GFXRenderer * _i ){
      printf(R"(Renderable<T>::Draw routine not yet defined for this type: 
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
    }

    static void Draw(const T& t, const Mat4f& model, GFXRenderer * _i) {
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
    
    static void Immediate(const T& t){
       printf("Drawable<T>::Immediate routine is not yet specialized\n");
    }

    template<class B>
    static void Immediate(const B& b){
      Drawable<B>::Immediate(b);
    }

  };

  template<> void Drawable<MBO>::Immediate(const MBO& m){
    m.mesh.drawElements();
  }

  namespace render{

    void begin(float r=1.0,float g=1.0,float b=1.0,float a=1.0){
      glNormal3f(0,0,1);
      glColor4f(r,g,b,a);
    }

    template<class A, class B>
    void drawAt(const A& a, const B& p){
      glPushMatrix(); 
      glTranslatef( p[0], p[1], p[2] );
      Drawable<A>::Immediate(a);
      glPopMatrix();
    }

    template<typename A>
    void draw(const A& a){
      glPushMatrix(); 
      Drawable<A>::Immediate(a);
      glPopMatrix();      
    }
  }
    
  /*   template<class T> */
  /*   void immediate(const T& t){ */
  /*     printf("Immediate Mode Draw routine not defined: you must declare a gfx::render::immediate(const YourClass&) function \n"); */
  /*   } */

  /* }; */


  /*!
   *  A GFXRenderNode is the root node of all processes
   *  is the basis for 
   *  experiments in shader and rendering to textures, etc
   *  Usually contains a shader program and pre or post processes.
   */
  struct GFXRenderNode {      
      
      /// Pass in width and height in pixels and optional parent (which could be an application)
      GFXRenderNode(int w=0, int h=0, GFXRenderNode * r = NULL) : 
      bEnable(true), 
      bImmediate(true),
      width(w), height(h), 
      mParent(r), 
      bES(false) {
        #ifdef GFX_USE_GLES
          bES = true;
        #endif  
      }

      virtual ~GFXRenderNode(){};

      GFXRenderNode * mParent;                                              ///< Pointer to Parent Process
      
      Vec3f light;                                                          ///< Light Position  

      bool bImmediate;                                                      ///< Use Fixed Function or Programmable
      void immediate(bool b) { bImmediate = b; }
      bool immediate() const { return bImmediate; } 

      bool bES;                                                             ///< Use OpenGL ES or Plain Open GL
      int width, height;                                                    ///< Pixels Width and Height
      bool bEnable;                                                         ///< Enable this Process

      //Setters
      bool useES(){ return bES; }
      void parent(GFXRenderNode * p) { mParent = p; }



      /*-----------------------------------------------------------------------------
       *  Pre and Post Processing
       *-----------------------------------------------------------------------------*/
      vector<GFXRenderNode*> mPre;                                          ///< List of Pre-Processing steps
      vector<GFXRenderNode*> mPost;                                         ///< List of Post-Processing steps

      void pre(GFXRenderNode& p) { mPre.push_back(&p); p.parent(this); }    ///< Add a Pre-Processing step
      void post(GFXRenderNode& p) { mPost.push_back(&p); p.parent(this); }  ///< Add a Post-Processing step

      void preProcess(){ for (auto& i : mPre ) (*i)(); }                    ///< Execute Pre-Processing steps
      void postProcess(){ for (auto& i : mPost ) (*i)(); }                  ///< Execute Post-Processing steps


      /// Subclasses can implement their own init and process();
      virtual void init(){}
      
      virtual void process(){ 
        onRender(); 
      }
      
      virtual void onRender(){}
    
      /*-----------------------------------------------------------------------------
       *  Root Render Node Operator
       *-----------------------------------------------------------------------------*/
      void operator()(){
        if (bImmediate) GL::lightPos( light[0], light[1], light[2] );
        preProcess();
          process();
        postProcess();
      }
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

        program->unbind();
      }


      //Bind all Attributes 
      void bindAttributes(){
        //shader id | name | size of vertex data container | offset into container of each parameter
        vatt.add(program->id(), "position", sizeof(Vertex), 0); 
        vatt.add(program->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
        vatt.add(program->id(), "normal", sizeof(Vertex), Vertex::on()); 
        vatt.add(program->id(), "texCoord", sizeof(Vertex), Vertex::ot()); 
      }

      void bindUniforms(){
          program -> uniform("lightPosition", light[0], light[1], light[2] ); //2.0, 2.0, 2.0);  
          program -> uniform("projection",  scene().xf.proj);
          program -> uniform("normalMatrix", scene().xf.normal);  
          program -> uniform("modelView",  scene().xf.modelView );
      }

      void bindModelView(){
          program -> uniform("modelView",  scene().xf.modelView );
      }

     void bindModelView( const Mat4f& mat ){ 
        static float mv[16];
        (mScene->mvm()*mat).fill(mv);
        program -> uniform("modelView", mv );   
      }

      virtual void process(){
        if (bImmediate) {
          mParent->onRender();
        } else {
          program -> bind();
            bindUniforms();
            mParent -> onRender();
          program -> unbind();
        }
      }


      template<class T>
      void draw(const T& t, float r = 1.0, float g=1.0, float b=1.0, float a=1.0){
         if (bImmediate) {
           render::begin(r,g,b,a);
           render::draw(t); 
         }
         else {
          Renderable<T>::UpdateColor(t,r,g,b,a); ///< currently updates mesh twice -- (once here and once in Draw routine if positions are modified)
          Renderable<T>::Draw(t,this);  ///< pass "this" to Renderable<T>::Draw in order to access current model view and vertex attributes
         }        
      }

      void draw(MBO& m, float r=1.0, float g=1.0, float b=1.0, float a=1.0){
        if (bImmediate){
           render::begin(r,g,b,a);
           render::draw(m); 
        }else {
          if ( m.shouldUpdate() ){
            m.mesh.color(r,g,b,a);
            m.update();
          }
            bindModelView(); ///< identity matrix
            m.render(vatt); 
        }
      }


      template<class T, class B>
      void drawAt(const T& t, const B& p, float r = 1.0, float g=1.0, float b=1.0, float a=1.0){
         if (bImmediate) {
          render::begin(r,g,b,a); 
          render::drawAt(t,p);
         }
         /* else { */
         /*  Renderable<T>::UpdateColor(t,r,g,b,a); ///< currently updates mesh twice -- (once here and once in Draw routine if positions are modified) */
         /*  Renderable<T>::Draw(t,this);  ///< pass "this" to Renderable<T>::Draw in order to access current model view and vertex attributes */
         /* } */        
      }



      template<class T>
      void draw(T * ptr, int num, float r=1.0,float g=1.0,float b=1.0,float a=1.0){
        MBO& m = Renderable<T>::Get()[0]; //or more ...
        m.bind(vatt);
        for (int i=0;i<num;++i){
          bindModelView( Renderable<T>::ModelMatrix( ptr[num] ) );
          m.drawElements();
        }
        m.unbind(vatt);
      }
  };


  template<>
  void Renderable<MBO> :: Draw(const MBO& m, GFXRenderer * _i){   
    _i->bindModelView(); ///< identity matrix
    m.render(_i->vatt);
  }

  template<>
  void Renderable<MBO> :: Draw(const MBO& m, const Mat4f& model, GFXRenderer * _i){
    _i->bindModelView(model); ///< modelview * submodel matrix
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
          rect ->render( vatt );
       texture -> unbind();
      program->unbind();
    }
  };


} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
