#ifndef SNAKESCORE_H
#define SNAKESCORE_H

#include <vector>
#include <utility>
#include <string>

class Score
{
	public:
		Score(size_t size, const char* f);
		std::pair<int, std::string> getPair(size_t place);
		void changePair(size_t place, std::pair<int, std::string> oof);
		void storeVector();
		void loadVector();
		int rankScore(int score);
		void putScore(int score, std::string name, int place);
		size_t size;
	private:
		std::vector < std::pair < int, std::string > > scoreArray;
		size_t trueSize;		
		const char* file;		
};

#endif
