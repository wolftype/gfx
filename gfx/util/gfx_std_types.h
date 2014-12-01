/*
 * =====================================================================================
 *
 *       Filename:  gfx_std_types.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2014 17:33:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef gfx_std_types_h
#define gfx_std_types_h

#include <map>
#include <typeinfo>

using namespace std;
namespace gfx { namespace util {
    
    enum StandardTypes {
        BOOLEAN,
        UCHAR,
        CHAR,
        UINT,
        INT,
        ULONG,
        LONG,
        FLOAT,
        DOUBLE        
    };

    typedef std::map< const char *, int> NameMap;

    NameMap create_types(){
        NameMap m;
        bool b;
        unsigned char uc;
    //    char c;
        float f;
        double d;
        int i;
        
        m[ typeid(b).name() ] = BOOLEAN;
        m[ typeid(uc ).name() ] = UCHAR;
        m[ typeid(f ).name() ] = FLOAT;
        m[ typeid(d ).name() ] = DOUBLE;
        m[ typeid(i ).name() ] = DOUBLE;
        return m;
    }

    static NameMap Types = create_types();

} } //gfx::util::

#endif
