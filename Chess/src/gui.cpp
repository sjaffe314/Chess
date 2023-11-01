#include "GUI.hpp"

GUI::GUI(RenderWindow& p_window) : window(p_window) 
{
	text.updateTex(window.loadFont("Checkmate", green));
	text.updatePos({ (window.getWidth() - text.getWidth()) / 2, 30 });
}

void GUI::draw()
{
	if (checkMate)	window.render(text);
}
