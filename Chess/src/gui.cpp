#include "GUI.hpp"

GUI::GUI() : window(RenderWindow::get())
{
	initializeEntities();
	
	whiteTimer.activate();
}

void GUI::initializeEntities()
{
	Vector2i dims, pos;
	
	buttons.reserve(7);
	
	dims = { window.getScale() * 5 / 2, window.getScale() / 2 };
	pos = { (window.getWidth() - 26 * window.getScale() / 3) / 4 - dims.x / 2, window.getHeight() / 2 - 4 * window.getScale() };
	buttons.emplace_back("New Game", pos, dims, GUIEvents::NewGame);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("Quit", pos, dims, GUIEvents::Quit);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("Toggle Timers", pos, dims, GUIEvents::ToggleTimers);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("Start/Stop", pos, dims, GUIEvents::StartStopTime);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("Reset Timers", pos, dims, GUIEvents::ResetTimers);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("+1 Minute", pos, dims, GUIEvents::AddTime);
	pos.y += dims.y * 3 / 2;
	buttons.emplace_back("-1 Minute", pos, dims, GUIEvents::RemoveTime);

	dims = { window.getScale() * 2, window.getScale() / 2 };
	pos = Vector2i{ window.getWidth() / 2 + 13 * window.getScale() / 3, (window.getHeight() - 9 * window.getScale()) / 2 } - dims;
	blackTimer.init(pos, dims, clockTime);
	pos.y += 19 * window.getScale() / 2;
	whiteTimer.init(pos, dims, clockTime);

	texts.reserve(6);
	pos = { window.getWidth() / 2, window.getHeight() / 2 - window.getScale() / 4 };
	texts.emplace_back(pos, window.loadText("Checkmate", green), true);
	texts.emplace_back(pos, window.loadText("Stalemate", green), true);
	texts.emplace_back(pos, window.loadText("Time Ran Out", green), true);
	pos.y += window.getScale() / 2;
	texts.emplace_back(pos, window.loadText("White Wins", green), true);
	texts.emplace_back(pos, window.loadText("Black Wins", green), true);
	texts.emplace_back(pos, window.loadText("Tied", green), true);

}

void GUI::draw()
{
	for (Button& button : buttons)
	{
		window.render(button);
	}
	window.render(whiteTimer);
	window.render(blackTimer);

	for (uint8_t ind : textsToDisplay) {
		window.render(texts[ind]);
	}
}

void GUI::update(Vector2i p_mousePos)
{
	for (Button& button : buttons)
	{
		button.isHovered(p_mousePos);
	}
	
	if (whiteTimer.isRunning() && whiteTimer.isActive())
	{
		whiteTimer.updateTime();
		if (whiteTimer.timeHasRunOut()) {
			blackTimer.stop();
			show(TextIndicies::BlackWins);
			show(TextIndicies::TimeRanOut);
			queueEvent(GUIEvents::StopGame);
		}
	}
	if (blackTimer.isRunning() && blackTimer.isActive())
	{
		blackTimer.updateTime();
		if (blackTimer.timeHasRunOut()) {
			whiteTimer.stop();
			show(TextIndicies::WhiteWins);
			show(TextIndicies::TimeRanOut);
			queueEvent(GUIEvents::StopGame);
		}
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
			case GUIEvents::ToggleTimers:
				whiteTimer.toggleInUse();
				blackTimer.toggleInUse();
				textsToDisplay.clear();
				queueEvent(GUIEvents::ContinueGame);
				break;
			case GUIEvents::ResetTimers:
				whiteTimer.set(clockTime);
				blackTimer.set(clockTime);
				textsToDisplay.clear();
				queueEvent(GUIEvents::ContinueGame);
				break;
			case GUIEvents::AddTime:
				whiteTimer.add(60);
				blackTimer.add(60);
				clockTime += 60;
				break;
			case GUIEvents::RemoveTime:
				whiteTimer.remove(60);
				blackTimer.remove(60);
				clockTime -= 60;
				break;
			case GUIEvents::StartStopTime:
				whiteTimer.toggleRunning();
				blackTimer.toggleRunning();
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
	whiteTimer.toggleActive();
	blackTimer.toggleActive();
}