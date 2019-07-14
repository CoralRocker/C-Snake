#include <stdio.h>
#include "Score.h"

void printScore(ScoreArray s)
{
	for(int i = 0; i < s.size; i++)
	{
		printf("%d: %s -- %dpts\n", i, s.arr[i].name, s.arr[i].score);
	}
}

int main()
{
	ScoreArray s = initScoreArray("Snake.bin", 0);
	readScore(&s);
	printf("Read Array: \n");
	printScore(s);

	printf("\nAdding John, score 20:\n");
	insertScore(&s, 0, 20, "John", 4);
	printScore(s);
	storeScore(&s);
	
	//freeScoreArray(&s);

	ScoreArray p = initScoreArray("Snake.bin", 0);
	readScore(&p);
	printf("\nReread Array: \n");
	printScore(p);
	freeScoreArray(&p);
}
