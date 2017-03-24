/*!
 *
 *
 *       @FILENAME:  gfx_render.h
 *
 *    Description:  rendering pipelines
 *                  a rendering graph that aims to make complicated multipass effects like motion trace easier.
 *                  provides boolean switchability between fixed function and programmable OpenGL
 *
 *    loosely based on work with Graham Wakefield for the AlloSphere Research Group during our NanoMed project
 *
 *    GFXRenderGraph has a root render node, width and height, immediate/programmable, and mono/stereo.
                     GFXApp calls its rendergraph . onRender () method
 *  
 *    GFXRenderNode the root node, calls all upstream processes, overloads << operator.  GFXAPP is a rendernode.
      GFXStereoNode pulls into back left and back right buffers (active) or resets and color masks in between (anaglyph)
      GFXFrameBufferNode
 *    GFXViewNode   sets the glViewport.  q: how to instantiate multiple with multiple mvp matrices.
 *    GFXShaderNode binds shader and calls upstream nodes
 *    GFXSceneNode  points to a scene (matrix manager) and sets modelviewprojection matrix uniform updates,
 *                  by updating downstream shader
 *                  GFXApp currently inherits from this class, but need not: just bind with a sceneController object...
 *    GFXMeshNode   points to a meshbuffer object and a vertex attribute list, updates downstream shader model transform
 *                  to do this, the GFXMeshNode::shader() method finds first downstream shader.
 *    @todo:
 *    GFXTextureNode points to a texture which it wraps around upstream input (a meshnode). downstream is a SceneNode or ShaderNode.
 *    GFXEffectsNode points a texture to a framebuffer color buffer, binds an MBO rect (slab) and executes a fragment shader on it
 *                    Q: render to screen to to another framebuffer?
 *    GFXMaterialNode Combines Shader, Textures to pass a Mesh through
 *
 *    Fix initialization so that downstream node calls init()
 *    Only upstream nodes that need to be reinitialized are
 *    Test destruction of shader programs
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

#include "gfx_renderable.h"
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


struct GFXShaderNode;
struct GFXFrameBufferNode;
struct GFXRenderNode;
struct GFXRenderGraph;


/*!
   Flags for runtime checks
*/
 enum {
    GFX_RENDER_NODE,
    GFX_STEREO_NODE,
    GFX_VIEW_NODE,
    GFX_SHADER_NODE,
    GFX_SCENE_NODE,
    GFX_MESH_NODE,
    GFX_FRAMEBUFFER_NODE,
    GFX_EFFECT_NODE,
    GFX_RENDER_GRAPH
 };

  /*!
   *  A GFXRenderNode is the root node and base class of all graphics processes

   *  It can also register to receive WindowEvents (such as onResize)

   *  It has:  pointer to ONE mDownstream rendernode
   *           pointers to MANY mUpstream rendernodes
   *
   *  The virtual method, onRender(int mode), specifies how
   *  the node should bind or onbind its upstream and downstream
   *  graphics processing events
   *
   *  @TODO Consider Formalizing with Arrows
      @TODO Turn struct into a class with private members
      @TODO Add SceneGraph for more complicated re-ordering
   */
  struct GFXRenderNode  {

      /// get type of node at runtime
      virtual const int nodetype() { return GFX_RENDER_NODE; }

      /// a pointer to common settings and (eventually) organizational management
      GFXRenderGraph * mRenderGraph;
      GFXRenderGraph& graph();
      GFXRenderGraph graph() const;

      /// whether to pass onResize notifications upstream ... (@todo rename)
      bool bResizeCascade = true;

      /// set downstream node
      GFXRenderNode& downstream( GFXRenderNode * p ){
        mDownstream = p;
        return *this;
      }

      /// find most downstream node of RenderGraph (may be this)
//      GFXRenderNode& root(){
//        GFXRenderNode * p = this;
//        GFXRenderNode * tmp = mDownstream;
//        while(tmp!=NULL) {
//          p = tmp;
//          tmp = tmp->mDownstream;
//        }
//        return *p;
//      }

      /// Gets downstream node
      GFXRenderNode& downstream(){
        return *mDownstream;
      }

      /// DivertFrom: redirect argument to this instance's downstream process
      /// See GFXSplitViewNode for example use
      GFXRenderNode& divert(GFXRenderNode& r){
        r.downstream(mDownstream);
        return *this;
      }

      /// ChannelTo: redirect all of this instance's upstream nodes to argument
      /// See GFXSplitViewNode for example use
      GFXRenderNode& channel(GFXRenderNode& r){
        for(auto& i : mUpstream) r << i;
        return *this;
      }

      /// clear all upstream processes @todo free GPU resources
      void reset() {
        if (!bVisited){
          bVisited = true;
          for (auto& i : mUpstream ) i -> reset();
          mUpstream.clear();
          bVisited = false;
        }
      }

      void clear(){
       glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
       glClearColor( 0,0,0,1 );
       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
     }

      void clearDepth(){
        glDepthMask(1);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
      }

      /// Find First Downstream Shader
      GFXShaderNode& shader() {

       auto * tmp = mDownstream;

       while (tmp && tmp->nodetype()!=GFX_SHADER_NODE){
         tmp = tmp->mDownstream;
       }

       if (!tmp) printf("error no shader downstream\n");
       return *(GFXShaderNode*)tmp;
     }


      /// Pointer to one downstream process (for preparing state)
      GFXRenderNode * mDownstream = NULL;
      // Pointers to upstream processes (to be pulled)
      vector<GFXRenderNode*> mUpstream;

      bool bEnable = true;                                                  ///< Enable this Process
      bool bVisited=false;                                                  ///< Node Has been Visited

      int width = 100;
      int height = 100;                                                     ///< Pixels Width and Height
      Vec3f light = Vec3f(1,1,4);                                           ///< Light Position

      void set(int w, int h) { width=w; height=h; }

      /// set width and height, call onInit, and recurse on all upstream nodes
      void init(int w, int h, GFXRenderGraph * r){
          mRenderGraph = r;
          set(w,h);
          onInit();
          for (auto& i : mUpstream) {
            i->init(w,h,r);
          }
      }

      /// Subclasses can implement their own onInit();
      virtual void onInit(){}
      /// on enter
      virtual void onEnter(){}
      /// on exit
      virtual void onExit(){}
      /// on update
      virtual void update(){}

      virtual void onRender(){
        if (!bVisited){ // avoid circular dependencies
          bVisited = true;
            onEnter();
              for (auto& i : mUpstream) i->onRender();
            onExit();
          bVisited = false;
        }
      }


      /// Add upstream node
      GFXRenderNode& operator << (GFXRenderNode * r){
          mUpstream.push_back(r);
          r->downstream(this);
          return *r;
      }

      /// Add upstream node by reference
      GFXRenderNode& operator << (GFXRenderNode& r){
          mUpstream.push_back(&r);
          r.downstream(this);
          return r;
      }

      // GFXPairNode& operator &(GFXRenderNode& r){
      //
      //     return GFXPairNode< decltype(*this), ;
      // }


      /// Resize width and height and cascade upstream @todo add boolean to control cascading
      /// @todo consider not cascading, and having scenegraph manage this communication
      virtual void resize(int w, int h){
        set(w,h);

        if (bResizeCascade) {
         for (auto& i : mUpstream){
            i -> resize(w,h);
         }
        }
      }
  };

