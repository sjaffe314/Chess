#pragma once

#include <sstream>

#include "RenderWindow.hpp"
#include "Entity.hpp"

class Timer : public DoubleEntity
{
public:
	Timer(Vector2i p_pos, Vector2i p_dims, uint32_t duration);
	Timer() : window(RenderWindow::get()) { alignment = FloatRight; }
	void init(Vector2i p_pos, Vector2i p_dims, uint32_t duration);
	void loadTime();
	void reset(uint32_t duration);
	void activate();
	void deactivate();
	void toggleActive();
	void updateTime();

	bool timeHasRunOut();
	bool isActive() { return active; }

	static std::string csToClock(uint32_t p_time);
private:
	static const uint32_t lightColor = 0xffffffff;
	static const uint32_t darkColor = 0x262421ff;

	uint32_t time = 600;
	uint32_t lastUpdate = 0;
	bool active = false;
	
	RenderWindow& window;
};