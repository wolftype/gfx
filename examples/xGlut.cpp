/*
 * =====================================================================================
 *
 *       Filename:  xWindow.cpp
 *
 *    Description:  Make a basic glut window from scratch and draw a 2D triangle in it
 *
 *        Version:  1.0
 *        Created:  06/09/2014 15:58:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 * =====================================================================================
 */

//Include OpenGL for graphics and Glut for windowing
#include "gfx_lib.h"

float width, height;

static void reshape(int w, int h){
  width  = w; 
  height = h;
}

// animate is a loop that is called by glutTimerFunc
static void animate(int){    
    //1. Update The Display
    glutPostRedisplay();
    //2. Swap the Buffers
    glutSwapBuffers();
    //3. Call this function again . . .
    glutTimerFunc(10, animate, 1);
}


static void draw(void){

    //1. Set view port size and position
    glViewport(0,0,width,height);
    //2. Set the Clear Screen Background Color To Red
    glClearColor(1,0,0,1);
    //3. Clear the Screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //4. Transform The orthogonal Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/height,width/height, -1.f, 1.f, 1.f, -1.f);

    //5. Transform ModelView Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //6. Draw a Triangle!
    glBegin(GL_TRIANGLES);
     
      glColor3f(1,0,0); 
      glVertex3f(-1,-.5,0);
      glColor3f(0,1,0); 
      glVertex3f(0,1,0);
      glColor3f(0,0,1); 
      glVertex3f(1,-.5,0);
      
    glEnd();

    //4. Swap the Data Buffer
    glutSwapBuffers();
}



int main(int argc, char * argv[] ){
   
    //1. Initialize Glut
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //2. Create A Window
    glutCreateWindow("glut");
    //3. Specify a Reshape Function (for whenever window is resized)
    glutReshapeFunc(reshape);
    //4. Reshape the window (calls the reshape function)
    glutReshapeWindow(640, 480);

    //3. Specify a Display Function (for whenever glutPostRedisplay is called)
    glutDisplayFunc(draw); 
    //4. Begin Animation Timer          
    glutTimerFunc(10,animate,1);
 
    //4. Start The Main Loop
    glutMainLoop();  

    return 0;
}
