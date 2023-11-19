#pragma once

#include <SDL.h>

#include <string>
#include <functional>

#include "Subsystem.hpp"
#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Button.hpp"
#include "Timer.hpp"

namespace GUIEvents {
	enum EventCodes : uint8_t
	{
		None,
		NewGame,
		StopGame,
		ContinueGame,
		Quit,
		AddTime,
		RemoveTime,
		ResetTimers,
		ToggleTimers,
		StartStopTime
	};
}

namespace TextIndicies {
	enum Indices : uint8_t
	{
		Checkmate,
		Stalemate,
		TimeRanOut,
		WhiteWins,
		BlackWins, 
		Tied
	};
}

class GUI : public Subsystem
{
public:
	GUI();
	void draw();
	void update(Vector2i p_mousePos);
	void onMouseDown(Vector2i p_mousePos);
	void onMouseUp(Vector2i p_mousePos);

	void resetClocks();
	void clearTexts() { textsToDisplay.clear(); }
	void toggleTurn();
	void show(uint8_t code) { textsToDisplay.push_back(code); }
private:
	void initializeEntities();
	
	RenderWindow& window;

	int clockTime = 2;

	uint32_t gray = 0xa6a5a2ff;
	uint32_t green = 0x07a822ff;

	std::vector<Button> buttons;
	std::vector<Entity> texts;
	std::vector<uint8_t> textsToDisplay;
	Timer whiteTimer;
	Timer blackTimer;
};