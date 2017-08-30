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
#include <fstream>

using namespace std;

int pingpong(int time, int length)
{
	int L = 2 * length;
	int T = time % L;
	int x = L - T;

	if (T < length)
	{
		return T;
	}
	else
	{
		return L - T;
	}
}

enum Directions
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT,
	UNDECIDED,
};

class vec2d {
public:
	float x;
	float y;
	vec2d() :x(0), y(0)
	{};
	vec2d(int initX, int initY) : x(initX), y(initY)
	{};

	float& operator[] (size_t i)
	{
		switch (i) {
		case 0: return x;
		case 1: return y;
		//will not use other access! default: throw "something";
		}
	}

	float operator[] (size_t i) const
	{
		return (*const_cast<vec2d*>(this))[i];
	}
};

class Bomb {
public:
	Bomb(int _x, int _y, int frame) :x(_x), y(_y), tick(0), framePlaced(frame)
	{
		frameExplodes = framePlaced + 4;
	}
	int x, y;
	int framePlaced;
	int frameExplodes;//framePlaced+4 waits 3 steps then explodes
	int tick;
	void render();//pass into params.
};

class Node {
public:
	Node(int _initX,int _initY):initPos(_initX,_initY), curPos(_initX, _initY)
	{
		b_eliminated = false;
	};
	Node()
	{};

	Node(const Node& othernode)
	{
		m_type  = othernode.m_type;
		width   = othernode.width;
		initPos = othernode.initPos;
		curPos  = othernode.curPos;
		initRange = othernode.initRange;
		travelRange = othernode.travelRange;
		additionalTimefornode = othernode.additionalTimefornode;
	}

	enum NodeType
	{
		HORIZONTAL = 0,
		VERTICAL,
		STATIC,
		NOTDETECTED,
	};

	vec2d initPos;
	vec2d curPos;
	int additionalTimefornode;//fake 'time' to fit into pingpong function
	NodeType m_type;
	int width;
	int initRange;
	int travelRange;
	int pos;
	bool b_eliminated;
	

	void computePosOnFrame(int frameNum) {};
	void renderToMap() {};

	void initUndecided()
	{
		m_type = NOTDETECTED;
	}

	void initStatic()
	{
		m_type = STATIC;
	}

	void initBoundaries(NodeType type,int dir, int travelWidth, int boundary1 = -1, int boundary2 = -1)
	{
		m_type = type;
		if (boundary1 == -1 && boundary2 == -1)
		{//no boundaries

			width = travelWidth - 1;//0 -
			initRange = 0;
			if (dir == 1)//right bottom
			{
				additionalTimefornode = initPos[m_type];
			}
			else
			{//left top
				additionalTimefornode = 2 * width - initPos[m_type];
			}
		}
		if (boundary1 != -1 && boundary2 != -1)
		{

			travelRange = (boundary2 - 1) - (boundary1 + 1);
			width = travelRange;//travel to boundary but not on boundary -1?!
			initRange = boundary1 + 1;
			if (dir == -1)
			{
				additionalTimefornode = travelRange + (travelRange - (initPos[m_type] - (boundary1 + 1)));
			}
			else
			{//RIGHT
				additionalTimefornode = initPos[m_type] - (boundary1 + 1);
			}
		}
		if (boundary1 != -1 && boundary2 == -1)
		{
			travelRange = (travelWidth - 1) - (boundary1 + 1);
			width = travelRange;
			initRange = boundary1 + 1;
			if (dir == -1)
			{
				additionalTimefornode = travelRange + (travelRange - (initPos[m_type] - (boundary1 + 1)));
			}
			else
			{//RIGHT
				additionalTimefornode = initPos[m_type] - (boundary1 + 1);
			}
		}
		if (boundary1 == -1 && boundary2 != -1)
		{
			travelRange = boundary2 - 1;
			width = travelRange;
			initRange = 0;
			if (dir == -1)
			{
				additionalTimefornode = travelRange + (travelRange - initPos[m_type]);
			}
			else
			{//RIGHT bottom
				additionalTimefornode = initPos[m_type];
			}
		}


	}

	

