/*
 * =====================================================================================
 *
 *       Filename:  xCubeMap.cpp
 *
 *    Description:  render a scene to a cube map and draw spherical projection onto screen
 *
 *        Version:  1.0
 *        Created:  02/02/2015 18:53:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */


#include "gfx_app.h"
#include "util/glut_window.hpp"

#include "util/gfx_stat.h"

using namespace gfx;


template<class T>
struct GFXMeshNode : GFXRenderNode {

  TMBO<T> mbo;
  VertexAttributes vatt;

  void init(){
    cout <<"initializing mesh node" << endl;
    for( auto& i : downstream().program->attributes() ){
      cout << i.first << endl;
      vatt.add(downstream().program->id(), i.first, sizeof(T), offset( i.first ) );
    }
  }

  virtual GLvoid * offset(string s){
    if (s == "normal") return T::on();
    if (s == "sourceColor") return T::oc();
    if (s == "texCoord") return T::ot();
    return 0;
  }

  void onRender(){
    mbo.render( vatt );
  }
  
};




struct CubeMap {
  
  CubeMap(int w, int h) : mWidth(w), mHeight(h) { init(); }

  GLuint mID;
  int mWidth, mHeight;

  GLenum mType = GL_UNSIGNED_BYTE;
  void type(GLenum t) { mType = t; }

  GLuint id() const { return mID; }


  void init(){
    glEnable(GL_TEXTURE_CUBE_MAP);
    generate();
    alloc();
    param();

  }

  void generate(){
    glGenTextures(1, &mID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
  }

  void alloc(){
    for (int i = 0;i<6;++i){
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, //< target
                    0,                                //< lod
                    GL_RGBA,                          //< internal format
                    mWidth, mHeight, 0,               //< width, height, border
                    GL_RGBA,                          //< format of data
                    mType,                            //< data type (e.g. GL_FLOAT)
                    NULL);                            //< no actual data yet
    }
  }

  void param(){
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
  }

  void bind(){
    glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
  }
  void unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }
};


struct RenderToCubeMap : GFXShaderNode {
  FBO fbo;
  CubeMap * texture;
  CubeMap * textureB;

  void init(){
    program = new ShaderProgram(makeVert(MakeCubemapVert), DefaultFrag, 0);
    bindAttributes();
    
    //careful of max width and height here
    texture = new CubeMap(width,height);
    textureB = new CubeMap(width,height);
   
    fbo.set(width,height);
    //fbo.depth(false);
    fbo.init();

    texture->bind();
    fbo.bind();   
      //  for (int i=0;i<6;++i){
          glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture->id(), 0 );
      //  }
    fbo.unbind();
    texture->unbind();
  }

   void swap() { 
      CubeMap * tmp = texture; texture = textureB; textureB = tmp;  
   };

  void onRender(){
      fbo.bind();
      program->bind();
      program->uniform("uNear",0.1f);
      program->uniform("uFar",10.f);
      glViewport(0, 0, width, height ); 
      glClearColor(0,0,0,0);
     // glClearDepth(1.f);
      for (int i=0;i<6;++i){
        program->uniform("cmFace",i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, texture->id(), 0 );
        fbo.checkStatus();
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (auto& i : mUpstream) {
          i->update(); //< updates currently bound shader's matrices
          i->onRender();
        }
      }
      program->unbind();
    fbo.unbind();
    swap();

  }

};

struct TexMap {

  TMBO<VertexTex3D> mbo;

  void init(){
                                      // w | h | spacing | lambda for assigning texture coordinates
    auto m = mesh::uvtex<VertexTex3D>( 11, 11, .2, [](float a, float b){ 
      
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

struct CubeSlab : GFXRenderNode {

    VertexAttributes vatt;

    TexMap rect;
    CubeMap * texture;

    virtual void init(){
      program = new ShaderProgram(CubeClipSpaceVert, CubeFrag);
      program->bind();
          /* vatt.add(program->id(), "position", sizeof(VertexTex3D), 0); */ 
          /* vatt.add(program->id(), "sourceColor", sizeof(VertexTex3D), VertexTex3D::oc() ); */ 
          /* vatt.add(program->id(), "normal", sizeof(VertexTex3D), VertexTex3D::on()); */ 
          /* vatt.add(program->id(), "texCoord", sizeof(VertexTex3D), VertexTex3D::ot()); */
        for( auto& i : program->attributes() ){
          cout << i.first << endl;
          vatt.add(program->id(), i.first, sizeof(VertexTex3D), offset( i.first ) );
      }
      program->unbind();

      rect.init();
    }

    virtual GLvoid * offset(string s){
      if (s == "normal") return VertexTex3D::on();
      if (s == "sourceColor") return VertexTex3D::oc();
      if (s == "texCoord") return VertexTex3D::ot();
      return 0;
    }

    virtual void onRender(){
      
      for (auto& i : mUpstream) i->onRender(); //call upstream render to framebuffer


    if (mDownstream) glViewport(0,0,mDownstream->width, mDownstream->height);

      program->bind();
        texture->bind();
          rect.mbo.render(vatt);
        texture->unbind();
      program->unbind();
    }

};



struct MyApp : GFXApp<GlutContext> {

 MBO mbo;
 RenderToCubeMap cubeSceneRenderer;
 CubeSlab cubeSlab;

 void setup(){
        
    mRenderer.immediate(false); 

    Rand::Seed();

    float span = 5.0;
    int num = 10;
    Mesh m;
    m.mode(GL::T);
    for (int i=0;i<num;++i){
    for (int j=0;j<num;++j){
    for (int k=0;k<num;++k){
      
      Vec3f pos = Vec3f(-span/2.0,-span/2.0,-span/2.0) + Vec3f((float)i/num,(float)j/num,(float)k/num) * span;
      Vec3f a = pos + Vec3f(0,.2,0);
      Vec3f b = pos + Vec3f(.2,.1,0);
      m.add(pos).add().add(a).add().add(b);

    }}}

    for (auto& i : m.vertex() ){
      i.Col.set( Rand::Num(), Rand::Num(), Rand::Num(), 1.0 );
    }
    m.store();
    mbo = m;

    cubeSceneRenderer.set(500,500);
    cubeSceneRenderer.init();

    cubeSlab.set(width,height);
    cubeSlab.init();
    cubeSlab.texture = cubeSceneRenderer.texture; //bind textures


    mRenderer << cubeSlab << cubeSceneRenderer << this;//bypass sceneRender mSceneRenderer;

 }


 void onDraw(){    
    draw(mbo,1,0,0);
 }

};


int main(){

  MyApp app;
  app.start();
  return 0;
}
