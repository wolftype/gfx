/*
 * =====================================================================================
 *
 *       Filename:  gfx_render.h
 *
 *    Description:  rendering pipelines
 *                  a rendering graph that makes complicated effects like motion trace easier.
 *                  provides boolean switchability between fixed function and programmable OpenGL
 *
 *    based on work with Graham Wakefield for the AlloSphere Research Group during our NanoMed project
 *
 *
 *    GFXRenderNode the root node, calls all upstream processes, overloads << operator. 
 *    GFXViewNode   sets the glViewport.  q: how to instantiate multiple with multiple mvp matrices.
 *    GFXShaderNode binds shader and calls upstream nodes
 *    GFXSceneNode  points to a scene (matrix manager) and sets modelviewprojection matrix uniform updates, 
 *                  by updating downstream shader
 *                  GFXApp currently inherits from this class, but need not: just bind with a sceneController object...
 *    GFXMeshNode   points to a meshbuffer object and a vertex attribute list, updates downstream shader model transform
 *                  to do this, the GFXMeshNode::shader() method finds first downstream shader.
 *    to do: 
 *    GFXTextureNode points to a texture which it wraps around upstream input (a meshnode). downstream is a SceneNode or ShaderNode.
 *    GFXEffectsNode points a texture to a framebuffer color buffer, binds an MBO rect (slab) and executes a fragment shader on it 
 *                    Q: render to screen to to another framebuffer? 
 *    GFXStereoNode  take upstream input (a SceneNode) and splits it into left and right eyes
 *    GFXOmniNode    take upstream input (a SceneNode) renders to left and right cubemaps
 *
 *    fix initialization so that downstream node calls init(), only upstream nodes that need to be reinitialized are
 *    test destruction of shader programs
 *
 *
 *    
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
#include "gfx_scene.h"
#include "gfx_control.h"
#include "gfx_monitorLayout.h"

namespace gfx{

  using namespace gfx::GLSL;

  struct GFXSceneNode;
  
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

    static void Draw(const T& t, GFXSceneNode * _i ){
      printf(R"(Renderable<T>::Draw routine not yet defined for this type: 
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
    }

    static void Draw(const T& t, const Mat4f& model, GFXSceneNode * _i) {
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
 
 
 enum {
    GFX_RENDER_NODE,
    GFX_VIEW_NODE,
    GFX_SHADER_NODE,
    GFX_SCENE_NODE,
    GFX_MESH_NODE,
    GFX_FRAMEBUFFER_NODE,
    GFX_EFFECT_NODE
 };


  /*!
   *  A GFXRenderNode is the root node of all processes
   *
   *  Has a pointer to a "mDownstream" rendernode 
   *  and multiple pointers to mUpstream rendernodes
   *
   *  The virtual method, onRender(), specifies how
   *  the node should bind or onbind its upstream and downstream
   *  graphics processing events
   *
   *  It can also register for WindowEvents (such as onResize)
   */
  struct GFXRenderNode : public WindowEventHandler {      
     
      virtual const int nodetype() { return GFX_RENDER_NODE; }
     
      bool bVisited=false;                                                  ///< You can avoid circular dependencies by setting this flag

      GFXRenderNode * mDownstream = NULL;                                   ///< Pointer to one downstream process (for re-setting state)
      
      GFXRenderNode& downstream( GFXRenderNode * p ){
        mDownstream = p;
        return *this;
      }

      /// Finds Base Root of RenderGraph
      GFXRenderNode& root(){
        GFXRenderNode * p = this;
        GFXRenderNode * tmp = mDownstream;
        while(tmp!=NULL) {
          p = tmp;
          tmp = tmp->mDownstream;
        }   
        return *p;  
      }

      GFXRenderNode& downstream(){
        return *mDownstream;
      }

      /// point argument to this instance's downstream process
      GFXRenderNode& bindDownstream(GFXRenderNode& r){
        r.downstream(mDownstream);
        return *this;
      }

      /// add all of this instance's upstream nodes to argument
      GFXRenderNode& bindUpstream(GFXRenderNode& r){
        for(auto& i : mUpstream) r << i;
        return *this;
      }

      vector<GFXRenderNode*> mUpstream;                                     ///< Pointers to multiple upstream processes (to be pulled)
      void clear() { mUpstream.clear(); }
     
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

      /// set width and height, call onInit, and recurse on all upstream nodes
      void init(int w=100, int h=100){
        set(w,h);
        onInit();
        for (auto& i : mUpstream) i->init(w,h);
      }

      /// Subclasses can implement their own init and process();
      virtual void onInit(){
        
      }
      
      virtual void enter(){}
      virtual void exit(){}
      virtual void update(){}
      virtual void onRender(){
        for (auto& i : mUpstream) i->onRender();  
      }

      GFXRenderNode& operator << (GFXRenderNode * r){
          mUpstream.push_back(r);
          r->downstream(this);
          return *r;
      }

      GFXRenderNode& operator << (GFXRenderNode& r){
          mUpstream.push_back(&r);
          r.downstream(this);
          return r;
      }

      void resize(int w, int h){
        onResize(w,h);  
        for(auto& i : mUpstream) i->resize(w,h);
      }

      virtual void onResize(int w, int h){
        set(w,h);
      }
  };



