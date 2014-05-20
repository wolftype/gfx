/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/28/2014 16:36:59
 *         Author:  Pablo Colapinto (), wolftype (gmail)
 *         Adapted from GLFW quick start tutorial
 *   Organization: lynda
 *
 * =====================================================================================
 */

#include <iostream>
#include <stdlib.h>
#include <vector>

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"

using namespace std;

float lr,ud;

struct Mouse {
    float x, y;
    bool isDown;
    float lx, ly;
};

struct Interface {

  static Mouse mouse;

  static void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
          glfwSetWindowShouldClose(window, GL_TRUE);

       if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || GLFW_PRESS) )
          lr += 10;
       if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || GLFW_PRESS) )
          lr -= 10;
       if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || GLFW_PRESS ))
          ud += 10;
       if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || GLFW_PRESS) )
          ud -= 10;


  }

  static void OnMouseMove(GLFWwindow* window, double x, double y){
    
      mouse.x = x ; mouse.y = y;

      cout << mouse.x << " " << mouse.y << endl; 
  
  }

  static void OnMouseDown(GLFWwindow* window, int button, int action, int mods){

      mouse.lx = mouse.x; mouse.ly = mouse.y;
      if (action == GLFW_PRESS) { cout << "button pressed" << endl; mouse.isDown = true; }
      if (action ==GLFW_RELEASE) { cout << "button released" << endl; mouse.isDown = false; }
  }


 static void PollEvents(){
    glfwPollEvents();
  }
};

 struct Mouse Interface::mouse = {0,0};


struct Window {

    GLFWwindow * window;
    int width, height;
    float ratio;

    static void OnResize(GLFWwindow * window, int w, int h){
      glfwSetWindowSize(window, w,h);
    }

    Window(int w, int h) {

        window = glfwCreateWindow(w,h,"lynda",NULL,NULL);
        if (!window) {
          glfwTerminate();
          exit(EXIT_FAILURE);
        }        
        glfwMakeContextCurrent(window);
        glViewport(0,0,w,h);

        //callbacks
        glfwSetKeyCallback(window, Interface::OnKeyDown);
        glfwSetWindowSizeCallback(window, Window::OnResize);
        glfwSetCursorPosCallback(window, Interface::OnMouseMove );
        glfwSetMouseButtonCallback(window, Interface::OnMouseDown );
    }

//glfwGetFramebufferSize(window, &width, &height);
    
    void getSize(){
        glfwGetFramebufferSize(window, &width, &height);
        ratio = (float) width / height;      
    }

    void setViewport(){ 
        getSize(); 
        //printf("%d, %d, %f\n", width, height, ratio);    
        glViewport(0,0,width,height);  
    }

    bool shouldClose(){
      return glfwWindowShouldClose(window);
    }
    void swap(){
      glfwSwapBuffers(window);
    }
    void destroy(){
      glfwDestroyWindow(window);
    }
    void clear(float r = 0.0, float g =0.0, float b=0.0,float a= 1.0){
      glClearColor(r,g,b,a);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    ~Window(){
      destroy();
    }
};


struct Vec3{ 
  float x, y, z; 
  void print(){ printf("%f, %f, %f\n", x,y,z); }
};
struct Vec4{ 
  float r, g, b, a; 
   void print(){ printf("%f, %f, %f, %f\n", r,g,b,a); }

};

struct Vertex{
  Vec3 position;
  Vec4 color;

  void print(){
    position.print();
    color.print();
  }
};

struct Mesh{
  vector<Vertex> vertex;
  vector<int> index;

  GLenum mode;

  Mesh& add( const Vertex& v){
    vertex.push_back(v);
    return *this;
  }

  Mesh& add( int idx ){
    index.push_back(idx);
    return *this;
  }

  void draw(){
    glBegin(GL_TRIANGLE_STRIP);
      for (int i = 0; i < vertex.size(); ++i){
        glColor4f( vertex[i].color.r, vertex[i].color.g, vertex[i].color.b, vertex[i].color.a );
        glVertex3f( vertex[i].position.x, vertex[i].position.y, vertex[i].position.z );
      }
    glEnd();
  }

  void drawElements(){
    glBegin( mode );
      for (int i = 0; i < index.size(); ++i){
        Vertex& v = vertex[index[i]];
        glColor4f( v.color.r, v.color.g, v.color.b, v.color.a );
        glVertex3f( v.position.x, v.position.y, v.position.z );
      }
    glEnd();
  }

  void print(){
    for (int i = 0; i < vertex.size(); ++i){
      vertex[i].print();
    }
  }
};


int main(){


  if (!glfwInit()) return(0);

  Window window(640,480) ;

  float time = 0.0;
  while( !window.shouldClose() ){  
    time += .01;

    window.setViewport();
    window.clear();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

   // glOrtho(-window.ratio, window.ratio, -1.f, 1.f, 1.f, -1.f);
    gluPerspective( 60, window.ratio,.1, 100);


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity(); 
    
    gluLookAt( 
        0,0,5,    //Eye position (positive is TOWARDS you)
        0, 0, -1, //Direction we are looking in (negative is into the screen)
        0, 1, 0); //Which way is Up?      
    
    //glScalef(.2,.2,.2);
    glRotatef( Interface::mouse.x,0,1,0);
   // glRotatef( lr, 0, 1, 0);
    glRotatef( Interface::mouse.y, 1, 0, 0);

    Mesh m;
    m.mode = GL_QUADS;
    
    Vertex a = { {-1,-1,-1}, {1,0,0,1} }; 
    Vertex b = { {-1,1,-1},  {1,1,0,1} };
    Vertex c = { {1,1,-1},   {1,0,0,1} };
    Vertex d = { {1,-1,-1},  {1,0,1,1} };
    
    Vertex e = { {-1,-1,1}, {0,0,1,1} };
    Vertex f = { {-1,1,1},  {1,0,1,1} };
    Vertex g = { {1,1,1},   {0,.3,1,1} };
    Vertex h = { {1,-1,1},  {.2,0,1,1} };

    m.add( a );
    m.add( b );
    m.add( c );
    m.add( d );
    m.add( e );
    m.add( f );
    m.add( g );
    m.add( h );

    m.add(0).add(1).add(2).add(3);
    m.add(3).add(2).add(6).add(7);
    m.add(7).add(6).add(5).add(4);
    m.add(4).add(5).add(1).add(0);
    m.add(1).add(5).add(6).add(2);
    m.add(4).add(0).add(3).add(7);
    

    glPolygonMode( GL_BACK, GL_LINE );
    m.drawElements();  

    window.swap(); 
    Interface::PollEvents();
  
  
  }

  printf("hello world\n");

  glfwTerminate();
  return 0;
}