	void computePosForFrame(int frame)
	{
		if (m_type == STATIC)
			return;

		pos = initRange + pingpong((frame - 1) + additionalTimefornode, width);
		curPos[m_type] = pos;
		//if (m_type == STATIC)//not changed after init.
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

typedef std::deque <shared_ptr<Node>> nodesdesque_t;

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

typedef std::deque <Possibility>  possibilitiesdeque_t;
typedef std::deque <Bomb>  bombsdeque_t;

class GameField {
public:
	int width, height;
	int numBombs;
	int rounds;
	int maxDepth;//adjust this by trial.
	bool simulate(int frame, nodesdesque_t& nodes, int bombs, bombsdeque_t& activebombs, possibilitiesdeque_t& seq, int depth)
	{
		int remains = nodes.size();
		if (remains == 0) {
			// no more nodes to blow
			return true;//seq;
		}
		if (bombs == 0 || frame>rounds) {
			// no more bombs or game finished
			return false;
		}
		if (depth > maxDepth)
		{
			return true;//compute now score.
		}
		// compute possible bomb locations
		vector<Possibility> possibles = {};
		nodesdesque_t nextNodesDeque;
		std::copy_if(nodes.cbegin(), nodes.cend(),
			std::back_inserter(nextNodesDeque),
			[](const shared_ptr<Node>& node) { return !node->b_eliminated; });
	
	};

};




class Detection {
public:
	string* initListInt;
	//grid_t grid[3];//detection frames
	intgrid_t patterngrid;
	intgrid_t passivenodesgrid;
	vector<bool> hasObstaclesInRow;//y - height -> row change
	vector<bool> hasObstaclesInColumn;//x - width -> column change
	nodegrid_t initNodeGrid;
	int width, height;
	const std::vector<int> pattern111 = { 1,1,1 };
	const std::vector<int> pattern21 = { 2,1 };
	const std::vector<int> pattern210 = { 2,1,0 };

	std::vector<string> firstFrameList;

	std::deque<Node*> nodesDeque;
	std::deque<Node*> undecidednodesDeque;

	


	Detection(int _width, int _height, vector<string>& initList)
	{
		width = _width;
		height = _height;

		hasObstaclesInRow.resize(height);
		hasObstaclesInColumn.resize(width);
		patterngrid.resize(height);
		passivenodesgrid.resize(height);
		initNodeGrid.resize(height);

		firstFrameList = initList;

		//width x height y
		//initListInt = initList;
		for (int y = 0; y < height; y++) {
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = initList[y];

			patterngrid[y].resize(width);
			passivenodesgrid[y].resize(width);
			initNodeGrid[y].resize(width);

			for (int x = 0; x < width; x++)
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);

				if (celltype == CellType::ET_NODE)
				{
					patterngrid[y][x]++;
					initNodeGrid[y][x]= new Node(x, y);
				}
				if (celltype == CellType::ET_PASSIVE)
				{
					passivenodesgrid[y][x]++;
					hasObstaclesInRow[y] = true;
					hasObstaclesInColumn[x] = true;
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
		std::fstream	of0("Map0.txt", std::ios::out | std::ios::trunc);

		if (of0.is_open())
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					of0 << patterngrid[y][x] << " ";
				}
				of0 << "\n";
			}
			of0.close();
		}
	}

