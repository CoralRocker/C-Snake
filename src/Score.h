#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Score {
	char * name;
	uint8_t nameSize;
	uint16_t score;
} Score;

typedef struct ScoreArray {
	Score *arr;
	uint8_t size;
	char *fileName;
} ScoreArray;

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

void freeScoreArray(ScoreArray *SA)
{
	free(SA->arr);
}

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