template<class TA, class TB>
struct GFXPairNode : GFXRenderNode {
    TA a;
    TB b;

    virtual void onInit(){

      a.onInit();
      b.onInit();

      channelAndDivert(a);
      channelAndDivert(b);

    }

    virtual void onRender(){
      a.onRender();
      b.onRender();
    }

};


/// GFXRenderGraph has a pointer to single GFXRenderNode root
/// and holds settings that are readable by all rendernodes connected to the root node.
struct GFXRenderGraph : public WindowEventHandler  {

      enum {
        IMMEDIATE = 1,         // immediate or programmable pipeline
        ES = 1 << 1,           // embedded OpenGL ES or OpenGL
      };

      enum {
        MONO, ACTIVE, ANAGLYPH, SEQUENTIAL, SPLIT
      };

      int mStereoMode = MONO;//ANAGLYPH;
      int mGLMode = IMMEDIATE;
      bool bLeft = true;

      bool left() { return bLeft; }

      int stereoMode() const { return mStereoMode; }
      bool stereo() const { return !(mStereoMode == MONO); }
      //bool active() { return mStereoMode & ACTIVE; }

      bool immediate() const { return (mGLMode & IMMEDIATE); }
      bool useES() const { return (mGLMode & ES); }

      void immediate(bool b) {
        b ? mGLMode |= IMMEDIATE : mGLMode &= !IMMEDIATE;
      }

