/*
 * =====================================================================================
 *
 *       Filename:  gfx_renderable.h
 *
 *    Description:  p
 *
 *        Version:  1.0
 *        Created:  02/22/2016 12:08:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef gfx_renderable_INC
#define gfx_renderable_INC

#include "gfx_mbo.h"

namespace gfx {

struct GFXSceneNode;

/*-----------------------------------------------------------------------------
   *  Programmable Pipeline
   *-----------------------------------------------------------------------------*/
template <class T>
struct RenderableBase
{
  ///Look up by type
  static vector<MBO> &Get () { return MeshBuffer<T>::Get (); }

  /// Look up by address
  static MBO &Get (const T &t) { return MeshBuffer<T>::Get (t); }

  static void SetUpdate ()
  {
    vector<MBO> &vm = Get ();
    for (auto &m : vm)
      {
        m.bUpdate = true;
      }
  }

  static void UpdateColor (float r, float g, float b, float a)
  {
    vector<MBO> &vm = Get ();
    for (auto &m : vm)
      {
        if (m.shouldUpdate ())
          {
            m.mesh.color (r, g, b, a);
            m.update ();
            m.bUpdate = false;
          }
      }
  }

  static void Update ()
  {
    vector<MBO> &vm = Get ();
    for (auto &m : vm)
      {
        m.update ();
      }
  }


  static void UpdatePosition (const T &t) {}
};

template <class T>
struct ModelMatrix
{
  static Mat4f Make (const T &t)
  {
    printf (R"(no matrix model defined for this type)");
  }
};

template <class T, int Mode = 0>
struct Renderable : RenderableBase<T>
{

  using Base = RenderableBase<T>;

  static void Draw (const T &t, GFXSceneNode *_i)
  {
    printf (R"(Renderable<T>::Draw routine not yet defined for this type:
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
  }

  static void Draw (const T &t, const Mat4f &model, GFXSceneNode *_i)
  {
    printf (
      R"(Renderable<T>::Draw (with ModelMatrix) routine not yet defined for this type:
              You must delcare a Renderable<CLASSNAME>::Draw function
              See gfx/gfx_render.h)");
  }

  static void DrawImmediate (const T &t)
  {
    printf (
      R"(Renderable<T>::DrawImmediate routine not yet defined this type)");
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
//  template<class T>
//  struct Drawable {
//
//    static void Draw(const T& t){
//       printf("Drawable<T>::Immediate routine is not yet specialized\n");
//    }
//
//    template<class B>
//    static void Draw(const B& b){
//      Drawable<B>::Draw(b);
//    }
//
//  };
//
//  template<> inline void Drawable<MBO>::Draw(const MBO& m){
//#ifdef GFX_IMMEDIATE_MODE
//#endif
//  }

template <>
inline void Renderable<MBO, 0>::DrawImmediate (const MBO &m)
{
#ifdef GFX_IMMEDIATE_MODE
  mesh::drawElements (m.mesh);
#endif
}


namespace render {
#ifdef GFX_IMMEDIATE_MODE
inline void begin (float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0)
{
  glNormal3f (0, 0, 1);
  glColor4f (r, g, b, a);
}

inline void color (float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0)
{
  glColor4f (r, g, b, a);
}

template <class A, class B>
inline void drawAt (const A &a, const B &p)
{
  glPushMatrix ();
  glTranslatef (p[0], p[1], p[2]);
  Renderable<A>::DrawImmediate (a);
  glPopMatrix ();
}

template <class A, class B>
inline void drawBAt (const A &a, const B &p)
{
  glPushMatrix ();
  glTranslatef (p[0], p[1], p[2]);
  Renderable<A, 1>::DrawImmediate (a);
  glPopMatrix ();
}

template <typename A>
inline void draw (const A &a)
{
  glPushMatrix ();
  Renderable<A>::DrawImmediate (a);
  glPopMatrix ();
}

template <typename A>
inline void drawB (const A &a)
{
  glPushMatrix ();
  Renderable<A, 1>::DrawImmediate (a);
  glPopMatrix ();
}

#else
inline void begin (float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0)
{
  printf ("a fixed functionality draw routine has been specified but OpenGL ES "
          "does not allow it\n");
}
inline void color (float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0)
{
  printf ("a fixed functionality draw routine has been specified but OpenGL ES "
          "does not allow it\n");
}
template <class A, class B>
inline void drawAt (const A &a, const B &p)
{
  printf ("a fixed functionality draw routine has been specified but OpenGL ES "
          "does not allow it\n");
}
template <typename A>
inline void draw (const A &a)
{
  printf ("a fixed functionality draw routine has been specified but OpenGL ES "
          "does not allow it\n");
}
#endif

template <typename A>
inline void pipe (const A &a, GFXSceneNode *re)
{
  Renderable<A>::Draw (a, re);
}

}  //render::
}  //gfx::

#endif /* ----- #ifndef gfx_renderable_INC  ----- */
