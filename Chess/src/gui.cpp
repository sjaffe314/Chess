#include "GUI.hpp"

GUI::GUI(RenderWindow& p_window) : window(p_window) 
{
	text.updateVals({10, 10}, window.loadFont("New Game", gray));
}

void GUI::draw()
{
	window.render(text);
}