      void left(bool b){
        bLeft = b;//if (b) mGLMode |= LEFT; else mGLMode &= ~LEFT;
      }

      GFXRenderNode * mRoot;

      GFXRenderGraph& root( GFXRenderNode * r ) { mRoot = r; return *this; }


      /// initialize with width, height, GL MODE, and STEREO MODE
      void init(GFXRenderNode * r, int w, int h, int glmode = IMMEDIATE, int stereomode=MONO){
        mGLMode = glmode;
        mStereoMode = stereomode;
        root(r);
        if (mRoot -> mUpstream.empty()) printf("warning: root render node is empty\n add nodes to root with << operator\n");
        mRoot -> init(w,h,this);
      }

      virtual void onRender(){
        if (mRoot)
          mRoot -> onRender();
      }


      /// Window Event Callback
      virtual void onResize(int w, int h){
        mRoot -> resize(w,h);
      }


      // move this
 //     Layout layout;
 //     Scene mScene;


 //   //set the frustrum viewport @TODO move this somewhere else, to a view class
 //   void setView(float z, bool isGrid, int row=0, int col=0){
 //
 //     float w = layout.screenWidth;
 //     float h = layout.screenHeight;

 //     //Single Screen Version:
 //     //bottom left corner of screen in pixels
 //     Pose p( -w/2.0,-h/2.0, 0);

 //     layout.speakerL = Vec3f( -w/2.0, 0, 0);
 //     layout.speakerR = Vec3f( w/2.0, 0, 0);

 //     //If we're in multi-screen mode, RE-DO pose positions based on grid layout . . .
 //     if (isGrid) {
 //
 //       p = layout.poseOf( row, col );

 //       cout << "CAMERA POSE" << endl;
 //       p.print();

 //       layout.speakerL = Vec3f(
 //         layout.left( row, col ),
 //         layout.bottom( row, col ) + layout.screenHeight / 2.0, 0);

 //       layout.speakerR = Vec3f(
 //          layout.left( row, col ) + layout.screenWidth,
 //         layout.bottom( row, col ) + layout.screenHeight / 2.0, 0);
 //      }


 //     mScene.viewpose = p;
 //     mScene.camera.pos() = Vec3f( 0, 0, z);
 //     mScene.camera.view = gfx::View( mScene.camera.pos(), p, (float)w/h, h );

 //   }

};

GFXRenderGraph& GFXRenderNode :: graph() { return *mRenderGraph; }
GFXRenderGraph GFXRenderNode :: graph() const { return *mRenderGraph; }



/// Has VertexAttributes and Pointer to a Shader
/// Basic shaders are defined in gfx_glsl.h
/// @TODO Simplify the default version, specialize this version
struct GFXShaderNode : GFXRenderNode {

   virtual const int nodetype() { return GFX_SHADER_NODE; }

