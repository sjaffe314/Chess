#pragma once

#include <SDL.h>

#include <string>
#include <functional>

#include "Subsystem.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Button.hpp"
#include "Timer.hpp"

enum EventCodes : uint8_t
{
	NewGame = 1,
	WhiteTimeout = 2,
	BlackTimeout = 3
};

class GUI : public Subsystem
{
public:
	GUI();
	void draw();
	void update(Vector2i p_mousePos);
	void onMouseDown(Vector2i p_mousePos);
	void onMouseUp(Vector2i p_mousePos);

	void resetClocks();
	void toggleTurn();
	void showCheckmate() { checkMate = true; }
private:
	void initializeEntities();
	
	RenderWindow& window;

	bool checkMate = false;
	int clockTime = 600;

	uint32_t gray = 0xa6a5a2ff;
	uint32_t green = 0x07a822ff;

	Entity text;
	std::vector<Button> buttons;
	Timer whiteTimer;
	Timer blackTimer;
};