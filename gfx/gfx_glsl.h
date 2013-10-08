//
//  vsr_glsl.h
//  Versor
/*

    FOR BUILDING GLSL SHADER PROGRAMS UP FROM COMMONLY USED SMALLER PIECES OF CODE

*/
//  Created by Pablo Colapinto on 2/28/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef gfx_glsl_h
#define gfx_glsl_h

#ifndef STRINGIFY
#define STRINGIFY(A) #A
#endif

namespace gfx{

    namespace GLSL{
        
		string SimpleVertex = STRINGIFY(
		
			attribute vec3 position;
		
			void main(void){
				vec4 tmp = vec4(position,1.0);
				gl_Position = tmp;
			}
		);

		string SimpleFragment = STRINGIFY(
		
			void main(void){
				gl_FragColor = vec4(1.0,1.0,1.0,1.0);
			}
		);


        string AVertex = STRINGIFY(
            attribute vec3 position;
            attribute vec3 normal;
            attribute vec4 sourceColor;
            attribute vec2 texCoord;
        );
        
        string UMatrix = STRINGIFY(
            uniform mat4 modelView;         // Model * View
           // uniform mat4 submodel;          // Glyph-specific Transform
            uniform mat4 projection;        // Projection Matrix (ortho or frustrum)
            uniform mat4 normalMatrix;      // Normal Matrix (inverse transpose of mvm)
        );
        
        string ULight = STRINGIFY(
            uniform vec3 lightPosition;
        );
        
        string ULightProperty = STRINGIFY(
            uniform vec3 ambientColor;
            uniform vec3 diffuseMaterial;
            uniform vec3 specularColor;
        );
        
        string USampler = STRINGIFY(
            uniform sampler2D sampleTexture;
        );
        
        string USamplerCube = STRINGIFY(
            uniform samplerCube sampleTexture;
        );
        
        //Pass from Vertex to Fragment Shader
        string Varying = STRINGIFY(
            varying vec4 colorDst;
            varying lowp vec2 texco;
        ); 

        //Pass from Vertex to Fragment Shader
        string VaryingN = STRINGIFY(
            varying vec4 colorDst;
            varying vec2 texco;
        );
        

        
        string VColor = STRINGIFY(
            varying vec4 colorDst;
        );

        string VTex = STRINGIFY(
            varying vec2 texco;
        );
//        
//        string ColorCube = STRINGIFY(
//            
//        );
        
        string NTransform = STRINGIFY(
           vec3 doNormal(vec4 n) {
                return normalize( ( normalMatrix * n ).xyz );
            }
        );
        
        // string VPass = STRINGIFY(
        //     vec4 doVertex (vec4 v) { return gl_ModelViewProjectionMatrix * v; }
        // );

        string VCalc = STRINGIFY(
            vec4 doVertex (vec4 v) {
				mat4 m = projection * modelView;// * submodel;
                return m * v;
            }
        );

        //Calculate Lighting
        string VLighting = STRINGIFY(
            
            uniform vec3 lightPosition;
            
            vec4 doColor(){
                
                //Infinite Viewer
                vec3 L = normalize( lightPosition );
                vec3 N = doNormal( vec4(normal,1.0) );
                vec3 E = vec3(0,0,1); 
                vec3 H = normalize( L + E );
                
                //Diffusion Coefficient
                float df = max(0.0, dot(N,L) );

                //Specular Coefficient
                float sf = max(0.0, dot(L,H) );
                    
                return vec4((sourceColor * df).xyz , 1 );
            }

        );

        string MVert = STRINGIFY(
            
            void main(void){
            
				colorDst = doColor();  
				colorDst = sourceColor;
                
                texco = texCoord;
                                
                vec4 pos =  vec4(position,1.0);
                vec4 nor = vec4(normal,1.0);
                
                //xm = submodel;
                
			    gl_PointSize = 10.0;
                gl_Position = doVertex(pos);
            }
        );
 
