/*
 *  gfx_shader.h
 *
 *  
 *
 */

#ifndef GFX_SHADER_H_INCLUDED
#define GFX_SHADER_H_INCLUDED

#include "gfx_gl.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>


namespace gfx {
    
using namespace std;
    
/// Shader Parameter Base Class for Attributes and Uniforms  
struct ShaderParam {
    
    enum Type {
        ATTRIBUTE = 0,
        UNIFORM        
    };
    
    Type ptype; 
        
    ShaderParam() : buf(256) {}
    
    ShaderParam(ShaderParam::Type t, GLuint p, GLuint i = 0) 
    : idx(i), buf(256), ptype(t) { 
        if (t == ATTRIBUTE ) getAttrib(p);
        else getUniform(p);
    }
    
    GLint id;
    GLuint idx;
    GLsizei buf;
    GLsizei len;
    GLint size;
    GLenum vectype;
    GLenum datatype;      //float, int, ubyte, etc
    
    GLchar name[256];

    
    void getAttrib(GLuint program) {
        glGetActiveAttrib(program, idx, buf, &len, &size, &vectype, name);
        getAttribId(program);
        datatype = GL::type(vectype); //Get GL Type from Vectype
    }

    void getUniform(GLuint program) {
        glGetActiveUniform(program, idx, buf, &len, &size, &vectype, name);
        getUniformId(program);
        datatype = GL::type(vectype);
    }
    
    void getAttrib(GLuint program, string tname) {
        id = glGetAttribLocation(program, tname.c_str());
        glGetActiveAttrib(program, id, buf, &len, &size, &vectype, name);
        datatype = GL::type(vectype); //Get GL Type from Vectype
    }

    void getUniform(GLuint program, string tname) {
        id = glGetUniformLocation(program, tname.c_str());
        glGetActiveUniform(program, id, buf, &len, &size, &vectype, name);
        datatype = GL::type(vectype);
    }
    
    void getUniformId(GLuint sprog){
        id = glGetUniformLocation(sprog, name);
    }
    void getAttribId(GLuint sprog){
        id = glGetAttribLocation(sprog, name);
    }
    
    void print(){
        printf(" name: %s\n id: %d\n bytes: %d\n cmp: %d\n", name, id, GL::bpp(datatype), GL::cmp(vectype) );
    }
    
}; 
    
struct Attribute : public ShaderParam {
    Attribute(){}
    Attribute(GLuint p, GLuint i) : ShaderParam(ATTRIBUTE, p, i ) {}
    Attribute(GLuint p, string n) : ShaderParam(ATTRIBUTE, p, glGetAttribLocation(p, n.c_str() ) ) {}
};

struct Uniform : public ShaderParam {
    Uniform(){}
    Uniform(GLuint p, GLuint i) : ShaderParam(UNIFORM, p, i ) {}        
    Uniform (GLuint p, string n) : ShaderParam(UNIFORM, p, glGetUniformLocation(p, n.c_str() ) ) {}
};
    
    
/*-----------------------------------------------------------------------------
 *  Shader Source Compilation
 *-----------------------------------------------------------------------------*/
class Shader {

public:
    enum Type {
		  VERTEX  = GL_VERTEX_SHADER,
		  FRAGMENT = GL_FRAGMENT_SHADER
	  };
    
    GLint id() const { return mId; }
    string name() const { return mName; }

    Shader() : bLoaded(0), bActive(0) {}

    Shader ( Shader::Type t ) : bLoaded(0), bActive(0), mType(t) {}

    Shader( string file, Shader::Type t ) 
    : bLoaded(0), bActive(0), mType(t) {
        load( file, mType );
    }
    
    /// Concatenate src into Source Code string;
    Shader& add(string src);

    /// Set Source Code and Compile
    void source(string src, Shader::Type);                         ///< Load Shader Src Code
   
    //deprecated for the moment 
    void load(string file, Shader::Type);                           ///< Load Shader File(.frag and .vert)

    void compile();                                                 ///< compile code
    void compilerCheck(GLuint ID);                                  ///< check compilation success

