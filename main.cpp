#include<iostream>
#include "Skiplist.h"

int main()
{
	for (int i = 0;i < 1;++i)
	{
		Skiplist skp;
		for (int i = 0;i < 10;++i)
		{
			skp.put(i, std::to_string(i));
		}
		skp.showSkipList();

		for (int j = 0;j < 10;++j)
		{
			std::string toget;
			skp.get(j, toget);
			std::cout << "get test:" << toget << std::endl;
			skp.del(j);

			skp.showSkipList();
			if (skp.get(j, toget))
			{
				std::cout << "false del:" << toget << std::endl;
			}
			else
			{
				std::cout << "success del:" << toget << std::endl;

			}
		}
		
	}
	
	return 0;
}