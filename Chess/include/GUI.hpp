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
private:
	RenderWindow& window;

	SDL_Color gray = { 0xa6, 0xa5, 0xa2, 0xff };
	Entity text;
};