   ShaderProgram * program;
   VertexAttributes vatt;

   bool b3D = false; // whether shader handles 3D effect (as in omni mode)

   virtual void onInit(){

        string V = graph().useES() ? DefaultVertES() : DefaultVert();
        string F = graph().useES() ? DefaultFragES() : DefaultFrag();

       // printf ("%s\n", V.c_str());

        program = new ShaderProgram(V,F);
        vatt.add<Vertex>(*program);
    }

   ///  Default Attributes include position, sourceColor, normal, and texCoord (AUTOMATE THIS with GLVertexData?)
   virtual void bindAttributes(){
        //shader id | name | size of vertex data container | offset into container of each parameter
//        program->bind();
//          vatt.add(program->id(), "position", sizeof(Vertex), 0);
//          vatt.add(program->id(), "sourceColor", sizeof(Vertex), Vertex::oc() );
//          vatt.add(program->id(), "normal", sizeof(Vertex), Vertex::on());
//          vatt.add(program->id(), "texCoord", sizeof(Vertex), Vertex::ot());
//        program->unbind();
   }

   virtual void onEnter(){
     if (!graph().immediate()) program->bind();
   }
   virtual void onExit(){
     if (!graph().immediate()) program->unbind();
   }

   virtual void onRender(){
        if ( graph().immediate() ) {
          for (auto& i : mUpstream){ i->onRender(); }
        } else {
          program -> bind();
            for (auto& i : mUpstream){
              i->onRender();
            }
          program -> unbind();
        }
    }

};

/// Render to two framebuffers
/// T is a GFXDrawBufferNode or a GFXFrameBufferNode or a GFXFrameBufferCubeMapNode
template<class T>
struct GFXStereoBufferNode : GFXRenderNode {

  T a, b;

  virtual void onInit(){

    a.onInit();
    b.onInit();

  }

  virtual void onRender(){
    graph().left(true);
    a.onRender();
    graph().left(false);
    b.onRender();
  }

};

/// Draws directly to graphics buffer, either BACK_LEFT, BACK_RIGHT or BACK
struct GFXDrawBufferNode : GFXRenderNode {

  /// bind drawbuffer and pull in upstream renders
  virtual void onRender(){

    /// result of graoh().stereo() and graph().left() is determined downstream
    auto buf = graph().stereo() ? (graph().left() ? GL_BACK_LEFT : GL_BACK_RIGHT ) : GL_BACK;
    glDrawBuffer( buf );

    for (auto& i : mUpstream) i->onRender();
  }

};

struct GFXSlabNode : GFXShaderNode {

    /// Pointer to a Mesh Buffer Object
    MBO * rect;
    /// Pointer to a Texture
    Texture * texture;
    /// Opacity
    float amt=1.0;


    virtual void onInit(){
      program = new ShaderProgram( graph().useES() ? ClipSpaceVertES() : ClipSpaceVert(),
                                   graph().useES() ? TFragAlphaES() : TFragAlpha(), 0);
      bindAttributes();

      rect = new MBO( Mesh::Rect( 2.0, 2.0 ).color(0,0,0,1.0) );
      texture = new Texture( width, height );
    }


