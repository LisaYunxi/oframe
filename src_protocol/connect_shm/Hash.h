#include <iostream>

class ShmHash
{
public:
	
	ShmHash(){	
	};

	unsigned int JSHash(char *str)
	{

		unsigned int hash = 1315423911; // nearly a prime - 1315423911 = 3 * 438474637

		while (*str)

		{	

			hash ^= ((hash << 5) + (*str++) + (hash >> 2));

		}

		return (hash & 0x7FFFFFFF);

	};

};