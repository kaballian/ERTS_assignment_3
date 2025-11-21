#pragma once
#include <iostream>


template<typename T>
class Singleton
{
public:
	static T* Instance() 
	{
		if(_instance == nullptr){
			_instance = new T;
		}
		return _instance;
	}
protected:
	Singleton() = default;
	~Singleton() = default;
	
	Singleton(const Singleton&)				= delete;
	Singleton& operator=(const Singleton&)	= delete;

private:
	static T* _instance;
};


template <typename T>
T* Singleton<T>::_instance = nullptr;