/*-----------------------------------------------------------------------------
 *  Render into a viewport (specified as percentage of full screen)
 *-----------------------------------------------------------------------------*/
struct GFXViewNode : GFXRenderNode {
    
     
      virtual const int nodetype() { return GFX_VIEW_NODE; }

    struct View{
       Vec4f view = Vec4f(0,0,1,1);
       Vec4f color = Vec4f(.05,.05,.05,1);
       View(){}
       View(const Vec4f& v) : view(v) {}
       View(float l, float b, float r, float t, float red=.05,float green=.05,float blue=.05) : 
       view(l,b,r,t),color(red,green,blue,1) {}
    };

    View * view;

    virtual void onInit(){
      //view.push_back( new View(0,0,.5,.5) );
      view = new View(0,0,1,1);
    }

//    void add(float l, float b, float r, float t) { view.push_back( new View(l,b,r,t)); }

    /* void split(int w,int h){ */
    /*   view.clear(); */
    /*   for (int i=0;i<w;++i){ */
    /*     for (int j=0;j<h;++j){ */
    /*       float l = (float)i/w; */
    /*       float b = (float)j/h; */
    /*       view.push_back( new View(l,b,1.0/w,1.0/h) ); */
    /*     } */
    /*   } */
    /* } */

    void onRender(){
      //for (auto& k : view){ //<-- for all views
        Vec4f& v = view->view;
        Vec4f& c = view->color;
        glViewport((v.x*width), (v.y*height),(v.z*width),(v.w*height));
        glScissor((v.x*width), (v.y*height),(v.z*width),(v.w*height));
        glEnable(GL_SCISSOR_TEST); 
        glClearColor(c[0],c[1],c[2],c[3]);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);      
          
          for (auto& i : mUpstream) i->onRender(); //<-- render all upstream nodes
        
        glDisable(GL_SCISSOR_TEST);
     // }
    }

    virtual void onResize(int w, int h){
      set(w,h);
    }
};



/*-----------------------------------------------------------------------------
 *  Basic Shader Code.  Default Version 
 *-----------------------------------------------------------------------------*/
struct GFXShaderNode : GFXRenderNode {
   
  virtual const int nodetype() { return GFX_SHADER_NODE; }

   ShaderProgram * program;
   VertexAttributes vatt;


   virtual void onInit(){

        string V = useES() ? DefaultVertES : DefaultVert;        ///< These basic shaders are defined in gfx_glsl.h 
        string F = useES() ? DefaultFragES : DefaultFrag; 
              
        program = new ShaderProgram(V,F);                                                              
        bindAttributes();
    }


   //Default Attributes include position, sourceColor, normal, and texCoord (AUTOMATE THIS?)
   virtual void bindAttributes(){
        //shader id | name | size of vertex data container | offset into container of each parameter
        program->bind();
          vatt.add(program->id(), "position", sizeof(Vertex), 0); 
          vatt.add(program->id(), "sourceColor", sizeof(Vertex), Vertex::oc() ); 
          vatt.add(program->id(), "normal", sizeof(Vertex), Vertex::on()); 
          vatt.add(program->id(), "texCoord", sizeof(Vertex), Vertex::ot());
        program->unbind(); 
   }

   virtual void enter(){
     if (!immediate()) program->bind();
   }
   virtual void exit(){
     if (!immediate()) program->unbind();
   }

