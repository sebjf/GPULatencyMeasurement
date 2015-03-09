//============================================================================
// Name        : GPULatencyMeasurement.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include "Mouse.hpp"

using namespace std;

Mouse* mouse;
float mousex;
float mousey;

class Texture
{
public:
	Texture(uint32_t colour)
	{
		 glGenTextures(1, &m_texName);
		 Initialise(colour);
	}

	GLuint m_texName;
	GLsizei m_width, m_height;
	void* m_data;

	void Initialise(uint32_t colour)
	{
		Bind();

		m_width = 1024;
		m_height = 1024;

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		m_data = malloc(sizeof(uint32_t) * m_width * m_height);
		for(int i = 0; i < m_width * m_height; i++){
			((uint32_t*)m_data)[i] = colour;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
	}

	void Bind()
	{
		 glBindTexture(GL_TEXTURE_2D, m_texName);
	}
};

class TexturedSquare
{
public:
	TexturedSquare(float size, uint32_t colour)
	{
		m_texture = new Texture(colour);
		m_size = size;
	}

	Texture* m_texture;
	float m_size;

	void Render()
	{
		Render(0.0f,0.0f);
	}

	void Render(float x, float y)
	{
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		m_texture->Bind();

		glBegin(GL_QUADS);

		float size = m_size/2;

		glTexCoord2f(0.0, 0.0); glVertex3f(x-size, y-size, 0.0f); // The bottom left corner
		glTexCoord2f(0.0, 1.0); glVertex3f(x-size, y+size, 0.0f); // The top left corner
		glTexCoord2f(1.0, 1.0); glVertex3f(x+size, y+size, 0.0f); // The top right corner
		glTexCoord2f(1.0, 0.0); glVertex3f(x+size, y-size, 0.0f); // The bottom right corner

		glEnd();
		glFlush();
		glDisable(GL_TEXTURE_2D);
	}
};

TexturedSquare* background;
TexturedSquare* cursor;
TexturedSquare* target;

void draw()
{
	//clear the background
    glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //code for rendering here

	//render the background map and target
	background->Render();
	target->Render(-1.0f, -1.0f);

	//render the cursor
	MouseState s = mouse->readDevice();
	cursor->Render(s.x,s.y);


    glutSwapBuffers();   // swapping image buffer for double buffering
    glutPostRedisplay(); // redrawing. Omit this line if you don't want constant redraw
}

int main(int argc, char** argv)
{
	mouse = new Mouse(false);
	mouse->Scale = 0.010f;

	/* Prepare the conditions for this measurement */

	bool b_vsyncenabled = true;
	bool b_singlebuffered = false;

	unsigned int displayModeFlags = GLUT_RGBA;
	if(b_singlebuffered){
		displayModeFlags |= GLUT_SINGLE;
	}else{
		displayModeFlags |= GLUT_DOUBLE;
	}

	/* Initialise the device and display */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // enabling double buffering and RGBA
    glutInitWindowSize(600, 600);
    glutCreateWindow("OpenGL"); // creating the window
    glutFullScreen();           // making the window full screen
    PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress( (const GLubyte*)"glXSwapIntervalSGI");
    glXSwapIntervalSGI((int)b_vsyncenabled);

    glDisable(GL_LIGHTING);

    /* Configure texture mapping */

    background = new TexturedSquare(2, 0xFF000020);
    target = new TexturedSquare(0.4, 0xFF200000);
    cursor = new TexturedSquare(0.25f, 0xFFFFFFFF);

    /* Start the main loop */

    glutDisplayFunc(draw);      // draw is your function for redrawing the screen

    glutMainLoop();

    return 0;
}
