#pragma once

#include <vector>

class Subsystem
{
public:
	uint8_t pollEvent()
	{
		if (eventQueue.empty()) return 0;
		uint8_t eventCode = eventQueue.back();
		eventQueue.pop_back();
		return eventCode;
	}
protected:
	void queueEvent(uint8_t action) { eventQueue.push_back(action); }

	std::vector<uint8_t> eventQueue;
};