    void unload() { glDeleteShader( mId ); mId = -1; }

    
    /// Get CURRENTLY BOUND SHADER PROGRAM
    static const GLint Current() {
        GLint tn;
        glGetIntegerv(GL_CURRENT_PROGRAM, &tn);
        GL::error("Shader Current Program");
        return tn;
    }
   
    /// Print out Shader Source Code
    void print(){
      printf("%s:\n%s", mType==VERTEX?"Vertex Source":"Fragment Source", mSrc.c_str()); 
    }
    
private:

    /// Unique ID
    GLint mId;

    /// Is Loaded
    bool bLoaded;
    
    /// Is Active boolean
    bool bActive;   
    
    /// Source Code String
    string mSrc;

    /// Shader Type (Vertex or Fragment, no support for GEOMETRY yet)
    Shader::Type mType;
    
    /// Name of Shader (@todo is this needed?)
    string mName;

};


inline void Shader :: source(string shadersrc, Shader::Type t) {
    mSrc = shadersrc;
    mType = t;
    bLoaded = true;
    compile();
}

  /// Add source code to shader
	inline Shader& Shader :: add (string src) {
	    mSrc += src;
      return *this;
	}


  /// Compile Shader
	inline void Shader::compile(){

	    string t = mType == VERTEX ? "vertex" : "fragment";
	    cout << "compiling " << t << " shader" << endl; 

	    const char * cSrc = mSrc.c_str();
      GLenum type = mType;
	    mId = glCreateShader( type );
      GL::error("Shader create");

	    glShaderSource(mId, 1, &cSrc, NULL);
      GL::error("Shader Source");

	    glCompileShader(mId);
      
	    GL::error("Shader Compile");

      compilerCheck(mId);
 }

  /// Check for Compiler errors
  inline void Shader::compilerCheck(GLuint ID){
	    GLint comp;
	    glGetShaderiv(mId, GL_COMPILE_STATUS, &comp);

	    if(comp == GL_FALSE ){
          cout << "Shader Compilation FAILED" << endl;
	        GLchar messages[256];
	        glGetShaderInfoLog(mId, sizeof(messages),0,&messages[0]);
	        cout << messages;
	    }
	}




/*-----------------------------------------------------------------------------
 *  SHADER PROGRAM
 *-----------------------------------------------------------------------------*/
class ShaderProgram {

    bool      bLinked;                          ///< Linked Boolean
	  bool			bLoaded;                          ///< Loaded Boolean
	  bool			bActive;                          ///< Active Boolean
			
    GLint 	mId;                                ///< Compiled and Linked Shader Program

    map<string, Attribute> mAttributeMap;       ///< Udata
    map<string, Uniform> mUniformMap;           ///< Udata	
    
        
    //Change this to a Map (so attributes and uniforms can be indexed by string)            
   // vector<Attribute> mAttribute;               ///< Udata
   // vector<Uniform> mUniform;                   ///< Udata

  //  typedef map<string, Attribute>::iterator AttIt;
  //  typedef map<string, Uniform>::iterator UniIt;

	public:

    map<string, Attribute> attributes() const { return mAttributeMap; }
    //map<string, Uniform> uniforms() const { return mUniformMap; }

    bool uniformExists(string s){
      return mUniformMap.find(s) != mUniformMap.end();
    }
    
    Shader vert, frag;

    ShaderProgram() : bLoaded(0), mId(0) {}

    /// Init by loading from file or from source strings directly (default)
    ShaderProgram( string vs, string fs, bool bFile=0) : bLoaded(0), mId(0) {
	      printf("Shader Program Initialize\n");
        if (bFile) load(vs,fs);
	      else source(vs, fs);
    }

    //single name
    ShaderProgram( string vs ) : bLoaded(0), mId(0) {
        load(vs + ".vert",vs + ".frag");
        get();
    }
   
    GLint id() const { return mId; }

    void attach( const Shader& s );                         ///< Attach Vert or Frag Shader to Program
    void link();                                            ///< Link Programs Together
    void linkCheck(GLuint ID);

