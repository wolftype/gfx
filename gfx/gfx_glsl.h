//
//  gfx_glsl.h
//  
/*!

    @file FOR BUILDING GLSL SHADER PROGRAMS UP FROM COMMONLY USED SMALLER PIECES OF CODE

*/
//  Created by Pablo Colapinto on 2/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef gfx_glsl_h
#define gfx_glsl_h

#include "gfx_lib.h"

namespace gfx{

    namespace GLSL{

      /*-----------------------------------------------------------------------------
       *  uniform variable
       *-----------------------------------------------------------------------------*/
       inline string UVar(){ return "uniform float amt;"; }

        
       /*-----------------------------------------------------------------------------
        *  function to return uniform float variable
        *-----------------------------------------------------------------------------*/
        inline string ufloat(string name){
          return "uniform float " + name + ";\n";
        }
       /*-----------------------------------------------------------------------------
        *  function to return uniform sampler2D variable
        *-----------------------------------------------------------------------------*/
        inline string usampler2D(string name){
          return "uniform sampler2D " + name + ";\n";
        }
        
        /*-----------------------------------------------------------------------------
         *  function to return uniform vec variable
         *-----------------------------------------------------------------------------*/

       /*-----------------------------------------------------------------------------
        *  Make a list of attributes based on names from type T
        *-----------------------------------------------------------------------------*/
       template<class T>
       string attributes(){
         string s;
         for (auto& i : T::Init().Attribute){
            s += "attribute " + i.first + ";\n";
         }
         return s;
       }

      /*-----------------------------------------------------------------------------
       *  TEST VERTEX SHADER
       *-----------------------------------------------------------------------------*/
      inline string SimpleVertex (){ return R"(
    
        attribute vec3 position;
    
        void main(void){
          vec4 tmp = vec4(position,1.0);
          gl_Position = tmp;
       }
      )";}


      /*-----------------------------------------------------------------------------
       *  TEST FRAGMENT SHADER
       *-----------------------------------------------------------------------------*/

      inline string SimpleFragment (){ return R"(
    
        void main(void){
          gl_FragColor = vec4(1.0,1.0,1.0,1.0);
        }
      )";}

      
      /*-----------------------------------------------------------------------------
       *  TYPICAL VERTEX ATTRIBUTES
       *-----------------------------------------------------------------------------*/
        inline string AVertex() { return R"(
            attribute vec3 position;
            attribute vec3 normal;
            attribute vec4 sourceColor;
            attribute vec2 texCoord;
        )";
        }
        

