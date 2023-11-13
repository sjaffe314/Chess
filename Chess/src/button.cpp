#include "Button.hpp"

Button::Button(const char* p_text, Vector2i p_pos, Vector2i p_dims, uint8_t p_eventCode)
	: Entity(p_pos, p_dims), eventCode(p_eventCode)
{
	tex = RenderWindow::get().createButtonTexture(p_dims, 10, primaryColor, secondaryColor, p_text, 0xa6a5a2ff);
}

bool Button::isHovered(Vector2i p_mousePos)
{
	if (p_mousePos.x > pos.x && p_mousePos.x < pos.x + currentFrame.w && p_mousePos.y > pos.y && p_mousePos.y < pos.y + currentFrame.h) {
		if (!isPressed) currentFrame.x = currentFrame.w;
		return true;
	}
	if (!isPressed) currentFrame.x = 0;
	return false;
}

void Button::press()
{
	isPressed = true;
	currentFrame.x = 2 * currentFrame.w;
}

bool Button::unpress(Vector2i p_mousePos)
{
	if (isPressed)
	{
		isPressed = false;
		return isHovered(p_mousePos);  // only returns true if the button is still hovered
	}
	return false;
}