   virtual void onRender(){
        if ( immediate() ) {
          for (auto& i : mUpstream){ i->onRender(); }
        } else {
          program -> bind();
         //   updateUniforms(); // note, this may be unnecessary: upstream nodes will reach downstream here and update uniforms
            for (auto& i : mUpstream){ 
              i->update(); // upstream nodes update this downstream node's values (see e.g. GFXSceneNode) 
              i->onRender(); 
            }
          program -> unbind();
        }    
    }

};

  
/*-----------------------------------------------------------------------------
 *  Scene Node To Update Downstream Shader Matrix Uniforms and Call Upstream Mesh Nodes
 *-----------------------------------------------------------------------------*/
  struct GFXSceneNode : GFXRenderNode {
       
  virtual const int nodetype() { return GFX_SCENE_NODE; }
 
       Scene * mScenePtr;                                               ///< pointer to scene matrix transforms

       void scene(Scene*s) { mScenePtr=s; }

       virtual void onResize(int w, int h){
         mScenePtr -> resize(w,h);
       }

       GFXShaderNode& shader(){ return *(GFXShaderNode*)mDownstream; }  ///< reference to shader downstream (to access uniforms and vertex attributes)

      /// Bind model view projection and normal matrices to downstream shader (and light position...)
      /// (uniform_if means do so only if such uniforms exist)
       virtual void update(){
            ShaderProgram& dp = *shader().program;

            dp.uniform_if("lightPosition", light[0], light[1], light[2] ); //2.0, 2.0, 2.0);  
            dp.uniform_if("projection",  mScenePtr->xf.proj);
            dp.uniform_if("normalMatrix", mScenePtr->xf.normal);  
            dp.uniform_if("modelView",  mScenePtr->xf.modelView );
       }

       void updateModelView(){
            static float mv[16];
            (mScenePtr->mvm()).fill(mv);
            ShaderProgram& dp = *shader().program;
            dp.uniform_if("modelView", mv);
       }

      /// Multiply modelview by some object-specific matrix
       void updateModelView( const Mat4f& mat ){ 
           static float mv[16];
           (mScenePtr->mvm()*mat).fill(mv);
           ShaderProgram& dp = *shader().program;
           dp.uniform_if("modelView", mv);   
      }

      void draw(MBO& m, float r=1.0, float g=1.0, float b=1.0, float a=1.0){
        if ( downstream().immediate() ){
           render::begin(r,g,b,a);
           render::draw(m); 
        }else {
           updateModelView(); ///< identity matrix
           m.render(shader().vatt); 
        }
      }

      // specialize your draw routines by defining a Renderable<T>::Draw(const T& t, GFXSceneNode * r) method
      // 
      template<class T>
      void draw(const T& t, float r = 1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate=false){
         if ( downstream().immediate()) {
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

      //draw many (specialize this by defining a Renderable<vector<T>>::Draw method
      //where you bind mbo and vertex attributes once, then drawElements() many times)
      template<class T>
      void draw(const vector<T>& t, float r=1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate =false){
          if (downstream().immediate()){
            render::begin(r,g,b,a);
            for (auto& i : t) render::draw(i);
          } else {
            if (bUpdate) {
              Renderable<T>::SetUpdate();
            }
            Renderable<T>::UpdateColor(r,g,b,a);
            //for (auto& i : t) Renderable<T>::Draw(i,this);
            Renderable<vector<T>>::Draw(t, this);
          }
        }

        virtual void onRender(){
          mScenePtr->push( shader().immediate() );
          for (auto& i : mUpstream) i->onRender();
          mScenePtr->pop( shader().immediate() );
        }

  };




  template<>
  void Renderable<MBO> :: Draw(const MBO& m, GFXSceneNode * _i){   
      // shader is already bound at this point
      _i->updateModelView(); ///< identity matrix
       m.render(_i->shader().vatt);
  }

  template<>
  void Renderable<MBO> :: Draw(const MBO& m, const Mat4f& model, GFXSceneNode * _i){
      _i->updateModelView(model); ///< modelview * submodel matrix
      //GFXShaderNode& sn = *(GFXShaderNode*)_i->mDownstream;
       m.render(_i->shader().vatt);
  }

  /* template<> */
  /* void Renderable<MBO> :: UpdateColor(const MBO& m, float r, float g, float b, float a){ */
  /*       /1* if (m.shouldUpdate()){ *1/ */
  /*       /1*   m.mesh.color(r,g,b,a); *1/ */
  /*       /1*   m.update(); *1/ */
  /*       /1* } *1/ */
  /* } */

 struct GFXMeshNode : public GFXRenderNode {
 
  virtual const int nodetype() { return GFX_MESH_NODE; }

    //Vec4f mColor = Vec4f(1,1,1,1);    
    vector<MBO*> mMbo;
    
    GFXMeshNode& add( MBO* m) { mMbo.push_back(m); return *this; }
    //void mbo(MBO* m) { mMbo=m; } 
    //GFXMeshNode& add( MBO * m ) { mbo.push_back(m); return *this; }

    GFXSceneNode& scene(){ return *(GFXSceneNode*)mDownstream; }                      ///< reference to scene downstream (to access shader)
    GFXShaderNode& shader() { 
     
      auto * tmp = mDownstream; 
      //find first downstream shader
      while (tmp && tmp->nodetype()!=GFX_SHADER_NODE){
        tmp = tmp->mDownstream;
      }
      
      if (!tmp) printf("error no shader downstream\n");
      return *(GFXShaderNode*)tmp;
    }

    virtual void onRender(){
        if ( shader().immediate() ){
      //     render::begin(mColor[0],mColor[1],mColor[2],mColor[3]);
           for (auto& m : mMbo) render::draw(*m); 
        }else {
           scene().updateModelView(); ///< identity matrix
           for (auto& m : mMbo) m->render(shader().vatt); 
        }
    }
 };

template<class T>
struct GFXMeshNodeT : GFXMeshNode {

  TMBO<T>* mbo;
  VertexAttributes * vatt;

  //must be bound to a shader downstream already
  void onInit(){
    
    vatt = new VertexAttributes();

    for( auto& i : shader().program->attributes() ){
      vatt->add(shader().program->id(), i.first, sizeof(T), offset( i.first ) );
    }
  
  }

  GLvoid * offset(string s){
    if (s == "normal") return T::on();
    if (s == "sourceColor") return T::oc();
    if (s == "texCoord") return T::ot();
    return 0;
  }

   virtual void onRender(){
        if ( shader().immediate() ){
          // render::begin(mColor[0],mColor[1],mColor[2],mColor[3]);
           render::draw(*mbo); 
        } else {
           //scene().updateModelView(); ///< identity matrix
           mbo->render(*vatt); 
        }
    }
  
};


  /*!
   *  A SLAB billboards a texture to the screen (with optional alpha)
   *
   */
  struct Slab : public GFXShaderNode {
  

    MBO * rect;
    Texture * texture;
    float amt=1.0;

    virtual void onInit(){
      program = new ShaderProgram( useES() ? ClipSpaceVertES : ClipSpaceVert, 
                                   useES() ? TFragAlphaES : TFragAlpha, 0);
      bindAttributes();

      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
      texture = new Texture( width, height );
    }

    virtual void onRender(){
      
      if (!bVisited) {
        bVisited=true;
        for (auto& i : mUpstream){ i->onRender(); }
      }

     //glViewport(0,0,width,height);
      if (mDownstream) glViewport(0,0,mDownstream->width, mDownstream->height);

      program->bind();
       program->uniform("alpha",amt);
       texture -> bind();
          rect ->render( vatt );
       texture -> unbind();
      program->unbind();

      bVisited=false;
    }

  };


  /*!
   *  BLUR Process takes a slab and blurs it in the fragment shader
   *  then displays it
   *  (perhaps it should only make the effect on the texture, not render it,
   *  rendering could be handled downstream)
   */
  struct Blur : public GFXShaderNode {


      MBO * rect;
      Texture * texture;

      float ux=.1;
      float uy=.1;
      float amt=1;
    
      virtual void onInit(){
      
        program = new ShaderProgram( bES ? ClipSpaceVertES : ClipSpaceVert, 
                                   bES ? TFragBlurES : TFragBlur, 0);
        bindAttributes();
      
        rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) ); 
        texture = new Texture( width, height );

     }

    virtual void updateUniforms(){
         this->program->uniform("ux",ux);
         this->program->uniform("uy",uy);
         this->program->uniform("bluramt",amt);
    }

     virtual void onRender(){
       
     if (!mUpstream.empty() && !bVisited) {
        bVisited=true;
        for (auto& i : mUpstream){ i->onRender(); }
      }

      if (mDownstream) glViewport(0,0,mDownstream->width, mDownstream->height);

        program->bind();
        updateUniforms();
        texture->bind();
        
            //don't repeat edge pixels
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

           rect ->render( vatt );
        texture -> unbind();
       program->unbind();
        
       bVisited=false;
 
     }

  };

  /*!
   *  A RENDER TO TEXTURE node renders into a texture bound to the
   *   framebuffer colorbuffer and draws it to the screen
   *
   *   use witin an app:
   *
   *   RenderToTexture r2t; //<-- captures to framebuffer object
   *   Slab slab;           //<-- renders to screen (rename Screen?)
   *
   *   mRenderer.clear()    //<-- empty upstream render
   *   mRenderer << slab << r2t << this; 
   *
   */
