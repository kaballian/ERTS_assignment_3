#pragma once
#include <iostream>
#include "Singleton.h"
#include "State.h"
class PowerOnSelfTest : public State
{
	friend class Singleton<PowerOnSelfTest>;
public:
	State* handleEvent(Event e) override
	{
		switch (e)
		{
		case Event::SelfTestOk:
			return new Initi
		}
	}

	bool systemSelfTest();
	PowerOnSelfTest() = default;
	~PowerOnSelfTest() = default;

	PowerOnSelfTest(const PowerOnSelfTest&) = delete;
	PowerOnSelfTest& operator=(const PowerOnSelfTest&) = delete;

	
};

