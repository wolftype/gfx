/*
 * =====================================================================================
 *
 *       Filename:  xGlutTimer.cpp
 *
 *    Description:  Glut Window with a Timer Callback for better Frames Per Second Control
 *
 *        Version:  1.0
 *        Created:  06/09/2014 16:21:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */

#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <GLUT/GLUT.h>

#include <stdio.h>


static void draw(void){

    //1. Set view port size and position
    glViewport(0,0,640,480);
    //2. Set the Clear Screen Background Color To Red
    glClearColor(1,0,0,1);
    //3. Clear the Screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // DRAW CODE HERE ....
    //
    // ...
    printf("drawing\n");
}

static void animate(int){
    
    //
    glutPostRedisplay();

    // Swap the Buffers
    glutSwapBuffers();

    // Call this function again
    glutTimerFunc(10, animate, 1);
}


int main(int argc, char * argv[] ){
   
    //1. Initialize Glut
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //2. Create A Window
    glutCreateWindow("glut");

    //3. Bind Display Callback
    glutDisplayFunc(draw);           
    //glutIdleFunc(draw); <-- Notice we no longer use IdleFunc
    glutTimerFunc(10,animate,1);
 
    //4. Start Main Loop
    glutMainLoop();  

    return 0;
}
