#pragma once


/***************************************************
* Boolean implementation
*
****************************************************/
typedef enum {
	true = 1,
	false = 0	
} bool;

/***************************************************
* Generates a random number.
* returns the generated number
* int max_num: max of the interval [0..max]
****************************************************/
int random_number(int max_num);

/***************************************************
 Generates a random number.
 returns the generated number between min_num an max_num
 int min_num: min of the interval [min..max]
 int max_num: max of the interval [min..max]
****************************************************/
int rand_between(int min_num, int max_num);