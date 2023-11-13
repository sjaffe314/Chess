#include "GUI.hpp"

GUI::GUI() : window(RenderWindow::get())
{
	initializeEntities();
	
	whiteTimer.activate();
}

void GUI::initializeEntities()
{
	buttons.emplace_back("New Game", Vector2i{window.getWidth() / 2 - 8 * window.getScale(), window.getHeight() / 2 - 3 * window.getScale()}, Vector2i{window.getScale() * 5 / 2, window.getScale() / 2}, NewGame);

	Vector2i timerDims = { window.getScale() * 2, window.getScale() / 2 };
	Vector2i timerPos = Vector2i{ window.getWidth() / 2 + 13 * window.getScale() / 3, (window.getHeight() - 9 * window.getScale()) / 2 } - timerDims;
	blackTimer.init(timerPos, timerDims, clockTime);
	timerPos.y += 19 * window.getScale() / 2;
	whiteTimer.init(timerPos, timerDims, clockTime);

	text.setTexture(window.loadText("Checkmate", green));
	text.setPos({ (window.getWidth() - text.getWidth()) / 2, 30 });
}

void GUI::draw()
{
	for (Button& button : buttons)
	{
		window.render(button);
	}
	window.render(whiteTimer);
	window.render(blackTimer);
	if (checkMate) window.render(text);
}

void GUI::update(Vector2i p_mousePos)
{
	for (Button& button : buttons)
	{
		button.isHovered(p_mousePos);
	}
	
	if (whiteTimer.isActive())
	{
		whiteTimer.updateTime();
		if (whiteTimer.timeHasRunOut()) queueEvent(WhiteTimeout);
	}
	if (blackTimer.isActive())
	{
		blackTimer.updateTime();
		if (blackTimer.timeHasRunOut()) queueEvent(BlackTimeout);
	}
}

void GUI::onMouseDown(Vector2i p_mousePos)
{
	for (Button& button : buttons)
	{
		if (button.isHovered(p_mousePos))
		{
			button.press();
		}
	}
}

void GUI::onMouseUp(Vector2i p_mousePos)
{
	for (Button& button : buttons)
	{
		if (button.unpress(p_mousePos))
		{
			queueEvent(button.getEventCode());
		}
	}
}

void GUI::resetClocks()
{
	whiteTimer.reset(clockTime);
	blackTimer.reset(clockTime);
	whiteTimer.activate();
}

void GUI::toggleTurn()
{
	whiteTimer.toggleActive();
	blackTimer.toggleActive();
}