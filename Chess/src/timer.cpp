#include "Timer.hpp"

//RenderWindow& Timer::window = RenderWindow::get();

Timer::Timer(Vector2i p_pos, Vector2i p_dims, uint32_t duration)
	: window(RenderWindow::get())
{
	alignment = FloatRight;
	init(p_pos, p_dims, duration);
}

void Timer::init(Vector2i p_pos, Vector2i p_dims, uint32_t duration)
{
	time = 1000 * duration;
	setBackground(window.createThreeColoredBox(p_dims, p_dims.y / 6, Timer::darkColor, Timer::lightColor, Timer::warningColor), 3);
	loadTime();
	setPos(p_pos);
}

void Timer::loadTime()
{
	uint32_t color = Timer::lightColor;
	if (active) {
		if (time < 20000 && inUse) {
			setBackgroundFrame(2);
		}
		else {
			setBackgroundFrame(1);
			color = Timer::darkColor;
		}
	}
	else {
		setBackgroundFrame(0);
	}
	setForeground(window.loadText((inUse) ? Timer::csToClock(time).c_str() : "-:--", color));
}

void Timer::activate()
{
	active = true;
	loadTime();
}

void Timer::deactivate()
{
	active = false;
	loadTime();
}

void Timer::toggleInUse()
{
	inUse = !inUse;
	lastUpdate = SDL_GetTicks();
	loadTime();
}

void Timer::toggleActive()
{
	active = !active;
	lastUpdate = SDL_GetTicks();
	loadTime();
}

void Timer::toggleRunning()
{
	running = !running;
	lastUpdate = SDL_GetTicks();
}

void Timer::updateTime()
{
	uint32_t timeEllapsed = SDL_GetTicks() - lastUpdate;
	uint32_t newTime = time - timeEllapsed;
	if (timeEllapsed >= time) {
		newTime = 0;
		stop();
	}
	bool needRerender = newTime < 20000 || newTime / 1000 % 10 == time / 1000 % 10;
	time = newTime;
	if (needRerender) loadTime();
	lastUpdate = SDL_GetTicks();
}

void Timer::start()
{
	running = true;
}

void Timer::stop()
{
	running = false;
}

bool Timer::timeHasRunOut()
{
	return time == 0;
}

std::string Timer::csToClock(uint32_t p_time)
{
	std::stringstream ss;
	ss << p_time / 60000 << ":" << p_time / 10000 % 6 << p_time / 1000 % 10;
	if (p_time < 20000) {
		ss << "." << p_time / 100 % 10;
	}
	return ss.str();
}
