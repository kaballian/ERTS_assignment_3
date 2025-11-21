#pragma once

enum class Event
{
	SelfTestOk,
	SelfTestFailed,
	Error,
	Initialized,
	Restart,
	Exit
};