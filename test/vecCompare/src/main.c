#include <stdio.h>
#include <stdlib.h>
#include <chipmunk.h>
#include <time.h>

int main( int argc, const char* argv[] )
{	
    const int ITEM_COUNT = 1000; 
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

    start = clock();
	
    //calc
    for(j = 0; j < ITEM_COUNT; j++)
    {
        for(i = 0; i < ITEM_COUNT; i++)
        {
            cpVect result = cpvadd(a[i], b[i]);
        }
    }
	
    end = clock();

    printf("done in: %d ms.\n", (end-start)/ (CLOCKS_PER_SEC / 1000));

    scanf_s("%d\n", &i);

    return 0;
}