#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structs */
/* Struct to hold a specific score */
typedef struct Score {
	char * name; // Pointer holding name
	uint8_t nameSize; // Size of name
	uint16_t score;
} Score;

/* Struct to hold full array of scores */
typedef struct ScoreArray {
	Score *arr;
	uint8_t size;
	char *fileName;
} ScoreArray;

/* Creates a score array. All Scores inside array are initialized to NULL. Uses malloc, so
 * memory must be freed before end of program.
 */
ScoreArray initScoreArray(char* fileName, uint8_t size)
{
	//size = (size > 0) ? size : 1;
	ScoreArray s = {
		(Score*)malloc(sizeof(Score)*size),
		size,
		fileName
	};
	for(uint8_t i = 0; i < size; i++)
	{
		s.arr[i].name = 0x00;
		s.arr[i].nameSize = 0;
		s.arr[i].score = 0;
	}
	return s;
}

/* If array is created with initScoreArray(), this method frees the allocated memory. */
void freeScoreArray(ScoreArray *SA)
{
	free(SA->arr);
}

/* Stores the scores in a binary file. The name of the file  is given when initializing the ScoreArray. */
void storeScore(ScoreArray* SA)
{
	FILE *f = fopen(SA->fileName, "wb");
	fwrite(&SA->size, sizeof(uint8_t), 1, f);
	freeScoreArray(SA);
	SA->arr = (Score*)malloc(sizeof(Score)*SA->size);
	for(uint8_t i = 0; i < SA->size; i++)
	{
		fwrite(&SA->arr[i].nameSize, sizeof(uint8_t), 1, f);
		fwrite(&SA->arr[i].name, sizeof(const char) * SA->arr[i].nameSize, 1, f);
		fwrite(&SA->arr[i].score, sizeof(uint16_t), 1, f);
	}
	fclose(f);
}

/* Reads the scores from a binary file. The name of the file is given when initializing the ScoreArray */
void readScore(ScoreArray *SA)
{
	FILE *f = fopen(SA->fileName, "rb");
	fread(&SA->size, sizeof(uint8_t), 1, f);
	freeScoreArray(SA);
	SA->arr = (Score*)malloc(sizeof(Score)*SA->size);
	for(uint8_t i = 0; i < SA->size; i++)
	{
		fread(&SA->arr[i].nameSize, sizeof(uint8_t), 1, f);
		fread(&SA->arr[i].name, sizeof(const char) * SA->arr[i].nameSize, 1, f);
		fread(&SA->arr[i].score, sizeof(uint16_t), 1, f);
	}
	fclose(f);
}
