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
	int x;
	int y;
	vec2d() :x(0), y(0)
	{};
	vec2d(int initX, int initY) : x(initX), y(initY)
	{};

	int& operator[] (size_t i)
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
	Bomb(int _x, int _y, int frame) :x(_x), y(_y), tickCounter(0), framePlaced(frame)
	{
		frameExplodes = framePlaced + 2;
	}
	int x, y;
	int framePlaced;
	int frameExplodes;//framePlaced+4 waits 3 steps then explodes
	int tickCounter;
	void render() {};//pass into params.

	bool tick()
	{
		tickCounter++;
		if ((tickCounter+framePlaced) >= frameExplodes)
		{
			return true;
		}
		return
			false;
	}

	shared_ptr<Bomb> clone()
	{
		shared_ptr<Bomb> pBomb = make_shared<Bomb>(x,y,framePlaced);

		//pBomb->x = x;
		//pBomb->y = y;
		//pBomb->framePlaced = framePlaced;
		//pBomb->frameExplodes = frameExplodes;

		return pBomb;
	}
};

class Node {
public:
	enum NodeType
	{
		HORIZONTAL = 0,
		VERTICAL,
		STATIC,
		NOTDETECTED,
	};

	Node(Node::NodeType _type):m_type(_type)
	{};

	Node(int _initX,int _initY):initPos(_initX,_initY), curPos(_initX, _initY)
	{
		b_eliminated = false;
		width = 0;
		initRange = 0;
		travelRange = 0;
		additionalTimefornode = 0;
		b_isbounded = false;
		
	};
	

	Node()
	{
		b_eliminated = false;
		width = 0;
		initRange = 0;
		travelRange = 0;
		additionalTimefornode = 0;
		b_isbounded = false;
	};

	

	shared_ptr<Node> clone()
	{
		shared_ptr<Node> pNode = make_shared<Node>(m_type);
			
		pNode->width =  width;
		pNode->initPos = initPos;
		pNode->curPos = curPos;
		pNode->initRange = initRange;
		pNode->travelRange = travelRange;
		pNode->additionalTimefornode = additionalTimefornode;
		pNode->b_eliminated = b_eliminated;
		pNode->b_isbounded = b_isbounded;

		return pNode;
	}

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

	
	vec2d initPos;
	vec2d curPos;
	int additionalTimefornode;//fake 'time' to fit into pingpong function
	NodeType m_type;
	int width;
	int initRange;
	int travelRange;
	bool b_eliminated;
	bool b_isbounded;

	void computePosOnFrame(int frameNum) {};
	

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
			b_isbounded = true;
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
			b_isbounded = true;
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
			b_isbounded = true;
		}


	}

	

	void computePosForFrame(int frame)
	{
		if (m_type == STATIC)
			return;

		curPos[m_type] = initRange + pingpong((frame - 1) + additionalTimefornode, width);
	
	}

	Node::NodeType GetType()
	{
		return m_type;
	}



};

typedef std::deque<Node> nodelist_t;

class CellType {

public:
	CellType()
	{
		m_eType = ET_EMPTY;
		pNode = 0;
	}
	enum EType
	{
		ET_EMPTY = '.',
		ET_NODE = '@',
		ET_PASSIVE = '#',
		ET_EXPLOSION = '*',


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
	Possibility(int _x, int _y, int _roundPlaced, int _roundActed) :x(_x), y(_y),
		roundActed(_roundActed),roundPlaced(_roundPlaced)
	{
		score = 0;
		scoreWithRange = 0;
		scoreWithSingles = 0;
		numBounded = 0;
		sumRangeOfMoving = 0;
	}
	int x, y; //coordinates of the point
	int roundActed;
	int roundPlaced;//
	int score;
	int scoreWithRange;
	int scoreWithSingles;
	int numBounded;
	int sumRangeOfMoving;
	//list<Node*> affectedNodesList;//?

};

typedef std::deque <Possibility>  possibilitiesdeque_t;
typedef std::deque <Bomb>  bombsdeque_t;

template<class InputIterator, class OutputIterator>
OutputIterator copy_gm(InputIterator first, InputIterator last, OutputIterator result)
{
	while (first != last) {
		*result = (*first)->clone();;
		++result; ++first;
	}
	return result;
}

template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if_gm(InputIt first, InputIt last,
	OutputIt d_first, UnaryPredicate pred)
{
	while (first != last) {
		if (pred(*first))
			*d_first++ = (*first)->clone();
		first++;
	}
	return d_first;
}

class GameField {
public:
	GameField(int _width, int _height) :width(_width), height(_height)
	{
		passiveNodesGrid.resize(height);
		currentMap.resize(height);
		placementMap.resize(height);
		step2predMap.resize(height);
		predictedMap.resize(height);
		isLastTest = false;
	};
	int width, height;
	int numBombs;
	int rounds;
	int maxDepth;//adjust this by trial.
	grid_t passiveNodesGrid;
	grid_t currentMap;
	grid_t placementMap;
	grid_t step2predMap;
	grid_t predictedMap;
	bombsdeque_t bombsdeque;
	possibilitiesdeque_t possibilitiesdeque;
	nodesdesque_t nodesdeque;
	bool isLastTest;

