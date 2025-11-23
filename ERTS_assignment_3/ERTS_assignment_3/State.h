#pragma once
#include "Event.h"
#include "StateMachine.h"

class State
{
public:
	virtual State* handleEvent(Event e) = 0;
	virtual void onEntry() {};
	virtual void onExit() {};
	virtual ~State() = default;

};

