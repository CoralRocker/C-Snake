#include <stdio.h>
#include "Score.h"

int main()
{
	ScoreArray s = initScoreArray("Snake.bin", 5);
	readScore(&s);
	for(int i = 0; i < s.size; i++)
	{
		printf("%d: %s %d\n", i, s.arr[i].name, s.arr[i].score);
	}
	
	insertScore(&s, rankScore(&s, 24), 24, "test", 4);
	for(int i = 0; i < s.size; i++)
		printf("%d: %s %d\n", i, s.arr[i].name, s.arr[i].score);
}	