struct RenderToTexture : GFXRenderNode {

  virtual const int nodetype() { return GFX_FRAMEBUFFER_NODE; }

   FBO fbo;
     
   Texture * texture;
   Texture * textureB;

   void onInit(){
     fbo.set(width,height);
     fbo.init();

     texture = new Texture(width,height);
     textureB = new Texture(width,height);

     fbo.attach(*texture);
   }
   
   void swap() { 
      Texture * tmp = texture; texture = textureB; textureB = tmp;  
      fbo.attach(*texture);
    };


   void onRender(){
           
      fbo.bind();
      fbo.clear();

        for (auto& i : mUpstream) { i->onRender(); }  

      fbo.unbind();
      swap();

      //do not reset glViewport here --> this will be set by downstream slab or effect
      //glViewport(0,0,root().width, root().height); //the downstream slab will reset this . . .
    }
 
};

/*!
 *  MotionBlur renders to a texture twice, then
 *  renders an outputmix of the two
 *
 *  use within an app
 *
 *  mRenderer << motionBlur << this;
 */
struct MotionBlur : GFXRenderNode {

  virtual const int nodetype() { return GFX_EFFECT_NODE; }

    //ra pulls from upstream
    RenderToTexture ra,rb;
    Slab lastFrame;
    Slab outputMix;

