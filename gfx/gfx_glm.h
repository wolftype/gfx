/*
 * =====================================================================================
 *
 *       Filename:  gfx_glm.h
 *
 *    Description:  glm data types and operations
 *
 *        Version:  1.0
 *        Created:  09/05/2014 15:52:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  gfx_glm_INC
#define  gfx_glm_INC

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"


//#include "gfx_scene.h"

namespace gfx {

  /*-----------------------------------------------------------------------------
   *  VERTEX DATA 
   *-----------------------------------------------------------------------------*/
  struct GVertex{
    glm::vec3 position;                     //<-- An xyz coordinate of the vertex
    glm::vec3 normal;                       //<-- A normal at the vertex
    glm::vec4 color;                        //<-- A color at the vertex
    glm::vec2 textureCoordinate;            //<-- A texture coordinate of the vertex

    //Offsets into memory
    static size_t offsetNormal() { return sizeof(glm::vec3); }
    static size_t offsetColor() { return sizeof(glm::vec3) * 2; }
    static size_t offsetTextureCoordinate() { return sizeof(glm::vec3)*2 + sizeof(glm::vec4); }
  };
  
  
  /*-----------------------------------------------------------------------------
   *  MODEL TRANSFORM
   *-----------------------------------------------------------------------------*/
  struct Model {

    glm::mat4 matrix;
    
    Model() : mPos(0,0,0), mRot(1,0,0,0), mScale(1){}

    Model& pos(float x, float y, float z) { mPos = glm::vec3(x,y,z); return *this;}
    glm::vec3 pos() const { return mPos; }

    //position in world space
    glm::vec3 mPos;
    //orientation in world space
    glm::quat mRot;
    //scale
    float mScale;

    Model& operator()(){     
      glm::mat4 scale = glm::scale( glm::mat4(), glm::vec3(mScale) );
      glm::mat4 rotate = glm::toMat4( mRot );
      glm::mat4 translate = glm::translate( glm::mat4(), mPos );

      matrix = translate * rotate * scale;

      return *this;     
    }

     GLfloat * ptr(){ return glm::value_ptr(matrix); }

  };


  /*-----------------------------------------------------------------------------
   *  VIEW TRANSFORM
   *-----------------------------------------------------------------------------*/
  struct View {

    glm::mat4 matrix;
    glm::vec3 mPos, mTarget, mUp;

    template<class T=glm::vec3>
    View& operator()(T pos = T(0,0,5), T targ = T(0,0,0), T up=T(0,1,0)){
      matrix = glm::lookAt( glm::vec3( pos[0],  pos[1],  pos[2]),
                           glm::vec3(targ[0], targ[1], targ[2]),
                           glm::vec3(  up[0],   up[1],   up[2]) );
      return *this;
    }

    GLfloat * ptr(){ return glm::value_ptr(matrix); }
  };


    
  /*-----------------------------------------------------------------------------
   *  PROJECTION TRANSFORM
   *-----------------------------------------------------------------------------*/
  struct Projection{
    glm::mat4 matrix;

    Projection& operator()( float rad = PI/3.0, float ratio = 4.0/3.0, float near=.01, float far=100.0){
      matrix = glm::perspective( rad, ratio, near, far);
      return *this;
    }
    GLfloat * ptr() { return glm::value_ptr(matrix); }
  };

  
  /*-----------------------------------------------------------------------------
   *  NORMAL MATRIX
   *-----------------------------------------------------------------------------*/
  struct Normal {
    glm::mat3 matrix;

    Normal& operator()(const glm::mat4& view, const glm::mat4& model){
      matrix = glm::transpose( glm::inverse( glm::mat3(view*model) ) );
      return *this;
    }

    GLfloat * ptr() { return glm::value_ptr(matrix); }
  };
  

  

};

#endif   /* ----- #ifndef gfx_glm_INC  ----- */
