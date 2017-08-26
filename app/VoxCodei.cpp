// VoxCodei.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <deque>

using namespace std;


class Node {
public:
	int initX, initY;
	int bound1X, bound1Y;//left/top bound
	int bound2X, bound2Y;//right/bottom bound

	void computePosOnFrame(int frameNum) {};
	void renderToMap() {};


};

typedef std::deque<Node> nodelist_t;

class CellType {

public:
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
	EType operator-(const EType& r) {
		return EType(static_cast<int>(r) - static_cast<int>(m_eType));   
	}
	bool operator==(const EType& type)
	{
		return (type == m_eType);
	};

	bool operator!=(const EType& type)
	{
		return (type != m_eType);
	};

	EType GetType() const
	{
		return m_eType;
	};
	void SetType(EType type)
	{
		m_eType=type;
	};

	void DecreaseTypeValue()
	{
		m_eType = static_cast<EType>(m_eType - 1);
	}


	EType m_eType;
	Node* pNode;
};



typedef std::vector<CellType> row_t;
typedef std::vector<row_t> grid_t;
typedef std::pair<int, int> node_t;
typedef std::list<node_t> nodes_list_t;

class GameField {
public:
	int width, height;

};


struct Possibility
{
	int x, y; //coordinates of the point
	int roundActed;
	int roundPlaced;//
	int scoreAsCount;
	int scoreWithRange;
	int scoreWithSingles;
	list<Node*> affectedNodesList;//?
	
};

class Detection {
public:
	Detection(int width, int height) {};
	~Detection() {};

	grid_t detectionFrames[3];

	void setFrame(grid_t& frame, int step) {};

};


template<typename T>
vector<T> splice(vector<T>& v, int start, int howmuch) {
	vector<T> result(begin(v) + start, begin(v) +  howmuch);
	v.erase(begin(v) + start, begin(v)  + howmuch);
	return result;
}

void clone(grid_t& grid, grid_t& newgrid)
{
	std::copy(grid.begin(), grid.end(), back_inserter(newgrid));
}
int countNodes(grid_t& grid)
{
	int count=0;
	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			if (grid[i][j] == CellType::ET_NODE)
			{
				count++;
			}
		}
	}

	return count;
}

void dump_map(const grid_t& m)
{
	for (size_t i = 0; i < m.size(); i++)
	{
		for (size_t j = 0; j < m[i].size(); j++)
		{
			std::cerr << static_cast<char>(m[i][j].GetType());
		}
		std::cerr << std::endl;
	}
}

/**
* Decrement countdown for future exploded nodes
* @param {grid_t} grid containing nodes and walls
*/
void purgeGrid(grid_t& grid) {
	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			if(grid[i][j] == CellType::BOMB_1  || grid[i][j] == CellType::BOMB_2 ||  grid[i][j] == CellType::BOMB_3)
			{
				grid[i][j].DecreaseTypeValue();
				grid[i][j].SetType((grid[i][j] == CellType::BOMB_0) ? CellType::ET_EMPTY : grid[i][j].GetType());
			}
		}
	}

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
	width = 8; height = 6;



	grid_t grid(height);

   string list[] = { 
		"........",
		"......@.",
		"@@@.@@@@",
		"......@.",
		"........",
		"........",
	};
	//debug


	for (int i = 0; i < height; i++) {
		string mapRow; // one line of the firewall grid
		//cin >> mapRow; cin.ignore();
		mapRow = list[i];
		grid[i].resize(width);
		for (int j = 0; j < width; j++)
		{
			grid[i][j].SetType(static_cast<CellType::EType>(mapRow[j]));
		}
		
	}
	dump_map(grid);
	
	vector<Possibility>* seq = NULL;
	// game loop
	int rounds = 15;
	int bombs = 3;
	while (rounds > -1) {
		//int rounds; // number of rounds left before the end of the game
		//int bombs; // number of bombs left
		//cin >> rounds >> bombs; cin.ignore();
		//cerr << "rounds " << rounds << "bombs " << bombs << endl;

		purgeGrid(grid);
		std::cout << "WAIT" << std::endl;
	}
	rounds--;
		
}