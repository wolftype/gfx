/*
 * =====================================================================================
 *
 *       Filename:  glfw_window.h
 *
 *    Description:  a window wrapper around glfwWindow
 *
 *        Version:  1.0
 *        Created:  02/07/2014 11:45:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto 
 *   Organization: 
 *
 * =====================================================================================
 */

#ifndef GLFW_WINDOW_WRAPPER_INCLUDED
#define GLFW_WINDOW_WRAPPER_INCLUDED

#define GLFW_INCLUDE_GLU
#include "GLFW/glfw3.h"
#include <iostream>


using namespace std;

struct Mouse {
    float x, y;
    bool isDown;
    float lx, ly;
    float dx, dy;
};

struct Keyboard{
  bool alt, shift, ctrl;
  int key;
  void reset(){
    alt = shift = ctrl = false;
  }
};

struct GLFWInterface {

  static int LR, UD;

  static Mouse mouse;
  static Keyboard keyboard;

  static void OnKeyDown(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
       keyboard.reset();
       keyboard.key = key;

       if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
          glfwSetWindowShouldClose(window, GL_TRUE);

       if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || GLFW_PRESS) )
          LR += 10;
       if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || GLFW_PRESS) )
          LR -= 10;
       if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || GLFW_PRESS ))
          UD += 10;
       if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || GLFW_PRESS) )
          UD -= 10;
       if ( (key == GLFW_KEY_LEFT_ALT||GLFW_KEY_RIGHT_ALT) && ( action == GLFW_REPEAT||GLFW_PRESS ) )
          keyboard.alt = true;
       if ( (key == GLFW_KEY_LEFT_SHIFT||GLFW_KEY_RIGHT_SHIFT) && ( action == GLFW_REPEAT||GLFW_PRESS ) )
          keyboard.shift = true;
       if ( (key == GLFW_KEY_LEFT_CONTROL||GLFW_KEY_RIGHT_CONTROL)&&(action==GLFW_REPEAT||GLFW_PRESS) )
          keyboard.ctrl = true;

  }

  static void OnMouseMove(GLFWwindow* window, double x, double y){
    
      mouse.x = x ; mouse.y = y;

      if (mouse.isDown == true ){
        mouse.dx = mouse.x - mouse.lx; mouse.dy = mouse.y - mouse.ly;
      }
  
  }

  static void OnMouseDown(GLFWwindow* window, int button, int action, int mods){

      if (action == GLFW_PRESS) { 
          cout << "button pressed" << endl;
          mouse.lx = mouse.x; mouse.ly = mouse.y;
          mouse.isDown = true; 
      }

      if (action == GLFW_RELEASE) { cout << "button released" << endl; mouse.isDown = false; }
  }

   static void PollEvents(){
      glfwPollEvents();
    }
};


struct Mouse GLFWInterface::mouse = {0,0};
struct Keyboard GLFWInterface::keyboard = {0,0,0,0};

int GLFWInterface::LR = 0;
int GLFWInterface::UD = 0;


struct Window {

    GLFWwindow * window;
    //GLFWInterface interface;
    int mWidth, mHeight;
    float ratio;

    static void OnResize(GLFWwindow * window, int w, int h){
      glfwSetWindowSize(window, w,h);
    }

    Window(int w, int h, const char * name="demo") {

        mWidth = w; mHeight =h;
        window = glfwCreateWindow(w,h,name,NULL,NULL);
        if (!window) {
          glfwTerminate();
          exit(EXIT_FAILURE);
        }        
        glfwMakeContextCurrent(window);
        glViewport(0,0,w,h);

        //callbacks
        /* glfwSetKeyCallback(window, GLFWInterface::OnKeyDown); */
        /* glfwSetWindowSizeCallback(window, Window::OnResize); */
        /* glfwSetCursorPosCallback(window, GLFWInterface::OnMouseMove ); */
        /* glfwSetMouseButtonCallback(window, GLFWInterface::OnMouseDown ); */

        glEnable( GL_DEPTH_TEST );          //<-- Enable Z-depth sorting 
    
    }

    
    void getSize(){
        glfwGetFramebufferSize(window, &mWidth, &mHeight);
        ratio = (float) mWidth / mHeight;      
    }

    void setViewport(){ 
        getSize(); 
        //printf("%d, %d, %f\n", width, height, ratio);    
        glViewport(0,0,mWidth,mHeight);  
    }

    bool shouldClose(){
      return glfwWindowShouldClose(window);
    }
    void swapBuffers(){
      glfwSwapBuffers(window);
    }
    void destroy(){
      glfwDestroyWindow(window);
    }
    void pollEvents(){
      glfwPollEvents();
    }
    void clear(float r = 0.0, float g =0.0, float b=0.0,float a= 1.0){
      glClearColor(r,g,b,a);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    ~Window(){
      destroy();
    }
};

#endif

