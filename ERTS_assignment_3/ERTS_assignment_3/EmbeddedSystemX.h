#pragma once
#include "Singleton.h"
#include "Failure.h"
#include "Initializing.h"
#include "PowerOnSelfTest.h"


class EmbeddedSystemX
{

private:
	enum class Event
	{
		SelfTestOk,
		SelfTestFailed,
		Restart,
		Initialized

	};

};

