#include "Timer.hpp"

//RenderWindow& Timer::window = RenderWindow::get();

Timer::Timer(Vector2i p_pos, Vector2i p_dims, uint32_t duration)
	: window(RenderWindow::get()), time(duration * 1000)
{
	alignment = FloatRight;
	setBackground(window.createTwoColoredBox(p_dims, p_dims.y / 6, Timer::darkColor, Timer::lightColor), 2);
	loadTime();
	setPos(p_pos);
}

void Timer::init(Vector2i p_pos, Vector2i p_dims, uint32_t duration)
{
	time = 1000 * duration;
	setBackground(window.createTwoColoredBox(p_dims, p_dims.y / 6, Timer::darkColor, Timer::lightColor), 2);
	loadTime();
	setPos(p_pos);
}

void Timer::loadTime()
{
	setForeground(window.loadText(Timer::csToClock(time).c_str(), (active) ? Timer::darkColor : Timer::lightColor));
}

void Timer::reset(uint32_t duration)
{
	time = duration * 1000;
	deactivate();
}

void Timer::activate()
{
	active = true;
	lastUpdate = SDL_GetTicks();
	setBackgroundFrame(1);
	loadTime();
}

void Timer::deactivate()
{
	active = false;
	setBackgroundFrame(0);
	loadTime();
}

void Timer::toggleActive()
{
	if (active) deactivate();
	else activate();
}

void Timer::updateTime()
{
	uint32_t newTime = time - (SDL_GetTicks() - lastUpdate);
	if (newTime <= 0) time = 0;
	bool needRerender = newTime < 20000 || newTime / 1000 % 10 == time / 1000 % 10;
	if (time != 0) time = newTime;
	if (needRerender) loadTime();
	lastUpdate = SDL_GetTicks();
}

bool Timer::timeHasRunOut()
{
	return time == 0;
}

std::string Timer::csToClock(uint32_t p_time)
{
	std::stringstream ss;
	ss << p_time / 60000 << ":" << p_time / 10000 % 6 << p_time / 1000 % 10;
	if (p_time < 20000)
	{
		ss << "." << p_time / 100 % 10;
	}
	return ss.str();
}
