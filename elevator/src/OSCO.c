#include <climits>
#define _CRT_RAND_S


int random_number(int max_num)
{
	unsigned int number = 0, out = 0;

	rand_s(&number);
	out = (unsigned int)((double)number / ((double)UINT_MAX + 1) * max_num) + 1;
	return out;
}

int rand_between(int min_num, int max_num)
{
	unsigned int number=0;
	int result = 0, low_num = 0, hi_num = 0;
	if (min_num < max_num)
	{
		low_num = min_num;
		hi_num = max_num +1; // include max_num in output
	}
	else {
		low_num = max_num +1; // include max_num in output
		hi_num = min_num;
	}


	rand_s(&number);
	result = (unsigned int)(((double)number / ((double)UINT_MAX + 1)) * (hi_num - low_num)+1 ) + low_num ;
	
	return result;
}