#include "SnakeScore.hpp"
#include <cstdio>
#include <vector>
#include <utility>
#include <string>
#include <cstring>
#include <fstream>

Score::Score(size_t size, const char* f)
: file(f)
{
	this->size = size + 1;
	for(int i = 0; i < this->size; i++)
		this->scoreArray.push_back(std::pair(0, "NULL"));
	this->trueSize = scoreArray.size();
	//scoreArray = {{0, "NULL"},{0, "NULL"},{0, "NULL"}, {0, "NULL"}};
};

std::pair<int, std::string> Score::getPair(size_t place)
{
	return this->scoreArray[place];
}

void Score::changePair(size_t place, std::pair<int, std::string> oof)
{
	this->scoreArray[place] = oof;
}

void Score::storeVector()
{
	std::ofstream file(this->file, std::ios::binary);
			
	file.write(reinterpret_cast<const char*>(&trueSize), sizeof(trueSize));
	int tempSize;
	char *tempString;
	for(int i = 0; i < trueSize; i++)
	{
		file.write(reinterpret_cast<const char*>(&scoreArray[i].first), sizeof(scoreArray[i].first)); //Write Score to file
		tempSize = strlen(scoreArray[i].second.c_str());
		file.write(reinterpret_cast<const char*>(&tempSize), sizeof(tempSize)); //Write STRLEN to file
		file.write((const char*)scoreArray[i].second.c_str(), sizeof(const char) * tempSize);
	}
}

void Score::loadVector()
{
	std::vector<std::pair<int, std::string>> tempVector;
	std::ifstream file(this->file, std::ios::binary);
	if(file.is_open())
	{
		int bufSize;
		file.read(reinterpret_cast<char*>(&size), sizeof(size));
		std::pair<int, std::string> tempPair;
		std::string tempString;	
		int tempInt;
		char *buf;
		for(int i = 0; i < this->size; i++){
			file.read(reinterpret_cast<char*>(&tempInt), sizeof(int));
			file.read(reinterpret_cast<char*>(&bufSize), sizeof(int));
			buf = new char[bufSize];
			file.read(buf, bufSize);
			tempString = "";
			tempString.append(buf, bufSize);
			tempPair.first = tempInt;
			tempPair.second = tempString;
			tempVector.push_back(tempPair);
			delete(buf);
		}
		if(size > tempVector.size())
		{
			for(int i = tempVector.size(); i < size; i++)
			{
				tempVector.push_back(std::pair<int,std::string>(0, "NULL"));
			}
		}
		this->scoreArray = tempVector;
	}else{
		for(int i = 0; i < size; i++)
		{
			tempVector.push_back(std::pair<int,std::string>(0, "NULL"));
		}
		this->scoreArray = tempVector;
	}
}
	
int Score::rankScore(int score)
{
	for(int i = 1; i < size; i++)
	{
		if(score > scoreArray[i].first)		
			return i;

	}
	return -1;
}

void Score::putScore(int score, std::string name, int place)
{	
	for(int i = size - 1; i >= place; i--)
	{
		if(i != place)
		{
			this->scoreArray[i].first = scoreArray[i-1].first;
			this->scoreArray[i].second = scoreArray[i-1].second;
		}else{
			this->scoreArray[place].first = score;
			this->scoreArray[place].second = name;
		}
	}
}