    /// Render everything upstream, then bind downstream shader
    virtual void onRender(){

      if (!bVisited) {
        bVisited=true;
        for (auto& i : mUpstream){ i->onRender(); }
      }

      /// Set to downstream width and height
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


/// Render to a 2D Buffer -- 2 swappable Textures and an FBO
struct GFXFrameBufferNode : GFXRenderNode {

   virtual const int nodetype() { return GFX_FRAMEBUFFER_NODE; }

   FBO fbo;

   Texture * texture;
   Texture * textureB;

   virtual void onInit(){
     fbo.set(width,height);
     fbo.init();

     texture = new Texture(width,height);
     textureB = new Texture(width,height);

     fbo.attach(*texture);
   }

   virtual void swap() {
      Texture * tmp = texture; texture = textureB; textureB = tmp;
      fbo.attach(*texture);
    };

   virtual void onEnter(){
      fbo.bind();
      fbo.clear();        // clear viewport, if rendering Anaglyph stereo, we need to only clear left eye
                          // and clear depth() for right eye -- where is this clear depth to be called?
   }

   virtual void onExit(){
      fbo.unbind();
      swap();
   }

   virtual void onRender(){
      onEnter();
        capture();
      onExit();
   }

   virtual void capture(){
      for (auto& i : mUpstream) {
        i->onRender();
      }
   }
};


/// Render to A CubeMap buffer
struct GFXFrameBufferCubeMapNode : GFXFrameBufferNode {

  FBO fbo;

  CubeMap * texture;
  CubeMap * textureB;

  virtual void onInit(){

    fbo.set(width,width);
    fbo.init();

    texture = new CubeMap(width);
    textureB = new CubeMap(width);

    texture->bind();
      fbo.attach(*texture);
    texture->unbind();
  }

  virtual void swap() {
     CubeMap * tmp = texture; texture = textureB; textureB = tmp;
  };

  virtual void capture(){

    auto& s = shader(); //get downstream shader @todo add error check

    for (int i=0;i<6;++i){
      s.program->uniform("cmFace",i); //change to uniform_if ?
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texture->id(), 0 );
      fbo.checkStatus();
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for (auto& i : mUpstream) {
        i->onRender();
      }
    }
  }

};



/// Passing Nodes into Stereo Node Pulls them into to Left and Right Buffers
struct GFXStereoNode : GFXRenderNode {

  virtual void onRender(){

      switch ( graph().stereoMode() ){

        case GFXRenderGraph::MONO:
        {
          graph().left(true);
          for (auto& i : mUpstream) i->onRender();
          break;
        }

        case GFXRenderGraph::ACTIVE:
        {

          glDrawBuffer(GL_BACK_LEFT);
          clear();
          graph().left(true);
          for (auto& i : mUpstream) i->onRender();

          glDrawBuffer(GL_BACK_RIGHT);
          clear();
          graph().left(false);
          for (auto& i : mUpstream) i->onRender(); //i->onRender( mode | STEREO | RIGHT );
          break;
        }

        case GFXRenderGraph::ANAGLYPH:
        {
          glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
          graph().left(true);
          for (auto& i : mUpstream)  i->onRender();

          glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);

          glDepthMask(1);
          glEnable(GL_DEPTH_TEST);
          glClear(GL_DEPTH_BUFFER_BIT);

          graph().left(false);
          for (auto& i : mUpstream) i->onRender();
          break;
        }

        case GFXRenderGraph::SPLIT:
        {
          graph().left(true);
          for (auto& i : mUpstream) i->onRender();

          graph().left(false);
          for (auto& i : mUpstream) i->onRender();

          break;
        }

        case GFXRenderGraph::SEQUENTIAL:
        {
          break;
        }

      }
    }

};


/*!
 *  Render into a viewport (specified as percentage of downstream node)
    Has a pointer to a GFXViewNode::View which holds view data
 */
struct GFXViewNode : GFXRenderNode {

    virtual const int nodetype() { return GFX_VIEW_NODE; }
    /// View data as percentage of screen
    SimpleView * viewData;

    virtual void onInit(){
      viewData = new SimpleView(0,0,1,1);
    }

   virtual void onEnter(){

       // set width and height based on downstream node, viewdata will be a percentage
       if (mDownstream) set( mDownstream->width, mDownstream->height);

       Vec4f& v = viewData->view;
       Vec4f& c = viewData->color;

       glViewport((v[0]*width), (v[1]*height),(v[2]*width),(v[3]*height));
       glScissor((v[0]*width), (v[1]*height),(v[2]*width),(v[3]*height));

       glEnable(GL_SCISSOR_TEST);
       glClearColor(c[0],c[1],c[2],c[3]);
       glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   }

