/*
 * mouse.cpp
 *
 *  Created on: 28 Jan 2014
 *      Author: sfriston
 */

#include "Mouse.hpp"
#include <iostream>
#include <poll.h>


Mouse::Mouse(bool invert_y)
:Scale(0.0833),
 InvertY(invert_y),
 xpos(0),
 ypos(0),
 m_mouse_cooldown(0)
{
	mouse = fopen("/dev/input/mouse1","rb");
	if(mouse == NULL){
		printf("Could not open mouse. Are you running with elevated permissions?\n");
	}
};

Mouse::~Mouse()
{
	fclose(mouse);
}

void Mouse::reset()
{
	xpos = 640 * (1/Scale);
	ypos = 360 * (1/Scale);
}

struct MouseState Mouse::readDevice()
{
	MouseDelta d = readMouse(false);

	xpos += d.x;

	if(InvertY){
		d.y = -d.y;
	}
	ypos += d.y;

	MouseState s;
	s.lmb = false;

	if(d.valid){
		bool mouse_down = d.lmb;
		if(mouse_down){
			if(!m_mouse_down){
				m_mouse_cooldown = 1000;
			}
		}
		m_mouse_down = mouse_down;
	}

	/* Since we filter the rising edge of the lmb state we need to give some play so when the
	 * delayed state is sampled, those just behind it will be picked up as well */
	if(m_mouse_cooldown > 0)
	{
		s.lmb = true;
		m_mouse_cooldown--;
	}

	s.x = (int)(xpos * Scale);
	s.y = (int)(ypos * Scale);


	return s;
}

struct MouseDelta Mouse::readMouse(bool block)
{
	struct MouseDelta value;
	value.x = 0;
	value.y = 0;
	value.lmb = false;
	value.valid = false;

	if(mouseInputAvailable() || block){

		struct MouseEvent event;
		int result = fread(&event,sizeof(struct MouseEvent),1,mouse);

		int x = 0;
		int y = 0;

		if(result == 1)
		{
			/*This works because even if the msb in word 1 is removed, the remainder of the counter
			 *is still 2's complement, so when the *signed* char is cast to an int, the integer is created in
			 *2's complement notation*/
			x = (int)event.x;
			y = (int)event.y;
			value.lmb = (event.flags & 0x1);

		//	printf("0x%x\n",event.flags);



			value.x = x;
			value.y = y;

			value.valid = true;
		}
	}

	return value;
}

bool Mouse::mouseInputAvailable()
{
	struct pollfd fds;
	fds.fd = fileno(mouse);
	fds.events = POLLIN;
	int timeout = 0;
	int ret = poll(&fds,1,timeout);
	if(ret > 0){
		if(fds.revents & POLLIN){
			return true;
		}
	}

	return false;
}
