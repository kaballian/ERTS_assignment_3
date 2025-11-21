#pragma once
#include "Singleton.h"
#include <iostream>
class Failure
{
	friend class Singleton<Failure>;
public:
	void display(int ErrorNo) { std::cout << "Failed: error#:" << ErrorNo << std::endl; }
	Failure() = default;
	~Failure() = default;

	Failure(const Failure&) = delete;
	Failure& operator=(const Failure&) = delete;
};

