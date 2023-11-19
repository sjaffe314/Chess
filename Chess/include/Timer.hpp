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
	void set(uint32_t duration) { time = 1000 * duration; stop(); loadTime(); }
	void add(uint32_t duration) { time += 1000 * duration; loadTime(); }
	void remove(uint32_t duration) { time = (time < 1000 * duration) ? 0 : time - 1000 * duration; loadTime(); }
	void activate();
	void deactivate();
	void toggleInUse();
	void toggleActive();
	void toggleRunning();
	void updateTime();
	void start();
	void stop();

	bool timeHasRunOut();
	bool isActive() { return active; }
	bool isInUse() { return inUse; }
	bool isRunning() { return running; }

	static std::string csToClock(uint32_t p_time);
private:
	static const uint32_t lightColor = 0xffffffff;
	static const uint32_t darkColor = 0x262421ff;
	static const uint32_t warningColor = 0xc90627ff;

	uint32_t time = 600000;
	uint32_t lastUpdate = 0;
	bool inUse = false;
	bool active = false;
	bool running = false;
	
	RenderWindow& window;
};