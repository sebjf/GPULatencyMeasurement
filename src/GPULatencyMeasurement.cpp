//============================================================================
// Name        : GPULatencyMeasurement.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <GL/glut.h>
#include "Mouse.hpp"

using namespace std;

Mouse* mouse;
float mousex;
float mousey;

void renderSquare(float x, float y, float size)
{
	glBegin(GL_QUADS);

	size = size/2;

	glVertex3f(x-size, y-size, 0.0f); // The bottom left corner
	glVertex3f(x-size, y+size, 0.0f); // The top left corner
	glVertex3f(x+size, y+size, 0.0f); // The top right corner
	glVertex3f(x+size, y-size, 0.0f); // The bottom right corner

	glEnd();
}

void draw()
{
    // code for rendering here
	MouseDelta s = mouse->readMouse(false);

	renderSquare(s.x,s.y,0.5);

	if(s.changed())
	{
		cout << s.x << "  " << s.y <<  endl;
	}


    glutSwapBuffers();   // swapping image buffer for double buffering
    glutPostRedisplay(); // redrawing. Omit this line if you don't want constant redraw
}

int main(int argc, char** argv)
{
	mouse = new Mouse(false);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // enabling double buffering and RGBA
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL"); // creating the window
 // glutFullScreen();           // making the window full screen
    glutDisplayFunc(draw);      // draw is your function for redrawing the screen

    glutMainLoop();

    return 0;
}
