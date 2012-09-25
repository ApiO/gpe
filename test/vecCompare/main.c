#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "chipmunk.h"
#include <time.h>

int main( int argc, const char* argv[] )
{	
	const int ITEM_COUNT = 50000; 
	int i, j;
	clock_t start, end;
	
	cpVect a[ITEM_COUNT], b[ITEM_COUNT];

	//init
	for(i = 0; i < ITEM_COUNT; i++)
	{
		a[i] = cpVect();
		a[i].x = rand();
		a[i].y = rand();

		b[i] = cpVect();
		b[i].x = rand();
		b[i].y = rand();
	}

	start = clock() / (CLOCKS_PER_SEC / 1000);

	
	//calc
	for(i = 0; i < ITEM_COUNT; i++)
	{
		cpVect result = cpvadd(a[i], b[i]);
	}
	
	end = clock() / (CLOCKS_PER_SEC / 1000);
	printf("done in: %f ms.\n", end-start);

	scanf_s("%d\n", &i);

	return 0;
}