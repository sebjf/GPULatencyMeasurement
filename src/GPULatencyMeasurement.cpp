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

/*
 * xorg.conf section for the Asus VG248QE showing modeline settings for driving it at 144Hz. Note that nvidia-settings
 * must be used to specify these timings before they are used.
 */
/*
 Section "Monitor"
    Identifier     "Monitor1"
    VendorName     "Unknown"
    ModelName      "Ancor Communications Inc VG248"
#    HorizSync       30.0 - 160.0
#    VertRefresh     50.0 - 150.0

#http://ubuntuforums.org/showthread.php?t=1994729&page=2

    Option "ModeValidation" "NoEdidModes"
    Option "ExactModeTimingsDVI" "TRUE"
    Modeline "1280x720x144" 143.5  1280 1304 1336 1360   720 723 728 733 +HSync -VSync

EndSection

 */

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
    glFinish();
}

int main(int argc, char** argv)
{
	mouse = new Mouse(false);
	mouse->Scale = 0.010f;

	/* Prepare the conditions for this measurement */

	bool b_vsyncenabled = true;
	bool b_doublebuffered = true;

	if (argc > 0)
	{
		switch(atoi(argv[1]))
		{
		case 1:
			b_vsyncenabled = true;
			b_doublebuffered = true;
			break;
		case 2:
			b_vsyncenabled = false;
			b_doublebuffered = true;
			break;
		case 3:
			b_vsyncenabled = true;
			b_doublebuffered = false;
			break;
		case 4:
			b_vsyncenabled = true;
			b_doublebuffered = false;
			break;
		}
	}

	unsigned int displayModeFlags = GLUT_RGBA;
	if(b_doublebuffered){
		displayModeFlags |= GLUT_DOUBLE;
	}else{
		displayModeFlags |= GLUT_SINGLE;
	}

	unsigned int swap_interval = 0;
	if(b_vsyncenabled > 0)
	{
		swap_interval = 1;
	}

	/* Initialise the device and display */

    glutInit(&argc, argv);
    glutInitDisplayMode(displayModeFlags); // enabling double buffering and RGBA
    glutInitWindowSize(1280, 720);
    glutCreateWindow("OpenGL"); // creating the window
    glutFullScreen();           // making the window full screen
    PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress( (const GLubyte*)"glXSwapIntervalSGI");
    glXSwapIntervalSGI(swap_interval);

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