    void load(const char * shaderName);                     ///< Load Shader From File(.frag and .vert)
    void load(string, string);                              ///< Load Fragment and Vertex Shaders from File
  
    void source( string vs, string fs);                     ///< Load from Source

    void bind() const;
    void unbind() const;

    bool linked()  const   { return bLinked; }
    bool loaded()  const   { return bLoaded; }
	 
		void enable()	{ bActive = 1; }										      ///< enable Shader
		void disable()	{ bActive = 0; }										    ///< disable Shader
		void toggle()	{ bActive = !bActive; }			              ///< toggle Shader

    void unload();
    
    void get();                                             ///< get attributes and uniforms
    void printSource();                                     ///< print shader source
    void printVariables();                                  ///< print shader uniform and attributes
    void print(){ printSource(); printVariables(); }        ///< print both source and variables
    
    //unused?
    void setUniformVariable (char * name, float value);     ///< Change a Variable in the Shader 
		void setTexture (char * name, int id);                  ///< Unused?

    /// Get ID of uniform by name
    GLint uniform(const char * name) { 
        return mUniformMap[ string(name) ].id; 
    }
    /// Get ID of attribute by name
    GLint attribute(const char * name) { 
        return mAttributeMap[ string(name) ].id; 
    }        


    /*-----------------------------------------------------------------------------
     *  Setting Uniforms
     *-----------------------------------------------------------------------------*/
    /// Set uniform if it exists
    template<class ... TS>
    const ShaderProgram& uniform_if(const char * name, TS ... var){
      if (uniformExists(name)) return uniform(name,var...);
      return *this;
    }

    const ShaderProgram& uniform(const char * name, int v0) {
        glUniform1i(uniform(name), v0);	
        GL::error("Set Shader Program Uniformi");
        return *this;
    }
    
    const ShaderProgram& uniform(const char * name, float v0) {
        glUniform1f(uniform(name), v0);	
        GL::error("Set Shader Program Uniformf");
        return *this;
    }
    
    const ShaderProgram& uniform(const char * name, float v0, float v1) {
        glUniform2f(uniform(name), v0, v1);	
        GL::error("Set Shader Program Uniformf");
        return *this;
    }
        
    const ShaderProgram& uniform(const char * name, float x, float y, float z ){
        glUniform3f(uniform(name), x, y, z);
        GL::error("Set Shader Program Uniform3f");
        return *this;
    }

    const ShaderProgram& uniform(const char * name, float vo[16]) {
        glUniformMatrix4fv(uniform(name), 1, GL_FALSE, vo); 
        GL::error("Set Shader Program UniformMat4fv");
        return *this;
    }

    const ShaderProgram& uniform(const char * name, const Mat4f& m) {
	      static float vo[16];
        m.fill(vo);
        glUniformMatrix4fv(uniform(name), 1, GL_FALSE, vo); 
        GL::error("Set Shader Program UniformMat4fv");
        return *this;
    }

    const ShaderProgram& attribute(const char * name, float x, float y, float z){
        glVertexAttrib3f(attribute(name), x, y, z);
        GL::error("Set Shader Program Attribute3f");
        return *this;
    }
    
 
    /*-----------------------------------------------------------------------------
     *  THE FOLLOWING STATIC FUNCTIONS OPERATE ON WHATEVER SHADER IS CURRENTLY BOUND
     *-----------------------------------------------------------------------------*/
    static const int UniformID( const char * name){
        GLint loc = glGetUniformLocation( Shader::Current(), name);
        if (loc == -1) printf("No such uniform named \"%s\" in shader\n", name);
        GL::error("Get Shader Program Uniform Location");
        return loc; 
    }
    
    static const void SetUniform( const char * name, float vo[16] ) {
        glUniformMatrix4fv( UniformID(name), 1, GL_FALSE, vo );
        GL::error("Static set SHADER UniformMat4fv");
    }

    static const void SetUniform( int idx, float vo[16] ) {
        glUniformMatrix4fv( idx, 1, GL_FALSE, vo );
        GL::error("Static set SHADER UniformMat4fv");
    }   
    
};


