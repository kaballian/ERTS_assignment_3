#pragma once
#include <iostream>
#include "Singleton.h"
class Initializing
{
	friend class Singleton<Initializing>;
public:
	bool startInitializing();
	Initializing() = default;
	~Initializing() = default;

	Initializing(const Initializing&);
	Initializing& operator=(const Initializing&) = delete;

};