    void onInit(){

      ra.init(width,height);
      rb.init(width,height);
      

      lastFrame.init(width,height);
      outputMix.init(width,height); 
      
      rb.fbo.depth(false);
      outputMix.amt=.995;

      //confusing for now because of difference between
      //binding into a node process and binding a texture result
      //could overload so that slabs "know" they are being bown to
      //a framebuffer capture and automatically point to their texture
    
      //bind capture call to upstream render calls
      bindUpstream(ra);

      //bind lastFrame to texture output result of capture call
      lastFrame.texture = ra.texture;

      //bind outputMix to texture B of local capture
      outputMix.texture = rb.textureB;

      //bind outputMix and lastFrame into local capture
      rb << outputMix;
      rb << lastFrame;

      //bind both outer and inner capture calls to outputMix
      outputMix << ra;
      outputMix << rb;

      //fixme: why do slabs need to know what is downstream? because of root().immediate check ...
      //for now disabled but needs a fix for immediate mode to work without mucking stuff up

 //     bindDownstream(mix);
 //     bindDownstream(slab); 
 //     bindDownstream(rb);


    }

    void onRender(){

      //outputMix onRender() pulls process down from upstream and displays results
      outputMix.texture = rb.textureB;
      outputMix.onRender();

    }

};


struct SceneGraph {

    Layout layout;    
    Scene mScene;

    GFXRenderNode mRenderNode;
    GFXViewNode mViewNode;
    GFXShaderNode mShaderNode;
    GFXSceneNode mSceneNode;
    GFXMeshNode mMeshNode;

    void init(int w, int h){
      mSceneNode.scene(&mScene);
      mShaderNode.immediate(false);
      mRenderNode << mViewNode << mShaderNode << mSceneNode << mMeshNode;
      mRenderNode.init(w,h);
    }

    void onRender(){
      mScene.push(false);
        mRenderNode.onRender();
      mScene.pop(false);
    }

    //set the frustrum viewport
    void setView(float z, bool isGrid, int row=0, int col=0){
            
      float w = layout.screenWidth;
      float h = layout.screenHeight;   

      //Single Screen Version:
      //bottom left corner of screen in pixels
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


      mScene.viewpose = p;
      mScene.camera.pos() = Vec3f( 0, 0, z); 
      mScene.camera.view = gfx::View( mScene.camera.pos(), p, (float)w/h, h );

    }                                    

};

} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */
