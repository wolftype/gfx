/*
 * =====================================================================================
 *
 *       Filename:  gfx_data.h
 *
 *    Description:  data mapping for vertex attribute arrays
 *
 *        Version:  1.0
 *        Created:  02/29/2016 12:51:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef gfx_data_INC
#define gfx_data_INC

namespace gfx {

/// Attributes Classification (experimental)
template <class
          T>  // attribute types, e.g. Vertex or VertexPosition or VertexColor
struct GLVertexData
{

  /// Attribute information
  struct Att
  {
    string type;
    string name;
  };

  /// Per template T static map of attribute names ("position", "normal", "color") and their offsets
  static map<string, GLvoid *> Attribute;

  /// Singletons per T
  static GLVertexData &Init ()
  {
    static GLVertexData TheGLVertexData;
    return TheGLVertexData;
  }

 private:
  GLVertexData () { Make (); }
  static void Make ();
};

/// The Attribute name and offset map
template <class T>
map<string, GLvoid *> GLVertexData<T>::Attribute;


}  // gfx::



#endif /* ----- #ifndef gfx_data_INC  ----- */
