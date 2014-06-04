/*
 * =====================================================================================
 *
 *       Filename:  hypershader.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/03/2014 20:10:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */
#include "gfx_glsl.h"

//STRINGIFY(
//
string hyper2 = R"(
        attribute vec3 position; 
        attribute vec3 normal;
        attribute vec4 sourceColor;
        attribute vec2 texCoord;

        varying vec2 texco;
        varying vec4 colorDst;   

      void main(void){  
        texco = texCoord;
        colorDst = sourceColor; 
        vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!!
        gl_Position = vec4(position,1.0);
        }
)";



string hyper = R"( 

attribute vec3 position;
attribute vec3 normal;
attribute vec4 sourceColor;
attribute vec2 texCoord;

uniform mat4 modelView;           // Model * View
uniform mat4 projection;          // Projection Matrix (ortho or frustum)
uniform mat4 normalMatrix;        // Normal Matrix (inverse transpose of mvm)

uniform vec3 lightPosition;
uniform sampler2D sampleTexture;

uniform float amt;

varying vec2 texco;
varying vec4 colorDst;

vec3 doNormal(vec4 n) {
     return normalize( ( normalMatrix * n ).xyz );
}

vec4 doMatXform(vec4 pos){
  return projection * modelView * pos;
}

vec4 doColor(){
    
    //Infinite Viewer
    vec3 L = normalize( lightPosition );
    vec3 N = doNormal( vec4(normal,1.0) );
    vec3 E = vec3(0,0,1); 
    vec3 H = normalize( L + E );
    
    //Diffusion Coefficient
    float df = max(0.0, dot(N,L) );

    //Specular Coefficient (not used here)
    float sf = max(0.0, dot(L,H) );
        
    return vec4((sourceColor * df).xyz , 1 );
}
 

struct VecR4 {
	float e1,e2,e3,e4;
};

struct Biv {
	float e14,e24,e34;
};

struct Rot {
	float s,e14,e24,e34;
};

struct Vec_Rot {
	float e1,e2,e3,e4,e124,e134,e234;
};

VecR4 r4( vec4 v){
  return VecR4(v.r,v.g,v.b,v.a);
}

Rot gen( const Biv b ){

  float dot = b.e14 * b.e14 + b.e24 * b.e24 + b.e34 * b.e34;
  float w = sqrt(dot);
  float cs = cos(w);
  float sn = -sin(w) / w; 
  
  return Rot(cs, b.e14 * sn, b.e24 * sn, b.e34 * sn); 
}

Vec_Rot v_gp_r(const VecR4 a, const Rot b){
  return Vec_Rot(
  a.e1 * b.s - a.e4 * b.e14,
  a.e2 * b.s - a.e4 * b.e24,
  a.e3 * b.s - a.e4 * b.e34,
  a.e1 * b.e24 - a.e2 * b.e14,
  a.e1 * b.e34 - a.e3 * b.e14,
  a.e1 * b.e14 + a.e2 * b.e24 + a.e3 * b.e34 + a.e4 * b.s,
  a.e2 * b.e34 - a.e3 * b.e24
  );
}


Rot reverse(const Rot a){
  return Rot(a.s,-a.e14,-a.e24,-a.e34);
}

VecR4 vr_gp_r(const Vec_Rot a, const Rot b){
  return VecR4(
    a.e1 * b.s - a.e4 * b.e14 - a.e124 * b.e24 - a.e134 * b.e34,
    a.e2 * b.s - a.e4 * b.e24 + a.e124 * b.e14 - a.e234 * b.e34,
    a.e3 * b.s - a.e4 * b.e34 + a.e134 * b.e14 + a.e234 * b.e24,
    - a.e124 * b.e34 + a.e134 * b.e24 - a.e234 * b.e14
  );
}


VecR4 spin(const VecR4 a, const Rot b){
  return vr_gp_r(v_gp_r(a,b), reverse(b) );
}

vec3 project( const VecR4 v ){
  float wt = 1.0 / (1.0 - v.e4);
  return vec3(v.e1/wt, v.e2/wt, v.e3/wt );
}

vec3 transform( vec4 v, vec3 biv){
  return project( spin( r4(v), gen( Biv(biv.r, biv.g, biv.b) ) ) );
}  

vec3 doVertex(vec4 p, vec2 tex){
  vec3 t = vec3(amt,0,0);//texture2D(sampleTexture, tex).xyz);
  return transform(p,t); 
}

void main(void){
   
  colorDst = vec4(1,0,0,1);//sourceColor;// + doColor();       
  texco = texCoord;
  vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!!
                       
  gl_PointSize = 5.0;
  gl_Position = vec4(doVertex(sourceColor,texco),1.0);

} 

)";

/*   vec4 pos = vec4(position,1.0); */
/*   vec4 nor = vec4(normal,1.0); */
/*   vec4 pn  = vec4(pos + nor); */
       

/* void main(void){ */  
/*   texco = texCoord; */
/*   colorDst = sourceColor; */ 
/*   vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!! */
/*   gl_Position = vec4(position,1.0); */
/*  } */



 /* void main(void){ */  
 /*   texco = texCoord; */
 /*   colorDst = sourceColor; */ 
 /*   vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!! */
 /*   gl_Position = vec4(position,1.0); */
 /*   } */

/* )"; */

