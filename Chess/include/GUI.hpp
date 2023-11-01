#pragma once

#include <SDL.h>

#include <string>

#include "RenderWindow.hpp"
#include "Entity.hpp"

class GUI
{
public:
	GUI(RenderWindow& p_window);
	void draw();
	void setCheckMateStatus(bool p_checkMate) { checkMate = p_checkMate; }
private:
	RenderWindow& window;

	bool checkMate = false;

	SDL_Color gray = { 0xa6, 0xa5, 0xa2, 0xff };
	SDL_Color green = { 7, 168, 34, 0xff };

	Entity text;
};