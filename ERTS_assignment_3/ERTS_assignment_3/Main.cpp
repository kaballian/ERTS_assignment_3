#include <iostream>
#include "PowerOnSelfTest.h"
#include "Singleton.h"




int main(void)
{
	std::cout << "begin" << std::endl;

	PowerOnSelfTest* test1 = Singleton<PowerOnSelfTest>::Instance();
	test1->systemSelfTest();

	PowerOnSelfTest* test2 = Singleton<PowerOnSelfTest>::Instance();
	test2->systemSelfTest();

	if (test1 == test2)
	{
		std::cout << " test1:" << test1 << " test2:" << test2 << std::endl;
	}

	
	

	return 1;

}