	void addThirdFrame(vector<string>& initList)
	{
		//print map for first two steps



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

		};
	}

	void caseUpDown(int x,int y,int direction, Node* pNode)
	{
		//check if there are passive nodes in column
		if (hasObstaclesInColumn[x])
		{
			int topObstacle = -1;
			int bottomObstacle = -1;
			bool stop = true;
			//check left
			int i = y - 1;
			while (i > -1)
			{
				int topIndex = passivenodesgrid[i--][x];
				if (topIndex)
				{
					topObstacle = i + 1;
					break;
				}
			};
			i = y + 1;
			while (i < height)
			{
				int bottomIndex = passivenodesgrid[i++][x];
				if (bottomIndex)
				{
					bottomObstacle = i - 1;
					break;
				}
			};
			//if(leftObstacle==-1)//no left obstacle
			pNode->initBoundaries(Node::VERTICAL,(direction==UP)? -1:1, height, topObstacle, bottomObstacle);

		}
		else
		{
			pNode->initBoundaries(Node::VERTICAL, (direction == UP) ? -1:1, height);
		}
	}

	void caseLeftRight(int x, int y, int direction, Node* pNode)
	{
		if (hasObstaclesInRow[y])
		{
			int leftObstacle = -1;
			int rigthObstacle = -1;
			bool stop = true;
			//check left
			int i = x - 1;
			while (i > -1)
			{
				int leftIndex = passivenodesgrid[y][i--];
				if (leftIndex)
				{
					leftObstacle = i + 1;
					break;
				}
			};
			i = pNode->initPos.x + 1;
			while (i < width)
			{
				int rightIndex = passivenodesgrid[y][i++];
				if (rightIndex)
				{
					rigthObstacle = i - 1;
					break;
				}
			};
			pNode->initBoundaries(Node::HORIZONTAL, (direction == LEFT) ? -1 : 1, height, leftObstacle, rigthObstacle);
			//pNode->initHorizontalBoundaries(static_cast<Directions>(direction), width, leftObstacle, rigthObstacle);

		}
		else
		{
			pNode->initBoundaries(Node::HORIZONTAL, (direction == LEFT) ? -1 : 1, height);
			//pNode->initHorizontalBoundaries(static_cast<Directions>(direction), width);
		}

	}
	void analyseUndecided()
	{
		int direction;
		std::vector<std::vector<int>> vectorpatterns{ {}/*UP*/,{} /*RIGHT*/,{} /*DOWN*/,{}/*LEFT*/ };
		std::deque<Node*>::iterator iter = undecidednodesDeque.begin();
		while (iter != undecidednodesDeque.end())
		{
		
			Node* pNode = *iter;
			formPattern(pNode->initPos.x, pNode->initPos.y, width, height, vectorpatterns);
			direction = figurePattern(vectorpatterns);//pass direction to corresponding node

			switch (direction)
			{
			case UP:
			case DOWN:
				caseUpDown(pNode->initPos.x, pNode->initPos.y, direction, pNode);
				
				break;
			case LEFT:
			case RIGHT:
				caseLeftRight(pNode->initPos.x, pNode->initPos.y, direction, pNode);

				break;
			case UNDECIDED:
				++iter;
				continue;

			}

			if (!(pNode->m_type == Node::NOTDETECTED))
			{
				nodesDeque.push_back(pNode);
				iter = undecidednodesDeque.erase(iter);
				for (int i = 1; i <= 3; i++)
				{
					pNode->computePosForFrame(i);
					patterngrid[pNode->curPos.y][pNode->initPos.x] = 0;
				}
			}
		
		}
		
	}

	int analyseThirdFrame(vector<string>& initList)
	{

		//now have all frames, compute patterns relative to first frame
		//TODO as I detected misdetection - 1 st hardcode resolution, 2 nd - will streamline function so that the cirlce is run ok.
		std::vector<std::vector<int>> vectorpatterns{ {}/*UP*/,{} /*RIGHT*/,{} /*DOWN*/,{}/*LEFT*/ };
		int direction;
		for (int y = 0; y < height; y++)//
		{
			string mapRow; // one line of the firewall grid
						   //cin >> mapRow; cin.ignore();
			mapRow = firstFrameList[y];
			for (int x = 0; x < width; x++)//first wee look at horizontal line
			{
				CellType::EType celltype = static_cast<CellType::EType>(mapRow[x]);
				if (celltype == CellType::ET_NODE) {

					//1 test static
					if (patterngrid[y][x] == 3)
					{
						initNodeGrid[y][x]->initStatic();
					}
					else
					{

						formPattern(x, y, width, height, vectorpatterns);
						
						//figure pattern, pass to node
						direction = figurePattern(vectorpatterns);//pass direction to corresponding node 
						if (direction == -1)
						{
							cerr << "no direction found for" << y << " " << x << endl;
						}
						//now check boundaries
						switch (direction)
						{
						case UP:
						case DOWN:
							caseUpDown(x, y, direction, initNodeGrid[y][x]);
							
							
							break;
						case LEFT:
						case RIGHT:
							caseLeftRight(x, y, direction, initNodeGrid[y][x]);
							
							break;
						case UNDECIDED:
							initNodeGrid[y][x]->initUndecided();

						}

					}
				}
				if (initNodeGrid[y][x] != NULL)
				{
					if (!(initNodeGrid[y][x]->m_type == Node::NOTDETECTED))
					{
						nodesDeque.push_back(initNodeGrid[y][x]);
					}
					else
					{
						undecidednodesDeque.push_back(initNodeGrid[y][x]);
					}
				}
			}

		}
		for (Node* pNode : nodesDeque)
		{
			for (int i = 1; i <=3; i++)
			{
				pNode->computePosForFrame(i);
				patterngrid[pNode->curPos.y][pNode->curPos.x] = 0;
			}
		}
 
		std::fstream	ofc("MapCleaned.txt", std::ios::out | std::ios::trunc);

		if (ofc.is_open())
		{
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					ofc << patterngrid[y][x] << " ";
				}
				ofc << "\n";
			}
			ofc.close();
		}
		//let's deal with undecided - so far just an attempt

		return undecidednodesDeque.size();





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
				//check if there is another one 111 pattern
				for (int j = 0; j < 4; j++)
				{
					if (j == found)
						continue;
					if (vectorpatterns[j] == pattern111 || vectorpatterns[j] == pattern21 || vectorpatterns[j] == pattern210)
					{
						//yes there is at least one more pattern
						found = UNDECIDED;
						break;
					}

				}
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




};












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
		"..@....@....",
		"...........@",
		".@..........",
		".....@......",
		"@...........",
		"............",
		"............",
		".@..........",
		"............",
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
				"..@........@",
				".......@....",
				"@...........",
				"............",
				".@...@......",
				"............",
				"............",
				"..@.........",
				"............",
			};
			//add second frame
			pDetection->addSecondFrame(initList);
			break;
		case 3:
			//add third frame, init all
			initList = {
				"..@.........",
				"...........@",
				".@.....@....",
				"............",
				"..@.........",
				".....@......",
				"............",
				"...@........",
				"............",
			};
			pDetection->addThirdFrame(initList);
			pDetection->analyseThirdFrame(initList);
			while (pDetection->undecidednodesDeque.size())
			{
				//resolve undecided
				pDetection->analyseUndecided();
			}
			break;
		default:
			//simulate
			//test so far
			Node* pNode = pDetection->nodesDeque[0];
			for (int frame = 1; frame <= 100; frame++)
			{
				//for (Node* pNode : pDetection->nodesDeque)
				
				{
					pNode->computePosForFrame(frame);
					bool stop = true;
				}
				std::cout << "frame " << frame <<" posX " << pNode->curPos.x  << " posY " << pNode->curPos.y << std::endl;
				bool stop = true;
			}
			break;

		}
		//int rounds; // number of rounds left before the end of the game
		//int bombs; // number of bombs left
		//cin >> rounds >> bombs; cin.ignore();
		//cerr << "rounds " << rounds << "bombs " << bombs << endl;
		//test
		
		std::cout << "WAIT" << std::endl;
		simrounds++;
	}
	
		
}