   virtual void onExit(){
       glDisable(GL_SCISSOR_TEST);
   }

};


/// Split View into left and right eyes (similar to &&& from arrows)
struct GFXSplitViewNode : GFXRenderNode {
    GFXViewNode left, right;

    virtual void onInit(){
      left.viewData = new SimpleView(0,0,.5,1,1,0,0);
      right.viewData = new SimpleView(.5,0,.5,1,0,1,0);

      // channel upstream nodes into left and right views
      channel(left);
      channel(right);

      // point left and right views downstream so they can read width/height
      divert(left);
      divert(right);
    }

    virtual void onRender(){
      if (graph().left()) left.onRender();
      else right.onRender();
    }

    virtual void resize(int w, int h){
      for (auto& i : mUpstream) i->resize(w/2.0, h);
    }


};

/// Print output to postscript
// struct GFXPrintNode : GFXRenderNode {
//   gfx::PostScript ps;

//   virtual void onRender (){
    
//   }
// };




/*!
 *  Scene Node To Update Downstream Shader Matrix Uniforms and Call Upstream Mesh Nodes
    Renders in either Immediate and Programmable mode, MONO or STEREO
 */
  struct GFXSceneNode : GFXRenderNode {

       virtual const int nodetype() { return GFX_SCENE_NODE; }

       Scene * mScenePtr;                                           ///< pointer to scene matrix transforms
       float mv[16];                                                ///< matrix float values

      // float eyeSep = .03; //move to lens)

       Scene& scene() { return *mScenePtr; }

       void scene(Scene*s) { mScenePtr=s; }

       virtual void onInit(){
          bResizeCascade = false;
        }

       virtual void resize(int w, int h){
         GFXRenderNode::resize(w,h);
         mScenePtr -> resize(w,h);
       }

       /// reference to shader downstream (to access uniforms and vertex attributes)
       /// @todo error detect
       GFXShaderNode& shader(){ return *(GFXShaderNode*)mDownstream; }

      /// Bind model view projection and normal matrices to downstream shader (and light position...)
      /// (uniform_if does so only if such uniforms exist)
       virtual void update(){
            ShaderProgram& dp = *shader().program;

            dp.uniform_if("lightPosition", light[0], light[1], light[2] ); //2.0, 2.0, 2.0);
            dp.uniform_if("projection",  mScenePtr->xf.proj);
            dp.uniform_if("normalMatrix", mScenePtr->xf.normal);
            dp.uniform_if("modelView",  mScenePtr->xf.modelView );
       }

       /// Update modelview only
       void updateModelView(){
            //static float mv[16];
            (mScenePtr->mvm()).fill(mv);
            ShaderProgram& dp = *shader().program;
            dp.uniform_if("modelView", mv);
       }

       /// Multiply modelview by some object-specific matrix
       void updateModelView( const Mat4f& mat ){
           //static float mv[16];
           (mScenePtr->mvm()*mat).fill(mv);
           ShaderProgram& dp = *shader().program;
           dp.uniform_if("modelView", mv);
      }

      /// Render, updating shader, pushing
      virtual void onRender(){

          // If 3D stereo effect is not encoded by shader, adjust stereo here
          // (otherwise, we will just render normally and let the shader do the 3D)
          //if ( graph().stereo() && !shader().b3D ){

          // Set camera's stereo properties to that of RenderGraph
          scene().camera.stereo(graph().stereo());
          // Set which eye to render
          scene().camera.left(graph().left());

          if (!graph().immediate())
            update();

          mScenePtr->push( graph().immediate() );
              for (auto& i : mUpstream) i->onRender();
          mScenePtr->pop( graph().immediate() );

      }
};