        /*-----------------------------------------------------------------------------
         *  TYPICAL MATRIX TRANSFORM UNIFORMS
         *-----------------------------------------------------------------------------*/
        inline string UMatrix() { return R"(
            uniform mat4 modelView;           // Model * View
            uniform mat4 projection;          // Projection Matrix (ortho or frustum)
            uniform mat4 normalMatrix;        // Normal Matrix (inverse transpose of mvm)
        )";
        }
        
        /*-----------------------------------------------------------------------------
         *  TYPICAL VARYING VALUES TO PASS FROM VERTEX TO SHADER
         *-----------------------------------------------------------------------------*/
        inline string VaryingES() { return R"(
            varying vec4 colorDst;
            varying lowp vec2 texco;
        )"; 
        }

        inline string Varying(){ return R"(
            varying vec4 colorDst;
            varying vec2 texco;
        )";
        }


        /*-----------------------------------------------------------------------------
         *  LIGHT POSITION UNIFORM
         *-----------------------------------------------------------------------------*/
        inline string ULight(){ return R"(
            uniform vec3 lightPosition;
        )";
        }
        

        /*-----------------------------------------------------------------------------
         *  LIGHT PROPERTIES
         *-----------------------------------------------------------------------------*/
        inline string ULightProperty (){ return  R"(
            uniform vec3 ambientColor;
            uniform vec3 diffuseMaterial;
            uniform vec3 specularColor;
        )";
       } 

        /*-----------------------------------------------------------------------------
         *  TEXTURE SAMPLER2D UNIFORM
         *-----------------------------------------------------------------------------*/
       inline string USampler () { return R"(
            uniform sampler2D sampleTexture;
        )";}
        
        /*-----------------------------------------------------------------------------
         *  TEXTURE SAMPLER CUBE UNIFORM
         *-----------------------------------------------------------------------------*/
        inline string USamplerCube () {return R"(
            uniform samplerCube sampleTexture;
        )";}

        
        /*-----------------------------------------------------------------------------
         *  VARYING COLOR 
         *-----------------------------------------------------------------------------*/
        inline string VColor (){return R"(
            varying vec4 colorDst;
        )";}

        /*-----------------------------------------------------------------------------
         *  VARYING TEXTURE 
         *-----------------------------------------------------------------------------*/
        inline string VTexES (){return R"(
            varying lowp vec2 texco;
        )";}

        inline string VTex(){return  R"(
            varying vec2 texco;
        )";}

        /*-----------------------------------------------------------------------------
         *  Utility function to create a string for a varying vector
         *-----------------------------------------------------------------------------*/
         template<int N>
         string vec(string name, bool bES=false){
           stringstream s; s << N;
           string res = bES? "lowp ":"";
           return "varying " + res + "vec"+s.str()+ " " + name;
         }

        /*-----------------------------------------------------------------------------
         *  FUNCTION TO CALCULATE NORMAL
         *-----------------------------------------------------------------------------*/
        inline string NTransform(){ return R"(
           vec3 doNormal(vec4 n) {
                return normalize( ( normalMatrix * n ).xyz );
            }
        )";
        }

        
        /*-----------------------------------------------------------------------------
         *  FUNCTION TO TRANSFORM VERTEX (see also cubemap version below)
         *-----------------------------------------------------------------------------*/
        inline string VCalc(){ return R"(
            vec4 doVertex (vec4 v) {
              mat4 m = projection * modelView;
              return m * v;
            }
        )";
        }

         /*-----------------------------------------------------------------------------
         *  FUNCTION TO TRANSFORM VERTEX AND DISPLACE BASED ON Bound SAMPLER 
         *-----------------------------------------------------------------------------*/
        inline string VDisplaceCalc() { return R"(
            vec4 doVertex (vec4 v) {
              float z = texture2D(sampleTexture, texco).r * amt;
              vec4 nv = vec4(v.x, v.y, v.z + z, v.w);
              mat4 m = projection * modelView;
              return m*nv;
            }
        )";
        }


        /*-----------------------------------------------------------------------------
         *  No Projection Matrix
         *-----------------------------------------------------------------------------*/
        inline string VDisplaceCalcSimple(){ return R"(
            
            uniform sampler2D sampleTexture;

            vec3 doNormal(){
              return normal;
            }
            vec4 doColor(){
              texco = texCoord;
              return sourceColor;
            }
            
            vec4 doVertex (vec4 v) {
              vec3 tn = doNormal();
              vec4 sc = doColor();
              float z = texture2D(sampleTexture, texco).r * amt;
              vec4 nv = vec4(v.x, v.y, v.z + z, v.w);
              return nv;
            }
        )";
        }

        
        /*-----------------------------------------------------------------------------
         *  FUNCTION TO CALCULATE LIGHTING
         *-----------------------------------------------------------------------------*/
         inline string VLighting(){ return R"(
            
            uniform vec3 lightPosition;
            
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
                    
                return vec4((sourceColor * df).rgb , sourceColor.a );
            }

        )";
         }

        
        /*-----------------------------------------------------------------------------
         *  MAIN VERTEX FUNCTION
         *-----------------------------------------------------------------------------*/
        inline string MVert() { return R"(
            
            void main(void){
            
                colorDst = doColor();  
                
                texco = texCoord;
                                
                vec4 pos =  vec4(position,1.0);
                vec4 nor = vec4(normal,1.0);
                
                gl_PointSize = 5.0;
                gl_Position = doVertex(pos);
            }
        )";
        }

        /*-----------------------------------------------------------------------------
         *  MAIN FRAGMENT FUNCTION
         *-----------------------------------------------------------------------------*/
         inline string DefaultFragES(){ return R"(

         /*-----------------------------------------------------------------------------
          *  DEFAULT FRAGMENT SHADER ES
          *-----------------------------------------------------------------------------*/
            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
                gl_FragColor = litColor;     //mix(litColor, texColor, .5);
            }
        )"; 
         }

         inline string DefaultFrag() { return R"(

         /*-----------------------------------------------------------------------------
          *  DEFAULT FRAGMENT SHADER
          *-----------------------------------------------------------------------------*/
            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
                gl_FragColor = litColor;      //mix(litColor, texColor, .5);
            }
        )";   
         }

        
         inline string TFragMix() { return R"(
         /*-----------------------------------------------------------------------------
          *  FRAGMENT .5 MIXED WITH TEXTURE
          *-----------------------------------------------------------------------------*/

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
            
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
                gl_FragColor = mix(litColor, texColor, 0.5);           
            }
        )"; 
        }
        

         inline string TFragAlphaES() { return R"(
         /*-----------------------------------------------------------------------------
          *  ALPHA WEIGHTED FRAGMENT SHADER (FOR MOTION BLUR AND TRAILS) 
          *-----------------------------------------------------------------------------*/

            uniform sampler2D sampleTexture;
            uniform float alpha;
            varying vec4 colorDst;
            varying lowp vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
                // gl_FragColor = vec4(texColor.rgb, texColor.a * litColor.a);           
                gl_FragColor = vec4(texColor.rgb, texColor.a * ( alpha *  litColor.a) );           

            }
        )";
         }

         inline string TFragAlpha() { return R"(

            uniform sampler2D sampleTexture;
            uniform float alpha;
            varying vec4 colorDst;
            varying vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
                gl_FragColor = vec4(texColor.rgb, texColor.a * ( alpha * litColor.a) );           
            }
        )";
        }

         /*-----------------------------------------------------------------------------
          *  BLUR FRAGMENT SHADER (FOR FAKE ANTI-ALIASING)
          *-----------------------------------------------------------------------------*/

         inline string TFragBlur(){ return R"(

            uniform sampler2D sampleTexture;  
            uniform float ux;
            uniform float uy;
            uniform float bluramt;
       
            varying vec4 colorDst;
            varying vec2 texco;

            vec4 neighbors( vec2 t, float xoff, float yoff, float amt ){
              vec2 top     = vec2(t.x, t.y + yoff ); 
              vec2 bottom  = vec2(t.x, t.y - yoff); 
              vec2 left    = vec2(t.x - xoff, t.y ); 
              vec2 right   = vec2(t.x + xoff, t.y ); 
              
              vec2 tr  = vec2(t.x + xoff, t.y + yoff ); 
              vec2 tl  = vec2(t.x - xoff, t.y + yoff ); 
              vec2 br  = vec2(t.x + xoff, t.y - yoff ); 
              vec2 bl  = vec2(t.x -xoff, t.y - yoff ); 
                     
              vec4 ts = texture2D(sampleTexture, top );
              vec4 bs = texture2D(sampleTexture, bottom );
              vec4 ls = texture2D(sampleTexture, left );
              vec4 rs = texture2D(sampleTexture, right ); 
              
              vec4 tls = texture2D(sampleTexture, tl );
              vec4 bls = texture2D(sampleTexture, bl );
              vec4 trs = texture2D(sampleTexture, tr );
              vec4 brs = texture2D(sampleTexture, br ); 
              
       //      return vec4( ( ts + bs + ls + rs ) * amt );   
              return vec4( ( ts + bs + ls + rs + tls + bls + trs + brs ) * amt );         
            }
           
            void main(void){
                
                vec4 texColor = texture2D(sampleTexture, texco); 

                vec4 average = neighbors( texco, ux , uy, bluramt/2.0);   ;
                vec4 average2 = neighbors( texco, ux/2.0, uy/2.0, bluramt);   
        
                vec4 litColor = texColor + colorDst;    //force use of variables!  
         
                gl_FragColor = max(texColor, average + average2);           

            }

        )";
        }
      
         /*-----------------------------------------------------------------------------
          *  BLUR FRAGMENT SHADER (FOR FAKE ANTI-ALIASING)
          *-----------------------------------------------------------------------------*/

         inline string TFragBlurES() { return R"(

            uniform sampler2D sampleTexture;  
            uniform float ux;
            uniform float uy;
            uniform float bluramt;
       
            varying vec4 colorDst;
            varying lowp vec2 texco;

            vec4 neighbors( vec2 t, float xoff, float yoff, float amt ){
              vec2 top     = vec2(t.x, t.y + yoff ); 
              vec2 bottom  = vec2(t.x, t.y - yoff); 
              vec2 left    = vec2(t.x - xoff, t.y ); 
              vec2 right   = vec2(t.x + xoff, t.y ); 
              
              vec2 tr  = vec2(t.x + xoff, t.y + yoff ); 
              vec2 tl  = vec2(t.x - xoff, t.y + yoff ); 
              vec2 br  = vec2(t.x + xoff, t.y - yoff ); 
              vec2 bl  = vec2(t.x -xoff, t.y - yoff ); 
                     
              vec4 ts = texture2D(sampleTexture, top );
              vec4 bs = texture2D(sampleTexture, bottom );
              vec4 ls = texture2D(sampleTexture, left );
              vec4 rs = texture2D(sampleTexture, right ); 
              
              vec4 tls = texture2D(sampleTexture, tl );
              vec4 bls = texture2D(sampleTexture, bl );
              vec4 trs = texture2D(sampleTexture, tr );
              vec4 brs = texture2D(sampleTexture, br ); 
              
       //      return vec4( ( ts + bs + ls + rs ) * amt );   
              return vec4( ( ts + bs + ls + rs + tls + bls + trs + brs ) * amt );         
            }
           
            void main(void){
                
                vec4 texColor = texture2D(sampleTexture, texco); 

                vec4 average = neighbors( texco, ux , uy, bluramt/2.0);   ;
                vec4 average2 = neighbors( texco, ux/2.0, uy/2.0, bluramt);   
        
                vec4 litColor = texColor + colorDst;    //force use of variables!  
         
                gl_FragColor = max(texColor, average + average2);           

            }

        )";
         }
             
      
                     


        inline string TFrag() { return R"(   
        /*-----------------------------------------------------------------------------
         *  PURE TEXTURE SHADER (FOR SLABS)
         *-----------------------------------------------------------------------------*/
            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying vec2 texco;
 
            void main(void){               
                
                vec4 texColor = texture2D(sampleTexture, texco); 
                vec4 litColor = texColor + colorDst;    //force use of variables! 
                
                gl_FragColor = texColor;                //mix(litColor, texColor, 0.0);
            }
        )";
        }


      inline string TFragES(){ return R"(   

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
 
            void main(void){               
                
              vec4 texColor = texture2D(sampleTexture, texco); 
              vec4 litColor = texColor + colorDst;    // Force use of variables! 
                
              gl_FragColor = texColor; //mix(litColor, texColor, 0.0);
            }
        )";   
      }


   /*-----------------------------------------------------------------------------
    *  CLIP SPACE VERTEX SHADER (FOR SLABS)
    *-----------------------------------------------------------------------------*/
    inline string ClipSpaceVertES() { return R"( 

      attribute vec3 position; 
      attribute vec3 normal;
      attribute vec4 sourceColor;
      attribute vec2 texCoord;

      varying lowp vec2 texco;
      varying vec4 colorDst;   

      void main(void){  
        texco = texCoord;
        colorDst = sourceColor; 
        vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!!
        gl_Position = vec4(position,1.0);
        }
    )"; 
    }
    
    
    inline string ClipSpaceVert(){ return R"( 

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
    }
         
 
    /*-----------------------------------------------------------------------------
    *  CLIP SPACE VERTEX SHADER (FOR CUBE SLABS) use with FUseCubeMap
    *-----------------------------------------------------------------------------*/
    inline string CubeClipSpaceVertES(){ return R"( 

      attribute vec3 position; 
      attribute vec3 normal;
      attribute vec4 sourceColor;
      attribute vec3 texCoord;

      varying lowp vec3 vTexDir;
      varying vec4 colorDst;   

      void main(void){  
        vTexDir = texCoord;
        colorDst = sourceColor; 
        vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!!
        gl_Position = vec4(position,1.0);
        }
    )"; 
    }
    
    
    inline string CubeClipSpaceVert() { return R"( 

        attribute vec3 position; 
        attribute vec3 normal;
        attribute vec4 sourceColor;
        attribute vec3 texCoord;

        varying vec3 vTexDir;
        varying vec4 colorDst;   

      void main(void){  
        vTexDir = texCoord;
        colorDst = sourceColor; 
        vec3 tn = normal + position;   //FORCE COMPILATION OF THESE TERMS!!
        gl_Position = vec4(position,1.0);
        }
    )";
    }
    
    inline string CubeFrag() { return R"(
        uniform samplerCube cubeMap;            
        varying vec3 vTexDir;
        
        void main(void){
            gl_FragColor = textureCube(cubeMap, vTexDir).rgba; 
        }
    )"; 
    }
           
    /*-----------------------------------------------------------------------------
     *  DEFAULT MESH BUFFER OBJECT SHADER (FOR VERTICES)
     *-----------------------------------------------------------------------------*/
    inline string DefaultVertES(){ return AVertex() + VaryingES() + UMatrix() + NTransform() + VLighting() + VCalc() + MVert(); }
    inline string DefaultVert(){ return  AVertex() + Varying() + UMatrix() + NTransform() + VLighting() + VCalc() + MVert(); }

    
    //Make a vertex shader, passing in a string for doVertex() calculations . . . 
    inline string makeVert(string tCalc, bool bES=false){
      return AVertex() + (bES ? VaryingES() : Varying()) + UMatrix() + NTransform() + VLighting() + tCalc + MVert();
    }
    
    inline string makeDisplacementVert(bool bES=false){
      return UVar() + USampler() + makeVert(VDisplaceCalc(),bES);
    }

    inline string makeDefaultVert(bool bES=false){
      return makeVert(VCalc(), bES);
    }
    
   // string DisplacementVert = UVar + USampler + AVertex + Varying + UMatrix + NTransform + VLighting + VDisplaceCalc + MVert;    
   // string DisplacementVertES = UVar + USampler + AVertex + VaryingES + UMatrix + NTransform + VLighting + VDisplaceCalc + MVert; 
     
   
    /*-----------------------------------------------------------------------------
     *  RENDER TO A CUBEMAP (position function) -- change uniform cmFace every pass
     *-----------------------------------------------------------------------------*/
    inline string MakeCubemapVert() { return R"(
            
          uniform int cmFace;
          uniform float uFar;
          uniform float uNear;
          //varying vec3 vTexDir;
             
             
          vec4 doVertex(vec4 v){
        
           vec4 vertex = modelView * v;
           
           vec3 n = normalize(vertex.xyz); 
                
            // GL_TEXTURE_CUBE_MAP_POSITIVE_X    
                    if (cmFace == 0) { vertex.xyz = vec3(-vertex.z, -vertex.y, -vertex.x); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_X  
            else if (cmFace == 1) { vertex.xyz = vec3( vertex.z, -vertex.y,  vertex.x); }  
            // GL_TEXTURE_CUBE_MAP_POSITIVE_Y    
            else if (cmFace == 2) { vertex.xyz = vec3( vertex.x,  vertex.z, -vertex.y); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y   
            else if (cmFace == 3) { vertex.xyz = vec3( vertex.x, -vertex.z,  vertex.y); }  
            // GL_TEXTURE_CUBE_MAP_POSITIVE_Z    
            else if (cmFace == 4) { vertex.xyz = vec3( vertex.x, -vertex.y, -vertex.z); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z   
            else          { vertex.xyz = vec3(-vertex.x, -vertex.y,  vertex.z); }  
                
            // convert into screen-space:  
            // simplified perspective projection since fovy = 90 and aspect = 1  
            vertex.zw = vec2(  
              (vertex.z*(uFar+uNear) + vertex.w*uFar*uNear*2.)/(uNear-uFar),  
              -vertex.z  
            );  
          
            return vertex;  
            
          }

        )";
    }
        

    
    /*-----------------------------------------------------------------------------
     *  RENDER OMNI STEREO TO A CUBE MAP (ripped from wakefield's code in AlloSystem al_OmniStereo.hpp)
     *-----------------------------------------------------------------------------*/
    inline string MakeOmniStereoCubeMapVert() { return R"(
            
          uniform int cmFace;
          uniform float uFar;
          uniform float uNear;
          
          uniform float omni_eye;       //0, 1, -1
          uniform float omni_radius;    //in GL units
             
          vec4 doVertex(vec4 v){
        
           vec4 vertex = modelView * v;
                    
           float l = length(vertex.xz);
           vec3 n = normalize(vertex.xyz); 

           float displacement = omni_eye *
            (omni_radius * omni_radius -
               sqrt(l * l * omni_radius * omni_radius +
                    omni_eye * omni_eye * (omni_radius * omni_radius - l * l))) /
            (omni_radius * omni_radius - omni_eye * omni_eye);

             vertex.xz += vec2(displacement * vn.z, displacement * -vn.x);
           
                
            // GL_TEXTURE_CUBE_MAP_POSITIVE_X    
                    if (cmFace == 0) { vertex.xyz = vec3(-vertex.z, -vertex.y, -vertex.x); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_X  
            else if (cmFace == 1) { vertex.xyz = vec3( vertex.z, -vertex.y,  vertex.x); }  
            // GL_TEXTURE_CUBE_MAP_POSITIVE_Y    
            else if (cmFace == 2) { vertex.xyz = vec3( vertex.x,  vertex.z, -vertex.y); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y   
            else if (cmFace == 3) { vertex.xyz = vec3( vertex.x, -vertex.z,  vertex.y); }  
            // GL_TEXTURE_CUBE_MAP_POSITIVE_Z    
            else if (cmFace == 4) { vertex.xyz = vec3( vertex.x, -vertex.y, -vertex.z); }  
            // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z   
            else          { vertex.xyz = vec3(-vertex.x, -vertex.y,  vertex.z); }  
                
            // convert into screen-space:  
            // simplified perspective projection since fovy = 90 and aspect = 1  
            vertex.zw = vec2(  
              (vertex.z*(uFar+uNear) + vertex.w*uFar*uNear*2.)/(uNear-uFar),  
              -vertex.z  
            );  
          
            return vertex;  
            
          }

        )";
    }

  
           

   /*-----------------------------------------------------------------------------
    *  UNUSED ANTI-ALIASING
    *-----------------------------------------------------------------------------*/
    inline string FXDB() { return R"(
      uniform sampler2D sampleTexture; 
      uniform vec2 frameBufSize;
       // uniform vec2 frameBufSize;

      varying vec4 colorDst; 
      varying lowp vec2 texco;
        
      void main(void){
         //vec2 frameBufSize = vec2( 1920.0, 1080.0 );
          
        vec3 rgbNW=texture2D(sampleTexture,texco+(vec2(-1.0,-1.0)/frameBufSize)).xyz;
        vec3 rgbNE=texture2D(sampleTexture,texco+(vec2(1.0,-1.0)/frameBufSize)).xyz;
        vec3 rgbSW=texture2D(sampleTexture,texco+(vec2(-1.0,1.0)/frameBufSize)).xyz;
        vec3 rgbSE=texture2D(sampleTexture,texco+(vec2(1.0,1.0)/frameBufSize)).xyz;
        vec3 rgbM=texture2D(sampleTexture,texco).xyz;
          
        vec3 nc = (rgbNW + rgbNE + rgbSW + rgbSE + rgbM)/5.0;
        
        //vec4 texColor = texture2D(sampleTexture, texco); 
        
        vec4 litColor = vec4(rgbM,1.0) + colorDst;    //force use of variables!  
         
        gl_FragColor= vec4(rgbM,1.0);//max(texColor, average + average2);
      }
      
    )";  
    }
    
    inline string FOGL(){ return R"(
       uniform sampler2D sampleTexture;
        uniform vec2 frameBufSize;

      varying vec4 colorDst; 
      varying vec2 texco;     
     )";   
    }
    
    inline string FOGLES() { return R"(
       uniform sampler2D sampleTexture;
       uniform vec2 frameBufSize;

      varying vec4 colorDst; 
      varying lowp vec2 texco;     
     )";     
    }
                            
    inline string FXAA() { return R"(
  
      void main(void) {
        //gl_FragColor.xyz = texture2D(buf0,texCoords).xyz;
        //return;
                 
        //vec2 frameBufSize = vec2( 1360.0, 768.0 );

        float FXAA_SPAN_MAX = 8.0;
        float FXAA_REDUCE_MUL = 1.0/8.0;
        float FXAA_REDUCE_MIN = 1.0/128.0;

        vec3 rgbNW=texture2D(sampleTexture,texco+(vec2(-1.0,-1.0)/frameBufSize)).xyz;
        vec3 rgbNE=texture2D(sampleTexture,texco+(vec2(1.0,-1.0)/frameBufSize)).xyz;
        vec3 rgbSW=texture2D(sampleTexture,texco+(vec2(-1.0,1.0)/frameBufSize)).xyz;
        vec3 rgbSE=texture2D(sampleTexture,texco+(vec2(1.0,1.0)/frameBufSize)).xyz;
        vec3 rgbM=texture2D(sampleTexture,texco).xyz;

        vec3 luma=vec3(0.299, 0.587, 0.114);
        float lumaNW = dot(rgbNW, luma);
        float lumaNE = dot(rgbNE, luma);
        float lumaSW = dot(rgbSW, luma);
        float lumaSE = dot(rgbSE, luma);
        float lumaM  = dot(rgbM,  luma);

        float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
        float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

        vec2 dir;
        dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
        dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

        float dirReduce = max(
          (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
          FXAA_REDUCE_MIN);

        float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

        dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
            max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
            dir * rcpDirMin)) / frameBufSize;

        vec3 rgbA = (1.0/2.0) * (
          texture2D(sampleTexture, texco.xy + dir * (1.0/3.0 - 0.5)).xyz +
          texture2D(sampleTexture, texco.xy + dir * (2.0/3.0 - 0.5)).xyz);
        vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
          texture2D(sampleTexture, texco.xy + dir * (0.0/3.0 - 0.5)).xyz +
          texture2D(sampleTexture, texco.xy + dir * (3.0/3.0 - 0.5)).xyz);
        float lumaB = dot(rgbB, luma);
                
        vec4 litColor = vec4(rgbA,1.0) + colorDst;    //force use of variables!  

        if((lumaB < lumaMin) || (lumaB > lumaMax)){
          gl_FragColor = vec4(rgbA,1.0);
        }else{
          gl_FragColor =  vec4(rgbB,1.0);
        }
      }
      )";
    }
           