	inline void ShaderProgram :: attach( const Shader& s){
      cout << "attaching shader to program" << endl;
	    glAttachShader( mId, s.id() );
	    GL::error("Shader Attach");
	}

	inline void ShaderProgram :: link(){
	    glLinkProgram(mId);
	    GL::error("Shader Link");
	}


	inline void ShaderProgram::source( string vs, string fs){
		  bLoaded = false;	

	    mId = glCreateProgram(); 
      GL::error("ShaderProgram Create");
		  printf("Program ID: %d\n", mId);

      // Load Shader and Source Code, Check for Errors
	    vert.source(vs, Shader::VERTEX);
	    frag.source(fs, Shader::FRAGMENT);
	
      // Attach Shaders to Program
	    attach(vert);
	    attach(frag);

      // Link and Validate
	    link();
      linkCheck(mId);

//	    printf("Shader Program id %d: %s %s\n", mId, vs.c_str(), fs.c_str() );

	    bLoaded = true;
	    bActive = true;

      // Get Shader Parameters
	    get();
	}

	inline void ShaderProgram :: load ( string vs, string fs){

		bLoaded = false;	

	    mId = glCreateProgram();  

	    vert.load(vs, Shader::VERTEX);
	    frag.load(fs, Shader::FRAGMENT);

	    attach(vert);
	    attach(frag);
	    link();

	//    printf("Shader Program: %d: %s %s\n", mId, vs.c_str(), fs.c_str() );

	    bLoaded = true;
	    bActive = true;      
	
	   get(); 

	}


  inline void ShaderProgram::unload(){

		if ( vert.id() ){
			glDetachShader( mId, vert.id() );		
	        vert.unload();
		}

		if (frag.id()){
			glDetachShader(mId,frag.id());
	        frag.unload();
		}

		if (mId){
			glDeleteShader(mId);
			mId = -1;
		}
		 GL::error("Shader Unload");
	}

	//---------------------------------------------------------------

	inline void ShaderProgram::bind() const {
		if ( bLoaded == true){
			if (bActive){
				glUseProgram(mId);
			} else {
				glUseProgram(0);
			}
	    GL::error("Shader Begin");
		}
	}

	inline void ShaderProgram::unbind() const{
	    glUseProgram(0);
	    GL::error("Shader End");

	}
	//---------------------------------------------------------------
	inline void ShaderProgram::setUniformVariable (char * name, float value){
		if (bLoaded){
			glUniform1f( glGetUniformLocation( mId, name), value );
		}
	    GL::error("Shader Set Uniform");

	}

	inline void ShaderProgram::setTexture(char * name, int id){

		if(bLoaded){

			int x = glGetUniformLocation(mId, name);
			glUniform1i(x, id);

		}

	    GL::error("Shader Set Texture");
	}

  /*-----------------------------------------------------------------------------
  *  FUNCION TO CHECK FOR SHADER LINK ERRORS
  *-----------------------------------------------------------------------------*/
  inline void ShaderProgram::linkCheck(GLuint ID){
        GLint linkStatus, validateStatus;
        glGetProgramiv(ID, GL_LINK_STATUS, &linkStatus);
            

        if(linkStatus == GL_FALSE ){
            cout << "Shader Linking FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages),0,&messages[0]);
            cout << messages;
        }

        glValidateProgram(ID);
        glGetProgramiv(ID, GL_VALIDATE_STATUS, &validateStatus);
          
        cout << "Link: " << linkStatus << "  Validate: " << validateStatus << endl; 
        if(linkStatus == GL_FALSE ){
            cout << "Shader Validation FAILED" << endl;
            GLchar messages[256];
            glGetProgramInfoLog(ID, sizeof(messages),0,&messages[0]);
            cout << messages;
        }

      }


/*-----------------------------------------------------------------------------
 *  GET SHADER UNIFORMS AND ATTRIBUTES
 *-----------------------------------------------------------------------------*/
	inline void ShaderProgram::get() {

      printf("ShaderProgram::get()\n");

	    GLuint program = id();

	    GLint numActiveUniforms = 0;
	    GLint numActiveAttributes = 0;
			GLint linkStatus = 0;
			GLint validateStatus =0;

	    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
	    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttributes);
			
