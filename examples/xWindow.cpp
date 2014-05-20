/*
 * =====================================================================================
 *
 *       Filename:  xWindow.cpp
 *
 *    Description:  windowing with glfw
 *
 *        Version:  1.0
 *        Created:  01/28/2014 17:07:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), wolftype (gmail)
 *   Organization:  Media Arts and Technology Program, UCSB
 *
 * =====================================================================================
 */

// INCLUDE the WINDOW Wrapper Class
#include "util/GLFWWindowWrapper.h"

int main(){

  if( !glfwInit() ) return 0;

  //Window window(640,480, "A window") ;

  printf("hello window\n");
  
  /* while ( !window.shouldClose() ){ */
  /*   window.clear(1,1,1,1);      //<-- CLEAR WINDOW CONTENTS and SET COLOR TO WHITE */      
  /*   window.swapBuffers();              //<-- SWAP BUFFERS */
  /*   glfwPollEvents();           //<-- LISTEN FOR WINDOW EVENTS */
  /* } */
  
  printf("goodbye window\n");

  return 0;
}

