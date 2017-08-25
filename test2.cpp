// test2.cpp : Defines the entry point for the console application.
//

//testing file for figuring directions for node from main frame and two successive frames

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <fstream>

using namespace std;

enum EType
{
	ET_EMPTY = '.',
	ET_NODE = '@',
	ET_PASSIVE = '#',
	BOMB_3 = 3,
	BOMB_2 = 2,
	BOMB_1 = 1,
	BOMB_0 = 0,
};

enum Directions
{
	UP=0,
	RIGHT,
	DOWN,
	LEFT,
};



typedef std::vector<EType> row_t;
typedef std::vector<row_t> grid_t;


void formPattern(int x, int y, int width, int height, vector<vector<int>>& pattern, std::vector<std::vector<int>>& vectorpattern) {

	vectorpattern[RIGHT].clear();
	for (int dx = 0; dx < 3 && x + dx < width; dx++)
	{
		vectorpattern[RIGHT].push_back(pattern[y][x + dx]); ;
	}
	vectorpattern[LEFT].clear();
	for (int dx = 0; dx < 3 && x - dx >= 0; dx++)
	{
		vectorpattern[LEFT].push_back(pattern[y][x - dx]);
	}
	vectorpattern[DOWN].clear();
	for (int dy = 0; dy < 3 && y + dy < height; dy++)
	{
		vectorpattern[DOWN].push_back(pattern[y+dy][x]);
	}
	vectorpattern[UP].clear();
	for (int dy = 0; dy < 3 && y - dy >= 0; dy++)
	{
		vectorpattern[UP].push_back(pattern[y-dy][x]);
	}
};

const std::vector<int> pattern111 = { 1,1,1 };
const std::vector<int> pattern21 = { 2,1 };
const std::vector<int> pattern210 = { 2,1,0 };

int figurePattern(std::vector<std::vector<int>>&  vectorpatterns)
{
	int found = -1;
	for (int i = 0; i < 4; i++)
	{
		if (vectorpatterns[i] == pattern111 || vectorpatterns[i] == pattern21 || vectorpatterns[i] == pattern210)
		{
			found = i;
			break;
			//to node - i is a direction of initial node movement
		}
	}
	if (found == -1)
	{
		//error - pattern is not found
		bool stop = true;
	}
	return found;
}


/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	int width; // width of the firewall grid
	int height; // height of the firewall grid
				//cin >> width >> height; cin.ignore();
	width = 12; height = 9;

	

	vector<vector<int>> pattern(height);

	grid_t grid1(height);
	grid_t grid2(height);
	grid_t grid3(height);

//maps from step 8 of https://www.codingame.com/ide/puzzle/vox-codei-episode-2
	string lstgrid1[] =
	{   
		"........@...",
		".......@....",
		".#.#.#@#.#.#",
		".....@......",
		"#.#.@#..#.#.",
		"...@........",
		"..@.........",
		".@..........",
		"@...........",
	};

	string lstgrid2[] =
	{   
		".......@....",
		"........@...",
		".#.#.#.#.#.#",
		"....@.@.....",
		"#.#..#..#.#.",
		"..@.........",
		"...@........",
		"............",
		".@..........",
	};
	string lstgrid3[] =
	{   
		"......@.....",
		".........@..",
		".#.#@#.#.#.#",
		".......@....",
		"#.#..#@.#.#.",
		".@..........",
		"....@.......",
		".@..........",
		"..@.........",
	};


//init patterns map
	for (int i = 0; i < height; i++) {//y
		pattern[i].resize(width);
	}
	
//fill in three initial maps 
	for (int i = 0; i < height; i++) {//y
		string mapRow; // one line of the firewall grid
					  
		mapRow = lstgrid1[i];
		grid1[i].resize(width);
		for (int j = 0; j < width; j++)//x
		{
			grid1[i][j] = static_cast<EType>(mapRow[j]);
			
		}

	}

	for (int i = 0; i < height; i++) {
		string mapRow; // one line of the firewall grid
					   
		mapRow = lstgrid2[i];
		grid2[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			grid2[i][j] = static_cast<EType>(mapRow[j]);
		}

	}

	for (int i = 0; i < height; i++) {
		string mapRow; // one line of the firewall grid
					   
		mapRow = lstgrid3[i];
		grid3[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			grid3[i][j] = static_cast<EType>(mapRow[j]);
		}

	}

	//fill in pattern map

	for (int y = 0; y < height; y++)//
	{
		for (int x = 0; x < width; x++)//first wee look at horizontal line
		{
			if (grid1[y][x] == ET_NODE) {
				pattern[y][x]++;
			}
			//now we found Node position if in three steps it was the same - then it is definetely static.
			if (grid2[y][x] == ET_NODE)
			{
				pattern[y][x]++;
			}
			if (grid3[y][x] == ET_NODE)
			{
				pattern[y][x]++;
			}

		}

	}

//output pattern	

	std::fstream of("Map.txt", std::ios::out | std::ios::trunc);

	if (of.is_open())
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				of << pattern[y][x] << " ";
			}
			of << "\n";
		}
		of.close();
	}
	
//figure directions for for particular node	
	std::vector<std::vector<int>> vectorpatterns{ {}/*UP*/, {} /*RIGHT*/, {} /*DOWN*/, {}/*LEFT*/ };
	int direction;
	for (int y = 0; y < height; y++)//
	{
		for (int x = 0; x < width; x++)//first wee look at horizontal line
		{
			if (grid1[y][x] == ET_NODE) {

				formPattern(x, y, width, height, pattern, vectorpatterns);
				//TODO - retrieve assosiated node from GameField
				//figure pattern, pass to node
				direction = figurePattern(vectorpatterns);//pass direction to corresponding node 
				if (direction == -1)
				{
					cerr << "no direction found for" << y << " " << x << endl;
				}
				
			}
		}

	}

}

//TODO - statics == 3 