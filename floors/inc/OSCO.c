#define UINT_MAX 0xffffffff
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

void rand_str(char* s, const int len) {
	char* alphanum = "abcdefghijklmnopqrstuvwxyz123456789";

	for (int i = 0; i < len; ++i) {
		int num = random_number(strlen(alphanum) - 1);
		s[i] = alphanum[num];
	}

	s[len] = 0;
}

int nexp(double lambda) {
	double newIAT;
	double rnd;

	rnd = (double)(rand() % 1000) / 1000; //rnd element of [0, 1]
	newIAT = -1 * (log(rnd) / lambda); // calculation of newIAT (-log(rnd/lambda), in microseconds

	return (int)newIAT;
}