			cout << "num uniforms: " << numActiveUniforms << endl;
			cout << "num attributes: " << numActiveAttributes << endl;

      for(int j=0; j < numActiveUniforms; j++)
      {

          Uniform u( program, j );
          mUniformMap[ string(u.name) ] = u;

         // cout << u.name << " " << uniform( u.name ) << endl; 
         // u.print();

      }

      for(int j=0; j < numActiveAttributes; j++) {

          Attribute a( program, j );
          mAttributeMap[ string(a.name) ] = a;

        //  cout << a.name << " " << attribute( a.name ) << endl; 
        //  a.print();

       }

	    }
 

/*-----------------------------------------------------------------------------
 *  PRINT SHADER SOURCE
 *-----------------------------------------------------------------------------*/
inline void ShaderProgram::printSource(){
 printf("Shader Program: %d:\n", mId);
 vert.print();
 frag.print();
 cout << endl;
}


/*-----------------------------------------------------------------------------
 *  PRINT SHADER UNIFORMS AND ATTRIBUTES
 *-----------------------------------------------------------------------------*/
inline void ShaderProgram::printVariables(){
  cout << "UNIFORMS" << endl;
  for (auto& i : mUniformMap) {
    cout << i.first << " " << i.second.id << endl;
    i.second.print();
  }
  cout << "ATTRIBUTES" << endl;
  for (auto& i : mAttributeMap) {
    cout << i.first << " " << i.second.id << endl;
    i.second.print();
  }
}

 
/*-----------------------------------------------------------------------------
 *  Not Currently Used
 *-----------------------------------------------------------------------------*/
     struct ShaderData {
     	struct Paramf {
            Paramf(const std::string& n, float v ) : name(n), val(v)  {}
            std::string name;
            float val;
        };
        
        struct Parami {
            Parami(const std::string& n, float v ) : name(n), val(v) {}
            std::string name;
            int val;
        };
        
        struct ParamMat4f{
            ParamMat4f(const std::string& n, float v[16]) : name(n) {
                copy(v, v+16, val);
                //cout << "Mat4f created" << val[0] << endl;
            }
            std::string name;
            float val[16];
        };
        
        std::vector<ParamMat4f> uniformMat4f;
        std::vector<Paramf> uniformf;
        std::vector<Parami> uniformi;
        std::vector<Paramf> attributef;
        std::vector<Parami> attributei;
        
        void addUniformMat4f(const std::string& n, float v[16]) { 
            uniformMat4f.push_back(ParamMat4f(n, v)); 
        }
        void addUniformf(const std::string& n, float v) { uniformf.push_back(Paramf(n,v)); }
        void addUniformi(const std::string& n, int v) { uniformi.push_back(Parami(n,v)); }
        void addAttributef(const std::string& n, float v) { attributef.push_back(Paramf(n,v)); }
        void addAttributei(const std::string& n, int v) { attributei.push_back(Parami(n,v)); }
        
        void uniform(const std::string& n, float v) {
            for (int i=0; i<uniformf.size(); i++) {
                if (uniformf[i].name == n) {
                    uniformf[i].val = v;
                }
            }
        }
        void uniform(const std::string& n, int v) {
            for (int i=0; i<uniformi.size(); i++) {
                if (uniformi[i].name == n) uniformi[i].val = v;
            }
        }
        
        void update(ShaderProgram * shaderprogram){
            
            for (int i = 0 ; i < uniformf.size(); ++i){
                shaderprogram -> uniform( uniformf[i].name.c_str(), uniformf[i].val );
            }
            
            for (int i = 0 ; i < uniformi.size(); ++i){
                shaderprogram -> uniform( uniformi[i].name.c_str(), uniformi[i].val );
            }
            
            for (int i = 0 ; i < uniformMat4f.size(); ++i){
                shaderprogram -> uniform( uniformMat4f[i].name.c_str(), uniformMat4f[i].val );
            }
        }
        
    };
  

inline void Shader::load(string file, Shader::Type type ){
  
}

} //gfx::

#endif
