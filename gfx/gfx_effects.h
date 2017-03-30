  /*
 * =====================================================================================
 *
 *       Filename:  gfx_effects.h
 *
 *    Description:  slab, motion blur, FBO, etc
 *
 *        Version:  1.0
 *        Created:  02/22/2016 11:48:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef  gfx_effects_INC
#define  gfx_effects_INC

#include "gfx_render.h"

namespace gfx {

  
  /*!
   *  A SLAB billboards a texture to the screen (with optional alpha)
   *  
   *  Can be used as a component in R2T
   */
  struct Slab : public GFXShaderNode {
  
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

    virtual void onRender(){
      
      if (!bVisited) {
        bVisited=true;
        for (auto& i : mUpstream){ i->onRender(); }
      }

      /// reset to downstream width and height (if it exists)
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
        
        program = new ShaderProgram( graph().useES() ? ClipSpaceVertES() : ClipSpaceVert(), 
                                     graph().useES() ? TFragBlurES() : TFragBlur(), 0);
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
   *   mRenderer.reset()    //<-- empty upstream render
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

      ra.init(width,height, mRenderGraph);
      rb.init(width,height, mRenderGraph);

      lastFrame.init(width,height,mRenderGraph);
      outputMix.init(width,height,mRenderGraph); 
      
      rb.fbo.depth(false);
      outputMix.amt=0.995;

      //confusing for now because of difference between
      //binding into a node process and binding a texture result
      //could overload so that slabs "know" they are being bown to
      //a framebuffer capture and automatically point to their texture
    
      //channel this upstream render calls into capture call
      channel(ra);

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

/// Binds a Cubemap Shader renders scene 6 times to FBO
struct RenderToCubeMap : GFXShaderNode {

  FBO fbo;

  CubeMap * texture;
  CubeMap * textureB;

  void onInit(){
    
    program = new ShaderProgram(makeVert(MakeCubemapVert()), DefaultFrag(), 0);

    bindAttributes();
    
    //be careful of max width and height here
    texture = new CubeMap(width);
    textureB = new CubeMap(width);
    
    fbo.set(width,width);
    fbo.init();

    texture->bind();
      fbo.attach(*texture);
    texture->unbind();

  }

   void swap() { 
      CubeMap * tmp = texture; texture = textureB; textureB = tmp;  
   };

   virtual void onEnter(){
      fbo.bind();
      glViewport(0, 0, width, width ); 
      glClearColor(0,0,0,0);
     
        program->bind();
          program->uniform("uNear",0.1f);
          program->uniform("uFar",10.f);
   }

   virtual void onExit(){
      program->unbind();
      fbo.unbind();
      swap();
   }

   virtual void onRender(){
      onEnter();
        capture();
      onExit();
   }

   void clear(){
      if (graph().left()) {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      } else {
        glDepthMask(1);
        glEnable(GL_DEPTH_TEST);
        glClear( GL_DEPTH_BUFFER_BIT );
      }
   }

  void capture(){
    

    for (int i=0;i<6;++i){
      
      program->uniform("cmFace",i);
      
      glFramebufferTexture2D(
          GL_FRAMEBUFFER, 
          GL_COLOR_ATTACHMENT0, 
          GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 
          texture->id(), 
          0 
      );
      clear();

      fbo.checkStatus();
      //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //STEREO CHECK?
      for (auto& i : mUpstream) {
        i->onRender();
      }
    }

  }

};



///// Binds an OmniStereo CubeMap Shader and renders 6 times to FBO
//struct RenderToOmniStereoCubeMap : GFXShaderNode {
//
//  GFXFrameBufferCubeMapNode cmLeft, cmRight;
//  GFXShaderOmniCubeMapNode cmShader;
//
//  void onInit(){
//    program = new ShaderProgram(makeVert(MakeOmniStereoCubeMapVert()), DefaultFrag(), 0);
//
//    bindAttributes();
//    
//    cmLeft.onInit();
//    cmRight.onInit();
//        
//  }
//
//
//  void onRender(){
//    program->bind();
//   
//      program->uniform("uNear",0.1f);
//      program->uniform("uFar",10.f);
//      program->uniform("omni_radius",5);
//
//      if ( left() ) {
//        program->uniform("omni_eye",-1);
//          cmLeft.onEnter();
//            cmLeft.capture(program);
//          cmLeft.onExit();
//      } else {
//       program->uniform("omni_eye",1);
//       cmRight.onEnter();
//         cmRight.capture(program);
//       cmRight.onExit();
//      }
//      
//    program->unbind();
//  }
//
//
//};



/// Map 2D u,v coordinates to 3D cube map texture look up of s,t,v coordinates
struct TexMap {

  TMBO<VertexTex3D> mbo;

  void onInit(){
                                      // w | h | spacing | lambda for assigning texture coordinates
    auto m = mesh::uvtex<VertexTex3D>( 21, 21, .1, [](float a, float b){ 
      
      float phi = -PIOVERFOUR + b * PIOVERTWO;
      float theta = a * PI;

      Quat qa(theta, Vec3f(0,1,0));
      Quat qb(phi, Quat::spin(Vec3f(0,0,1),qa)); 

      Vec3f v = Quat::spin(Vec3f(1,0,0), qb*qa);
      
      return v.unit();
    
    } );
    
    mbo = m;
  }

};

/// Looks up a 3D Cubemap Texture based on s,r,t values
struct CubeSlab : GFXShaderNode {

    TexMap rect;
    CubeMap * texture;
    
    /**
    * @brief creates shader program, binds it and iterates through its attributes,
             adding vertex attributes to the vertex attribute array

             @todo looks upstream for a bufferNode and binds texture to it
    */
    virtual void onInit(){

      program = new ShaderProgram(CubeClipSpaceVert(), CubeFrag());
      vatt.add<VertexTex3D>(*program);

      rect.onInit();

      // if there is an upstream framebuffernode, bind to its texture (experimental...)
      
    }

    virtual void onRender(){
    
 
      for (auto& i : mUpstream) i->onRender(); //call upstream render to framebuffer
      if (mDownstream) {
        glViewport(0,0,mDownstream->width, mDownstream->height);
      }

      program->bind();
        texture->bind();
          rect.mbo.render(vatt);
        texture->unbind();
      program->unbind();
    
    }

};

/// StereoCube
struct StereoCubeSlab : GFXShaderNode {

    TexMap rect;
    CubeMap * texture;

    virtual void onInit(){
      program = new ShaderProgram(CubeClipSpaceVert(), CubeFrag());
      vatt.add<VertexTex3D>(*program);

      rect.onInit();
    }

    virtual void onRender(){
      
      for (auto& i : mUpstream) i->onRender(); //call upstream render to framebuffer
      if (mDownstream) {
        glViewport(0,0,mDownstream->width, mDownstream->height);
      }

      program->bind();
        texture->bind();
          rect.mbo.render(vatt);
        texture->unbind();
      program->unbind();
    
    }

};


/// StereoScene Renders Left and Right Eye by moving camera position
struct StereoScene : GFXSceneNode {

  float eyeSep = 1; ///< eye separation

  virtual void onRender(){
    
    auto tmpPos = scene().camera.pos();
    auto tmpX = scene().camera.x();

    auto eyeOffset = graph().stereo() ? (tmpX * eyeSep * .5) * ( graph().left() ? -1 : 1) : Vec3f(0,0,0);

    scene().camera.pos() = tmpPos + eyeOffset ;     
     
    mScenePtr->push( graph().immediate() );//shader().immediate() );
     for (auto& i : mUpstream) i->onRender(); //call upstream 
    mScenePtr->pop( graph().immediate()  );//shader().immediate() );

    scene().camera.pos() = tmpPos;

  }
  
//    switch( mMode & ANAGLYPH ){
//      case 1:
//      {
//        
//       glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
//       scene().camera.pos() = tmpPos + (tmpX * eyeSep * .5);      
//       mScenePtr->push( immediate() );//shader().immediate() );
//        for (auto& i : mUpstream) i->onRender(); //call upstream 
//       mScenePtr->pop( immediate()  );//shader().immediate() );
//
//       glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
//       glDepthMask(1);
//       glEnable(GL_DEPTH_TEST);
//       glClear(GL_DEPTH_BUFFER_BIT);
//
//       scene().camera.pos() = tmpPos - (tmpX * eyeSep * .5);        
//       mScenePtr->push(immediate() );//shader().immediate() );
//        for (auto& i : mUpstream) i->onRender(); //call upstream
//       mScenePtr->pop(immediate() );//shader().immediate() );
//
//       scene().camera.pos() = tmpPos;
//      
//       break;
//      }
//      
//    }

};



} //gfx::

#endif   /* ----- #ifndef gfx_effects_INC  ----- */