	void init(intgrid_t& passivenodesgrid)
	{
		for (int y = 0; y < height; y++) {
			passiveNodesGrid[y].resize(width);
			currentMap[y].resize(width);
			placementMap[y].resize(width);
			step2predMap[y].resize(width);
			predictedMap[y].resize(width);

			for (int x = 0; x < width; x++)
			{
				int hasPassive = passivenodesgrid[y][x];

				if (hasPassive)
				{
					passiveNodesGrid[y][x].SetType(CellType::ET_PASSIVE);
				}

			}

		}

	};
		
	void applyExplosion(grid_t& grid, int x, int y)
	{
		grid[y][x].SetType(CellType::ET_EMPTY);
		if (grid[y][x].pNode)
		{
			grid[y][x].pNode->b_eliminated = true;
		}
		for (int i = x - 1; i >= max(0, x - 3); --i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				grid[y][i].SetType(CellType::ET_EMPTY);
				grid[y][i].pNode->b_eliminated = true;
			}

		}
		for (int i = x + 1; i <= min<int>(grid[0].size() - 1, x + 3); ++i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				grid[y][i].SetType(CellType::ET_EMPTY);
				grid[y][i].pNode->b_eliminated = true;
			}
		}
		for (int i = y - 1; i >= max(0, y - 3); --i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				grid[i][x].SetType(CellType::ET_EMPTY);
				grid[i][x].pNode->b_eliminated = true;
			}
		}
		for (int i = y + 1; i <= min<int>(grid.size() - 1, y + 3); ++i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				grid[i][x].SetType(CellType::ET_EMPTY);
				grid[i][x].pNode->b_eliminated = true;
			}
		}
	}

	int calcScore(grid_t&  grid, shared_ptr<Possibility>& possibility, int x, int y)
	{
		int damage = 0;
		if (grid[y][x] == CellType::ET_NODE)
		{
			damage++;
			if (grid[y][x].pNode->b_isbounded)
			{
				possibility->numBounded++;
				//possibility->sumRangeOfMoving+= grid[y][x].pNode->width;
			}
			
		}
		for (int i = x - 1; i >= max(0, x - 3); --i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				damage++;
				if (grid[y][i].pNode->b_isbounded)
				{
					possibility->numBounded++;
				}
			}
		}
		for (int i = x + 1; i <= min<int>(grid[0].size() - 1, x + 3); ++i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				damage++;
				if (grid[y][i].pNode->b_isbounded)
				{
					possibility->numBounded++;
				}
			}
		}
		for (int i = y - 1; i >= max(0, y - 3); --i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				damage++;
				if (grid[i][x].pNode->b_isbounded)
				{
					possibility->numBounded;
				}
			}
		}
		for (int i = y + 1; i <= min<int>(grid.size() - 1, y + 3); ++i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				damage++;
				if (grid[i][x].pNode->b_isbounded)
				{
					possibility->numBounded++;
				}
			}
		}
		possibility->score = damage;
		return damage;
	}

	void applyExplosionWithTrack(grid_t& grid,grid_t& trackgrid, int x, int y)
	{
		grid[y][x].SetType(CellType::ET_EMPTY);
		trackgrid[y][x].SetType(CellType::ET_EXPLOSION);
		if (grid[y][x].pNode)
		{
			grid[y][x].pNode->b_eliminated = true;
		}

		for (int i = x - 1; i >= max(0, x - 3); --i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				//grid[y][i].SetType(CellType::ET_EMPTY);
				trackgrid[y][i].SetType(CellType::ET_EXPLOSION);
				grid[y][i].pNode->b_eliminated = true;
			} 
			else if (grid[y][i] == CellType::ET_EMPTY)
			{
				trackgrid[y][i].SetType(CellType::ET_EXPLOSION);
			}

		}
		for (int i = x + 1; i <= min<int>(grid[0].size() - 1, x + 3); ++i)
		{
			if (grid[y][i] == CellType::ET_PASSIVE)
				break;
			else if (grid[y][i] == CellType::ET_NODE)
			{
				//grid[y][i].SetType(CellType::ET_EMPTY);
				trackgrid[y][i].SetType(CellType::ET_EXPLOSION);
				grid[y][i].pNode->b_eliminated = true;
			}
			else if (grid[y][i] == CellType::ET_EMPTY)
			{
				trackgrid[y][i].SetType(CellType::ET_EXPLOSION);
			}
		}
		for (int i = y - 1; i >= max(0, y - 3); --i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				//grid[i][x].SetType(CellType::ET_EMPTY);
				trackgrid[i][x].SetType(CellType::ET_EXPLOSION);
				grid[i][x].pNode->b_eliminated = true;
			}
			else if (grid[i][x] == CellType::ET_EMPTY)
			{
				trackgrid[y][i].SetType(CellType::ET_EXPLOSION);
			}
		}
		for (int i = y + 1; i <= min<int>(grid.size() - 1, y + 3); ++i)
		{
			if (grid[i][x] == CellType::ET_PASSIVE)
				break;
			else if (grid[i][x] == CellType::ET_NODE)
			{
				//grid[i][x].SetType(CellType::ET_EMPTY);
				trackgrid[i][x].SetType(CellType::ET_EXPLOSION);
				grid[i][x].pNode->b_eliminated = true;
			}
			else if (grid[i][x] == CellType::ET_EMPTY)
			{
				trackgrid[i][x].SetType(CellType::ET_EXPLOSION);
			}
		}
	}
	
	bool simulate(int frame, int& bombs)
	{
		
		//1 - render map where we place a bomb
		//temp one bomb

		int remains = nodesdeque.size();
		if (remains == 0) {
			// no more nodes to blow
			return true;//seq;
		}
		if (bombs == 0) {
			// no more bombs
			return false;
		}

		int numBounded = count_if(nodesdeque.begin(), nodesdeque.end(), [](const shared_ptr<Node>& pNode) { return pNode->b_isbounded; });
		if (numBounded == bombs)
		{
			//last test detected - we have isolated regions equal to bombs each requires one bomb
			isLastTest = true;
		}

		
		// compute possible bomb locations
		vector<shared_ptr<Possibility>> possibles = {};
		//simulate current map, remove nodes which are affected by current bomb
		//
		//clear
		std::copy(passiveNodesGrid.begin(), passiveNodesGrid.end(), currentMap.begin());
		std::copy(passiveNodesGrid.begin(), passiveNodesGrid.end(), placementMap.begin());
		std::copy(passiveNodesGrid.begin(), passiveNodesGrid.end(), step2predMap.begin());
		std::copy(passiveNodesGrid.begin(), passiveNodesGrid.end(), predictedMap.begin());
		

		for (shared_ptr<Node> pNode : nodesdeque)
		{
			pNode->computePosForFrame(frame);
			currentMap[pNode->curPos.y][pNode->curPos.x].SetType(CellType::ET_NODE);
			currentMap[pNode->curPos.y][pNode->curPos.x].pNode = pNode.get();
		}
		//apply bombs on currentMap
		std::deque<Bomb>::iterator iter = bombsdeque.begin();
		while (iter != bombsdeque.end())
		{
			if (iter->frameExplodes == frame)
			{//applyBomb to currentMap
				//Bomb bomb = *iter;
				applyExplosionWithTrack(currentMap,currentMap,iter->x, iter->y);
				iter = bombsdeque.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		nodesdesque_t nodesdeque_local;
		//copy nodes
		nodesdeque.erase(remove_if(nodesdeque.begin(), nodesdeque.end(), [](const shared_ptr<Node>& pNode) { return pNode->b_eliminated; }), nodesdeque.end());
		copy_gm(nodesdeque.begin(), nodesdeque.end(), back_inserter(nodesdeque_local));

		for (shared_ptr<Node> pNode : nodesdeque_local)
		{
			pNode->computePosForFrame(frame+1);
			placementMap[pNode->curPos.y][pNode->curPos.x].SetType(CellType::ET_NODE);
			placementMap[pNode->curPos.y][pNode->curPos.x].pNode = pNode.get();
		}
		
		iter = bombsdeque.begin();
		while (iter != bombsdeque.end())
		{
			if (iter->frameExplodes == frame+1)
			{//applyBomb to currentMap
				//Bomb bomb = *iter;
				//TODO - save somehow the removed positions - maybe useful
				applyExplosionWithTrack(placementMap, currentMap, iter->x, iter->y);
				
			}
			++iter;
		}
		//remove nodes
		nodesdeque_local.erase(remove_if(nodesdeque_local.begin(), nodesdeque_local.end(), [](const shared_ptr<Node>& pNode) { return pNode->b_eliminated; }), nodesdeque_local.end());

		for (shared_ptr<Node> pNode : nodesdeque_local)
		{
			pNode->computePosForFrame(frame + 2);
			step2predMap[pNode->curPos.y][pNode->curPos.x].SetType(CellType::ET_NODE);
			step2predMap[pNode->curPos.y][pNode->curPos.x].pNode = pNode.get();
		}

		iter = bombsdeque.begin();
		while (iter != bombsdeque.end())
		{
			if (iter->frameExplodes == frame + 2)
			{//applyBomb to currentMap
				//Bomb bomb = *iter;
				//TODO - save somehow the removed positions - maybe useful
				applyExplosionWithTrack(step2predMap, currentMap, iter->x, iter->y);

			}
			++iter;
		}
		//remove nodes
		nodesdeque_local.erase(remove_if(nodesdeque_local.begin(), nodesdeque_local.end(), [](const shared_ptr<Node>& pNode) { return pNode->b_eliminated; }),nodesdeque_local.end());


		for (shared_ptr<Node> pNode : nodesdeque_local)
		{
			pNode->computePosForFrame(frame + 3);
			predictedMap[pNode->curPos.y][pNode->curPos.x].SetType(CellType::ET_NODE);
			predictedMap[pNode->curPos.y][pNode->curPos.x].pNode = pNode.get();
		}

		//now place a bomb in free space
		
		int stillBounded = count_if(nodesdeque_local.begin(), nodesdeque_local.end(), [](const shared_ptr<Node>& pNode) { return pNode->b_isbounded; });
		
		
		
		remains = nodesdeque_local.size();
		if (remains == 0)
		{
			return true;
		}
		for (size_t y = 0; y < currentMap.size(); y++)
		{
			for (size_t x = 0; x < currentMap[y].size(); x++)
			{
				CellType cell = currentMap[y][x];
				if (cell == CellType::ET_EMPTY)
				{
					shared_ptr<Possibility> possibility = make_shared<Possibility>((int)x, (int)y,frame+1,frame+3);
					int count = calcScore(predictedMap, possibility, x, y);
					int desiredCount = std::max({ remains / bombs, 1 });
					if (isLastTest)
					{
						//slowly decrease desiredCount to find all 'big' hits
						desiredCount = std::max({ remains / bombs, int (remains / (frame*0.05)) - 3,1 });
					}

					if (count == remains || count >=desiredCount ) {//aims at least for two cells ok.
						possibles.push_back(possibility);
					}
				}
			}
		}
		
		sort(possibles.begin(), possibles.end(), [](const shared_ptr<Possibility>& a, const shared_ptr<Possibility>& b) { return b->score < a->score; });
		if (stillBounded>0)
		{
			possibles.erase(remove_if(possibles.begin(), possibles.end(), [](const shared_ptr<Possibility>& pNode) { return (pNode->numBounded == 0); }), possibles.end());
			//now check possibilities for bounded if they could be improved? or for double bounded only? for last test?
		}

		if (bombs > 0)
		{
			if (possibles.size())
			{
				if (possibles[0]->score > 0)
				{
					bombsdeque.push_back(Bomb(possibles[0]->x, possibles[0]->y, possibles[0]->roundPlaced));
					bombs--;
				}
				
			}
			
		}

		//std::cout << "frame " << frame << " posX " << pNode->curPos.x << " posY " << pNode->curPos.y << std::endl;
	
		return true;
	};

};




class Detection {
public:
	
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
	void addFrame(vector<string>& initList)
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
			pNode->initBoundaries(Node::HORIZONTAL, (direction == LEFT) ? -1 : 1, width, leftObstacle, rigthObstacle);
			//pNode->initHorizontalBoundaries(static_cast<Directions>(direction), width, leftObstacle, rigthObstacle);

		}
		else
		{
			pNode->initBoundaries(Node::HORIZONTAL, (direction == LEFT) ? -1 : 1, width);
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
					patterngrid[pNode->curPos.y][pNode->curPos.x] = 0;
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
      
		//let's deal with undecided

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
};












/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	bool real = false;

	int width; // width of the firewall grid
	int height; // height of the firewall grid
	if (real)
	{
		cin >> width >> height; cin.ignore();
		cerr << "width " << width << " height " << height << endl;
	}
	else
	{
		width = 16;
		height = 12;
	}

		
	
	

	int rounds; // number of rounds left before the end of the game
	int bombs; // number of bombs left
	

	GameField game(width,height);
	vector<string> initList = {};
	
	
	
	//create game field and make initialization
	//Detection detection(width, height, initList);

	shared_ptr<Detection> pDetection(nullptr);
	
	//dump_map(grid);
	//vector<string> initList;

	// game loop
	int simrounds = 1;
	if (!real)
	{
		rounds = 100;
		bombs = 10;
	}

	while (1) {
		
		
		//initList.clear();
		//int rounds; // number of rounds left before the end of the game
		//int bombs; // number of bombs left
		if (real)
		{
			cin >> rounds >> bombs; cin.ignore();
			cerr << "bombs " << bombs << endl;
			initList.clear();
			for (int i = 0; i < height; i++) {
				string mapRow; // one line of the firewall grid
				getline(cin, mapRow);
				
				initList.push_back(mapRow);
				//add mapRow to array
				//cerr << "mapRow " << i << " " << mapRow << endl;//again
				
			}
		}
		else
		{
			rounds = 30 - simrounds;
		}
		switch (simrounds)
		{
		case 1:
			if (!real)
			{
				initList =
				{
					"##..@...##@@@.@@",
					"##.....#.##..@..",
					"..#...#..#######",
					"...#@#...##@...#",
					"@..@.@..@#....#.",
					"...#@#####...#..",
					"..#...#...#@#...",
					".#....#@..@.@..@",
					"#...@.##..#@#...",
					"#######@##...#..",
					"......@.@#....##",
					"@.........#@..#.",
					
				};
			}
			//init
			//initrounds = rounds;
			pDetection.reset(new Detection(width, height, initList));
			game.init(pDetection->passivenodesgrid);
			break;
		case 2:
			if (!real)
			{
				initList = {
					"##.@....##@@@.@@",
					"##.....#.##.@...",
					"..#...#..#######",
					"...#@#..@##.@..#",
					"...@.@...#....#.",
					"@..#@#####...#..",
					"..#...#@..#@#...",
					".#....#...@.@...",
					"#..@..##..#@#..@",
					"#######@##...#..",
					"......@.@#....##",
					".@........#.@.#.",

				};
			}
			//add second frame
			pDetection->addFrame(initList);
			break;
		case 3:
			//add third frame, init all
			if (!real)
			{
				initList = {
					"##@.....##@@@.@@",
					"##.....#.##@....",
					"..#...#.@#######",
					"...#@#...##..@.#",
					"...@.@...#....#.",
					"...#@#####...#..",
					"@.#...#...#@#...",
					".#....#@..@.@...",
					"#.@...##..#@#...",
					"#######@##...#.@",
					"......@.@#....##",
					"..@.......#..@#.",
				};
			}
			pDetection->addFrame(initList);
			pDetection->analyseThirdFrame(initList);
			while (pDetection->undecidednodesDeque.size())
			{
				//resolve undecided
				pDetection->analyseUndecided();
			}
			for (Node* pNode : pDetection->nodesDeque)
			{
				game.nodesdeque.push_back(pNode->clone());
			}
			
			game.rounds = rounds;
			game.simulate(simrounds, bombs);
			break;
		default:
			game.simulate(simrounds, bombs);
			   		
			
			break;

		}

		//int rounds; // number of rounds left before the end of the game
		//int bombs; // number of bombs left
		//cin >> rounds >> bombs; cin.ignore();
		//cerr << "rounds " << rounds << "bombs " << bombs << endl;
		//test
		deque<Bomb>::iterator it = find_if(game.bombsdeque.begin(), game.bombsdeque.end(), [&simrounds](const Bomb& bomb) { return bomb.framePlaced-1 == simrounds; });
		if (it != game.bombsdeque.end())
		{
			cout << it->x << " " << it->y << endl;
		}
		else
		{
			std::cout << "WAIT" << std::endl;
		}
		
		simrounds++;
		if (simrounds > 101)
		{
			break;
		}
	}
	
		
}