/*-----------------------------------------------------------------------------
 *  FIXED FUNCTION SHADER CODE
 *-----------------------------------------------------------------------------*/
inline string FFLightingVert() { return R"(
    varying vec4 color;
    varying vec3 normal;
    varying vec3 lightDir;
    varying vec3 eyeVec;
    void main(){
      color = gl_Color;
      vec4 vertex =  gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
      normal = gl_NormalMatrix * gl_Normal;
      vec3 V = vertex.xyz;
      eyeVec = normalize(-V);
      lightDir = normalize(vec3(gl_LightSource[0].position.xyz - V));
      gl_Position = vertex; 
    }
  )";
}


inline string FFLightingFrag() { return R"(
    uniform float lighting;
    varying vec4 color;
    varying vec3 normal;
    varying vec3 lightDir;
    varying vec3 eyeVec;
    void main() {
      vec4 final_color = color * gl_LightSource[0].ambient;
      vec3 N = normalize(normal);
      vec3 L = lightDir;
      float lambertTerm = max(dot(N, L), 0.0);
      final_color += gl_LightSource[0].diffuse * color * lambertTerm;
      vec3 E = eyeVec;
      vec3 R = reflect(-L, N);
      float spec = pow(max(dot(R, E), 0.0), 0.9 + 1e-20);
      final_color += gl_LightSource[0].specular * spec;
      gl_FragColor = mix(color, final_color, .8); //color;//
    }
  )"; 
  }


        // static string SVert = R"(
        //     
        //                   attribute vec3 position;
        //                   // attribute vec3 normal;
        //                   // attribute vec4 sourceColor;
        //                   // attribute vec2 texCoord;
        //                 uniform mat4 modelView;         // Model * View
        //                // uniform mat4 submodel;          // Glyph-specific Transform
        //                 uniform mat4 projection;        // Projection Matrix (ortho or frustrum)
        //                // uniform mat4 normalMatrix;      // Normal Matrix (inverse transpose of mvm)
        //   
        //   vec4 doVertex (vec4 v) {
        //     mat4 m = projection * modelView;// * submodel;
        //                     return m * v;
        //                 }
        //   
        //    void main(void){
        //             
        //     vec4 np = doVertex( vec4(position, 1.0) );
        //             
        //     gl_Position = np;
        //   }
        // );
        // 
        // static string SFrag = R"(
        //           
        //       
        //    void main(void){
        //     
        //     gl_FragColor = vec4(1.0,1.0,1.0,1.0);
        //   }
        // );
        //
        //

        
    } //GLSL::

} //ctl::

#endif
