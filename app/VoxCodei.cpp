// VoxCodei.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <deque>
#include <memory>

using namespace std;


class Node {
public:
	Node(int _initX,int _initY):initX(_initX),initY(_initY)
	{};
	Node()
	{};

	enum NodeType
	{
		HORIZONAL = 0,
		VERTICAL,
		STATIC,
	};

	int initX, initY;
	int bound1X, bound1Y;//left/top bound
	int bound2X, bound2Y;//right/bottom bound
	NodeType m_type;

	void computePosOnFrame(int frameNum) {};
	void renderToMap() {};
	void setXY(int _initX, int _initY)
	{
		initX = _initX;
		initY = _initY;
	}



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
typedef std::vector<vector<int>> intgrid_t;
typedef std::vector<vector<Node*>> nodegrid_t;


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
	string* initListInt;
	grid_t grid[3];//detection frames
	intgrid_t patterngrid;
	intgrid_t passivenodesgrid;
	vector<bool> hasObstaclesInRow;//y - height -> row change
	vector<bool> hasObstaclesInColumns;//x - width -> column change
	nodegrid_t initNodeGrid;
	int width, height;
	const std::vector<int> pattern111 = { 1,1,1 };
	const std::vector<int> pattern21 = { 2,1 };
	const std::vector<int> pattern210 = { 2,1,0 };

	enum Directions
	{
		UP = 0,
		RIGHT,
		DOWN,
		LEFT,
	};


	Detection(int _width, int _height, vector<string>& initList)
	{
		width = _width;
		height = _height;

		grid[0].resize(height);
		grid[1].resize(height);
		grid[2].resize(height);
		hasObstaclesInRow.resize(height);
		hasObstaclesInColumns.resize(width);
		patterngrid.resize(height);
		passivenodesgrid.resize(height);
		initNodeGrid.resize(height);



		//width x height y
		//initListInt = initList;
		for (int y = 0; y < height; y++) {
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = initList[y];
			grid[0][y].resize(width);
			grid[1][y].resize(width);
			grid[2][y].resize(width);
			patterngrid[y].resize(width);
			passivenodesgrid[y].resize(width);
			initNodeGrid[y].resize(width);

			for (int x = 0; x < width; x++)
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);
				//grid[0][i][j].SetType(celltype);
				if (celltype == CellType::ET_NODE)
				{
					patterngrid[y][x]++;
					initNodeGrid[y][x]= new Node(x, y);
				}
				if (celltype == CellType::ET_PASSIVE)
				{
					passivenodesgrid[y][x]++;
					hasObstaclesInRow[y] = true;
					hasObstaclesInColumns[x] = true;
				}

			}

		}

	};
	//add second frame
	void addSecondFrame(vector<string>& initList)
	{
		for (int y = 0; y < height; y++)//
		{
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = initList[y];
			for (int x = 0; x < width; x++)//first wee look at horizontal line
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);
				if (celltype == CellType::ET_NODE)
				{
					patterngrid[y][x]++;
					
				}
			}

		}
	}
	void addThirdFrame(vector<string>& initList)
	{
		for (int y = 0; y < height; y++)//
		{
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = initList[y];
			for (int x = 0; x < width; x++)//first wee look at horizontal line
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);
				if (celltype == CellType::ET_NODE)
				{
					patterngrid[y][x]++;

				}
			}

		}
		//now have all frames
		std::vector<std::vector<int>> vectorpatterns{ {}/*UP*/,{} /*RIGHT*/,{} /*DOWN*/,{}/*LEFT*/ };
		int direction;
		for (int y = 0; y < height; y++)//
		{
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = initList[y];
			for (int x = 0; x < width; x++)//first wee look at horizontal line
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);
				if (celltype == CellType::ET_NODE) {

					formPattern(x, y, width, height,  vectorpatterns);
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

	void formPattern(int x, int y, int width, int height,  std::vector<std::vector<int>>& vectorpattern) 
	{

		vectorpattern[RIGHT].clear();
		for (int dx = 0; dx < 3 && x + dx < width; dx++)
		{
			vectorpattern[RIGHT].push_back(patterngrid[y][x + dx]); ;
		}
		vectorpattern[LEFT].clear();
		for (int dx = 0; dx < 3 && x - dx >= 0; dx++)
		{
			vectorpattern[LEFT].push_back(patterngrid[y][x - dx]);
		}
		vectorpattern[DOWN].clear();
		for (int dy = 0; dy < 3 && y + dy < height; dy++)
		{
			vectorpattern[DOWN].push_back(patterngrid[y + dy][x]);
		}
		vectorpattern[UP].clear();
		for (int dy = 0; dy < 3 && y - dy >= 0; dy++)
		{
			vectorpattern[UP].push_back(patterngrid[y - dy][x]);
		}
	};

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

	~Detection() 
	{
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++)
			{

				if (initNodeGrid[y][x] != NULL)
				{
					delete initNodeGrid[y][x];
				}


			}
		}

	};

	shared_ptr<GameField> createGameField()
	{
		//find nodes and their boundaries

	}

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
	
	//test with test 7 of Vox Codei
	width = 12;
	height = 9;
	
	vector<string> initList =
	{
		"........@...",
		".......@....",
		".#....@...#.",
		".....@......",
		"....@.......",
		"...@........",
		"..@.........",
		"#@.......#..",
		"@...........",
	};
	
	
	
	//create game field and make initialization
	//Detection detection(width, height, initList);

	shared_ptr<Detection> pDetection(nullptr);
	
	//dump_map(grid);
	//vector<string> initList;

	// game loop
	int simrounds = 1;
	int initrounds = 0;

	while (simrounds < 5) {
		/*
		int rounds; // number of rounds left before the end of the game
		int bombs; // number of bombs left
		cin >> rounds >> bombs; cin.ignore();
		cerr << "bombs " << bombs << endl;
		initList.clear();
		for (int i = 0; i < height; i++) {
			string mapRow; // one line of the firewall grid
			getline(cin, mapRow);
			//add mapRow to array
			initList.push_back(mapRow);
			cerr << "mapRow " << i << " " << mapRow << endl;
		}
		*/
		switch (simrounds)
		{
		case 1:
			//init
			//initrounds = rounds;
			pDetection.reset(new Detection(width, height, initList));
			break;
		case 2:
			initList = {
				".......@....",
				"........@...",
				".#........#.",
				"....@.@.....",
				"............",
				"..@.........",
				"...@........",
				"#.@......#..",
				".@..........",
			};
			//add second frame
			pDetection->addSecondFrame(initList);
			break;
		case 3:
			//add third frame, init all
			initList = {
				"......@.....",
				".........@..",
				".#..@.....#.",
				".......@....",
				"......@.....",
				".@..........",
				"....@.......",
				"#..@.....#..",
				"..@.........",
			};
			pDetection->addThirdFrame(initList);
			break;
		default:
			//simulate
			break;

		}
		//int rounds; // number of rounds left before the end of the game
		//int bombs; // number of bombs left
		//cin >> rounds >> bombs; cin.ignore();
		//cerr << "rounds " << rounds << "bombs " << bombs << endl;

		
		std::cout << "WAIT" << std::endl;
		simrounds++;
	}
	
		
}