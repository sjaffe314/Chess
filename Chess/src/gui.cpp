#include "GUI.hpp"

GUI::GUI() : window(RenderWindow::get())
{
	initializeEntities();
	
	whiteTimer.activate();
}

void GUI::initializeEntities()
{
	buttons.reserve(6);
	
	Vector2i buttonDims = { window.getScale() * 5 / 2, window.getScale() / 2 };
	Vector2i buttonPos = { (window.getWidth() - 26 * window.getScale() / 3) / 4 - buttonDims.x / 2, window.getHeight() / 2 - 4 * window.getScale() };
	//buttonPos.y = 100;
	buttons.emplace_back("New Game", buttonPos, buttonDims, NewGame);
	buttonPos.y += buttonDims.y * 3 / 2;
	buttons.emplace_back("Quit", buttonPos, buttonDims, Quit);
	buttonPos.y += buttonDims.y * 3 / 2;
	buttons.emplace_back("Toggle Timers", buttonPos, buttonDims, ToggleTimers);
	buttonPos.y += buttonDims.y * 3 / 2;
	buttons.emplace_back("Reset Timers", buttonPos, buttonDims, ResetTimers);
	buttonPos.y += buttonDims.y * 3 / 2;
	buttons.emplace_back("+1 Minute", buttonPos, buttonDims, AddTime);
	buttonPos.y += buttonDims.y * 3 / 2;
	buttons.emplace_back("-1 Minute", buttonPos, buttonDims, RemoveTime);


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
	
	if (whiteTimer.isRunning())
	{
		whiteTimer.updateTime();
		if (whiteTimer.timeHasRunOut()) queueEvent(WhiteTimeout);
	}
	if (blackTimer.isRunning())
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
			switch (button.getEventCode())
			{
			case ToggleTimers:
				if (whiteTimer.isRunning() || blackTimer.isRunning())
				{
					whiteTimer.stop();
					blackTimer.stop();
				}
				else
				{
					if (whiteTimer.isActive()) whiteTimer.start();
					if (blackTimer.isActive()) blackTimer.start();
				}
				break;
			case ResetTimers:
				whiteTimer.set(clockTime);
				blackTimer.set(clockTime);
				break;
			case AddTime:
				whiteTimer.add(60);
				blackTimer.add(60);
				clockTime += 60;
				break;
			case RemoveTime:
				whiteTimer.add(-60);
				blackTimer.add(-60);
				clockTime -= 60;
				break;
			default:
				queueEvent(button.getEventCode());
			}
		}
	}
}

void GUI::resetClocks()
{
	whiteTimer.set(clockTime);
	blackTimer.set(clockTime);
}

void GUI::toggleTurn()
{
	if (whiteTimer.isRunning() || blackTimer.isRunning())
	{
		whiteTimer.toggleRunning();
		blackTimer.toggleRunning();
	}
	else
	{
		whiteTimer.toggleActive();
		blackTimer.toggleActive();
	}
}