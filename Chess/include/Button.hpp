#pragma once

#include <functional>

#include "Entity.hpp"
#include "RenderWindow.hpp"

class Button : public Entity
{
public:
	Button(const char* p_text, Vector2i p_pos, Vector2i p_dims, uint8_t p_eventCode);
	bool isHovered(Vector2i p_mousePos);
	void press();
	bool unpress(Vector2i p_mousePos);

	uint8_t getEventCode() { return eventCode; }
private:
	uint32_t primaryColor = 0x02610dff;
	uint32_t secondaryColor = 0x02a114ff;

	uint8_t eventCode = 0;
	bool isPressed = false;
};