/*-----------------------------------------------------------------------------
 *  DEFAULT RENDERABLES
 *-----------------------------------------------------------------------------*/

  template<> inline
  void Renderable<MBO> :: Draw(const MBO& m, GFXSceneNode * _i){
      // shader is already bound at this point
      _i->updateModelView(); ///< identity matrix
       m.render(_i->shader().vatt);
  }

  template<> inline
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


 /**
 * @brief List of Mesh Buffer Objects of which to bind (VAO or VBO) and drawElements()
   @sa GFXMeshNodeT
 */
 struct GFXMeshNode : public GFXRenderNode {

    virtual const int nodetype() { return GFX_MESH_NODE; }

    vector<MBO*> mMbo;

    GFXMeshNode& add( MBO* m) { mMbo.push_back(m); return *this; }

    /// reference to scene downstream (to access shader) @todo error check
    GFXSceneNode& scene(){ return *(GFXSceneNode*)mDownstream; }


    virtual void onRender(){
        if ( graph().immediate() ){
           for (auto& m : mMbo) {
             render::draw(*m);
           }
        }else {
           scene().updateModelView();
           for (auto& m : mMbo) m->render(shader().vatt);
        }
    }
 };



template<class T>
struct GFXMeshNodeT : GFXMeshNode {

  /// pointer to MBO of type T (@TODO maybe should be a std::vector<TMBO<T>*> mbo)
  TMBO<T> * mbo;
  VertexAttributes vatt;

  //must be bound to a shader downstream already
  void onInit(){

   // vatt = new VertexAttributes();

    auto& s = shader();
   // auto& a = GLVertexData<T>::Attribute;
    /// @TODO test this use of GLVertexData<T>::Attribute map
    for( auto& i : s.program->attributes() ){
      vatt.add(s.program->id(), i.first, sizeof(T), GLVertexData<T>::Attribute[i.first] );//offset( i.first ) );
    }

  }
   virtual void onRender(){
        if ( graph().immediate() ) {// shader().immediate() ){
          // render::begin(mColor[0],mColor[1],mColor[2],mColor[3]);
           render::draw(*mbo);
        } else {
           //scene().updateModelView(); ///< identity matrix
           mbo->render(vatt);
        }
    }

};
} //gfx::

#endif   /* ----- #ifndef gfx_process_INC  ----- */



//      void draw(MBO& m, float r=1.0, float g=1.0, float b=1.0, float a=1.0){
//        if ( downstream().immediate() ){
//           render::begin(r,g,b,a);
//           render::draw(m);
//        }else {
//           updateModelView(); ///< identity matrix
//           m.render(shader().vatt);
//        }
//      }

      // specialize your draw routines by defining a Renderable<T>::Draw(const T& t, GFXSceneNode * r) method
      //
//      template<class T>
//      void draw(const T& t, float r = 1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate=false){
//         if ( downstream().immediate()) {
//           render::begin(r,g,b,a);
//           render::draw(t);
//           cout << "Hello" << endl;
//         }
//         else {
//          if (bUpdate) {
//            Renderable<T>::SetUpdate();
//          }
//          Renderable<T>::UpdateColor(r,g,b,a); ///
//          Renderable<T>::Draw(t,this);  ///< pass "this" to Renderable<T>::Draw in order to access current model view and vertex attributes
//         }
//      }
//
//      //draw many (specialize this by defining a Renderable<vector<T>>::Draw method
//      //where you bind mbo and vertex attributes once, then mesh::drawElements() many times)
//      template<class T>
//      void draw(const vector<T>& t, float r=1.0, float g=1.0, float b=1.0, float a=1.0, bool bUpdate =false){
//          if (downstream().immediate()){
//            render::begin(r,g,b,a);
//            for (auto& i : t) render::draw(i);
//          } else {
//            if (bUpdate) {
//              Renderable<T>::SetUpdate();
//            }
//            Renderable<T>::UpdateColor(r,g,b,a);
//            //for (auto& i : t) Renderable<T>::Draw(i,this);
//            Renderable<vector<T>>::Draw(t, this);
//          }
//        }
//
