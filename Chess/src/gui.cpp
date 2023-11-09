#include "GUI.hpp"

GUI::GUI() : window(RenderWindow::get())
{
	text.updateTex(window.loadFont("Checkmate", green));
	text.updatePos({ (window.getWidth() - text.getWidth()) / 2, 30 });
}

void GUI::draw()
{
	if (checkMate)	window.render(text);
}