        string MVertN = STRINGIFY(
            
            void main(void){
            
				//colorDst = doColor();  
				colorDst = sourceColor;
                
                texco = texCoord;
                                
                vec4 pos =  vec4(position,1.0);
                vec4 nor = vec4(normal,1.0);
                
                //xm = submodel;
				vec4 np = doVertex(pos);
			    gl_PointSize = 10.0;
				gl_Position = np;
            }
        );       
        
         string MFragES = STRINGIFY(

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
				gl_FragColor = litColor;//mix(litColor, texColor, .5);
            }
        ); 

         string MFrag = STRINGIFY(

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
				gl_FragColor = litColor;//mix(litColor, texColor, .5);
            }
        );   

         string TFragMix = STRINGIFY(

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
            
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
				gl_FragColor = mix(litColor, texColor, 0.5);     			
            }
        ); 

         string TFragAlphaES = STRINGIFY(

            uniform sampler2D sampleTexture;
			uniform float alpha;
            varying vec4 colorDst;
            varying lowp vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
				// gl_FragColor = vec4(texColor.rgb, texColor.a * litColor.a);     			
				gl_FragColor = vec4(texColor.rgb, texColor.a * ( alpha * .1 * litColor.a) );     			

            }
        );

         string TFragAlpha = STRINGIFY(

            uniform sampler2D sampleTexture;
			uniform float alpha;
            varying vec4 colorDst;
            varying vec2 texco;
           
            void main(void){
                
                vec4 litColor = colorDst;
                
                vec4 texColor = texture2D(sampleTexture, texco);
                
				// gl_FragColor = vec4(texColor.rgb, texColor.a * litColor.a);     			
				gl_FragColor = vec4(texColor.rgb, texColor.a * ( alpha * litColor.a) );     			

            }
        );
 
         string TFragBlur = STRINGIFY(

            uniform sampler2D sampleTexture;  
   			uniform float ux;
			uniform float uy;
			uniform float bluramt;
			 
            varying vec4 colorDst;
            varying lowp vec2 texco;

             


			vec4 neighbors( vec2 t, float xoff, float yoff, float amt ){
				vec2 top = vec2(t.x, t.y + yoff ); 
				vec2 bottom = vec2(t.x, t.y - yoff); 
				vec2 left = vec2(t.x - xoff, t.y ); 
				vec2 right = vec2(t.x + xoff, t.y ); 
				
				vec2 tr  = vec2(t.x + xoff, t.y + yoff ); 
				vec2 tl   = vec2(t.x - xoff, t.y + yoff ); 
				vec2 br   = vec2(t.x + xoff, t.y - yoff ); 
				vec2 bl    =vec2(t.x -xoff, t.y - yoff ); 
				   		
				vec4 ts = texture2D(sampleTexture, top );
				vec4 bs = texture2D(sampleTexture, bottom );
				vec4 ls = texture2D(sampleTexture, left );
				vec4 rs = texture2D(sampleTexture, right ); 
				
			   	vec4 tls = texture2D(sampleTexture, tl );
				vec4 bls = texture2D(sampleTexture, bl );
				vec4 trs = texture2D(sampleTexture, tr );
				vec4 brs = texture2D(sampleTexture, br ); 
				
 //   			return vec4( ( ts + bs + ls + rs ) * amt );   
				return vec4( ( ts + bs + ls + rs + tls + bls + trs + brs ) * amt ); 				
			}
           
            void main(void){
                
                vec4 texColor = texture2D(sampleTexture, texco); 

				vec4 average = neighbors( texco, ux , uy, bluramt/2.0);   ;
				vec4 average2 = neighbors( texco, ux/2.0, uy/2.0, bluramt);   
				
				vec4 litColor = texColor + colorDst;    //force use of variables!  
   			
				gl_FragColor = max(texColor, average + average2);     			

            }
        );
                            

          string TFrag = STRINGIFY(   

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying vec2 texco;
 
            void main(void){               
                
                vec4 texColor = texture2D(sampleTexture, texco); 
				vec4 litColor = texColor + colorDst;    //force use of variables! 
                
				gl_FragColor = texColor;//mix(litColor, texColor, 0.0);
            }
        );


         string TFragES = STRINGIFY(   

            uniform sampler2D sampleTexture;  
            varying vec4 colorDst;
            varying lowp vec2 texco;
 
            void main(void){               
                
                vec4 texColor = texture2D(sampleTexture, texco); 
				vec4 litColor = texColor + colorDst;    //force use of variables! 
                
				gl_FragColor = texColor;//mix(litColor, texColor, 0.0);
            }
        );   


		string ClipSpaceVertES = STRINGIFY( 

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
		); 
		
		
		string ClipSpaceVert = STRINGIFY( 

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
		);
		     
		
		
		string DefaultVert =  AVertex + Varying + UMatrix  + NTransform + VLighting + VCalc + MVert;
		
		
		// string TFragBasic = STRINGIFY(     
		// 
		// 	uniform sampler2D tex; 
		// 	varying lowp vec2 texco; 
		// 
		// 	void main(void){
		//       //  vec4 nc = colorDst
		//         vec4 texColor = texture2D(tex, texco);   
		// 		gl_FragColor = texColor;
		//     }
		// ); 
        
        //cubemapping function [in progress]
        string VMakeCubemap = STRINGIFY(
            
            uniform cmFace;
            varying vec3 vTexDir;
             
            vec4 cubemap(vec4 vertex){
                vec3 n = normalize(vertex.xyz); //NORMAL
                
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
				else				  { vertex.xyz = vec3(-vertex.x, -vertex.y,  vertex.z); }	
                
                // convert into screen-space:	
				// simplified perspective projection since fovy = 90 and aspect = 1	
				vertex.zw = vec2(	
					(vertex.z*(uFar+uNear) + vertex.w*uFar*uNear*2.)/(uNear-uFar),	
					-vertex.z	
				);	
				return vertex;	
            }
        );
        
        string FUseCubemap = STRINGIFY(
            uniform samplerCube cubeMap;            
            varying vec3 vTexDir;
            
            void main(void){
                textureCube(cubeMap, vTexDir).rgb 
            }
        );   
           

		string FXDB = STRINGIFY(
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
			
		);    
		
		string FOGL = STRINGIFY(
 			uniform sampler2D sampleTexture;
		    uniform vec2 frameBufSize;

			varying vec4 colorDst; 
			varying vec2 texco;   	
	   );   
		
		string FOGLES = STRINGIFY(
 			uniform sampler2D sampleTexture;
		    uniform vec2 frameBufSize;

			varying vec4 colorDst; 
			varying lowp vec2 texco;   	
	   );   	
                            
		string FXAA = STRINGIFY(
  
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
			);
           


				// static string SVert = STRINGIFY(
				//     
				// 			            attribute vec3 position;
				// 			            // attribute vec3 normal;
				// 			            // attribute vec4 sourceColor;
				// 			            // attribute vec2 texCoord;
				// 		            uniform mat4 modelView;         // Model * View
				// 		           // uniform mat4 submodel;          // Glyph-specific Transform
				// 		            uniform mat4 projection;        // Projection Matrix (ortho or frustrum)
				// 		           // uniform mat4 normalMatrix;      // Normal Matrix (inverse transpose of mvm)
				// 	
				// 	vec4 doVertex (vec4 v) {
				// 		mat4 m = projection * modelView;// * submodel;
				// 		                return m * v;
				// 		            }
				// 	
				// 	 void main(void){
				// 						
				// 		vec4 np = doVertex( vec4(position, 1.0) );
				// 						
				// 		gl_Position = np;
				// 	}
				// );
				// 
				// static string SFrag = STRINGIFY(
				// 			    
				// 			
				// 	 void main(void){
				// 		
				// 		gl_FragColor = vec4(1.0,1.0,1.0,1.0);
				// 	}
				// );
        
    } //GLSL::

} //ctl::

#endif
