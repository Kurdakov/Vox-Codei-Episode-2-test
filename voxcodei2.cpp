// VoxCodei.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>

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

EType operator-(EType const& r, int val) {
	return EType(static_cast<int>(r) - val);   // convert r to integer, add 1, convert back to Enum
}
typedef std::vector<EType> row_t;
typedef std::vector<row_t> grid_t;
typedef std::pair<int, int> node_t;
typedef std::list<node_t> nodes_list_t;


struct Possibility
{
	int c,x, y; //count, coordinates of the point
	
	//bool operator<(const Possibility& rhs)
	//{
	//	return this->c < rhs.c;
	//}
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

int countNodes(const grid_t& grid)
{
	int count=0;
	for (size_t i = 0; i < grid.size(); i++)
	{
		for (size_t j = 0; j < grid[i].size(); j++)
		{
			if (ET_NODE == grid[i][j])
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
			std::cerr << static_cast<char>(m[i][j]);
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
			//(BOMB_2 == grid[i][j] || BOMB_1 == grid[i][j]) ? grid[i][j] - 1 : grid[i][j] == BOMB_0 ? ET_EMPTY : grid[i][j];
			if(BOMB_3 == grid[i][j] || BOMB_2 == grid[i][j] || BOMB_1 == grid[i][j])
			{
				grid[i][j] = grid[i][j] - 1;
				grid[i][j]=(grid[i][j] == BOMB_0) ? ET_EMPTY : grid[i][j];
			}
		}
	}

}



int applyBlow(grid_t& grid, int x,int y,bool horiz, int until,bool immediate,bool simulate) {
	int count = 0;
	bool stopped = (until == 0);
		if (y < grid.size() && x < grid[y].size()) {
			if (grid[y][x] == ET_PASSIVE) {
				// meet passive node
				stopped = true;
			}
			else if (grid[y][x] == ET_NODE) {
				count++;
					if (!simulate) {
						grid[y][x] = immediate ? ET_EMPTY : BOMB_3;
					}
			}
		}
		else {
			stopped = true;
		}
		if (!stopped) {
			count += applyBlow(grid, x + (horiz ? until > 0 ? 1 : -1 : 0),
				y + (!horiz ? until > 0 ? 1 : -1 : 0), horiz,
				until + (until > 0 ? -1 : 1), immediate, simulate);
		}
		return count;
}

int  applyExplosion(grid_t& grid, int x, int y, bool immediate = true, bool simulate = false) {
	grid[y][x] = ET_EMPTY;
	int count = applyBlow(grid, x + 1, y, true, 2, immediate, simulate);
	count += applyBlow(grid, x - 1, y, true, -2, immediate, simulate);
	count += applyBlow(grid, x, y + 1, false, 2, immediate, simulate);
	count += applyBlow(grid, x, y - 1, false, -2, immediate, simulate);
	return count;
}

bool simulate(grid_t& grid,int bombs, vector<Possibility>& seq) {
	int remains = countNodes(grid);
		if (remains == 0) {
			// no more nodes to blow
			return true;//seq;
		}
	if (bombs ==  0) {
		// no more bombs
		return false;
	}
	// compute possible bomb locations
	vector<Possibility> possibles = {};

	for (size_t y = 0; y < grid.size(); y++)
	{
		for (size_t x = 0; x < grid[y].size(); x++)
		{
			EType cell = grid[y][x];
			int count = cell == ET_EMPTY ? applyExplosion(grid, x, y, true, true) : 0;
			if (count ==  remains || count > 1) {
				possibles.push_back(Possibility { count, (int) x, (int) y });
			}
		}
	}

		// order them by number of destroyed nodes
		sort(possibles.begin(), possibles.end(), [](const Possibility& a, const Possibility& b) { return b.c < a.c; });
		//sort(possibles.begin(), possibles.end());//with operator overloading

		
		int idx = seq.size();
		for (Possibility possible: possibles) {
			seq.push_back(possible);
			grid_t newGrid = {};
			//clone
			std::copy(grid.begin(), grid.end(), back_inserter(newGrid)); 
			applyExplosion(newGrid, possible.x, possible.y);
			bool solution = simulate(newGrid, bombs - 1, seq);
			if (solution) {
					// complete solution found !
				return solution;//seq;
			};
			// backtrack possibility
			//splice(*seq, idx, (*seq).size());
			seq.pop_back();
		}
	// no solution found
		return false;//NULL;
}


/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	int width; // width of the firewall grid
	int height; // height of the firewall grid
	cin >> width >> height; cin.ignore();
    cerr << "width " << width << "height " << height << endl;


	grid_t grid(height);



	for (int i = 0; i < height; i++) {
		string mapRow; // one line of the firewall grid
		cin >> mapRow; cin.ignore();
		cerr << "mapRow " << mapRow << endl;
		grid[i].resize(width);
	    for (int j = 0; j < width; j++)
	    {
		    grid[i][j] = static_cast<EType>(mapRow[j]);
	    }
    }
	dump_map(grid);
	vector<Possibility> seq = {};
	//vector<Possibility>* seq = NULL;
	// game loop
	while (1) {
		int rounds; // number of rounds left before the end of the game
		int bombs; // number of bombs left
		cin >> rounds >> bombs; cin.ignore();
		cerr << "rounds " << rounds << "bombs " << bombs << endl;

		purgeGrid(grid);

		bool seqfound;
		if (seq.size()==0) {
			// compute winning order sequence, only when we got bomb numbers
			simulate(grid, bombs,seq);
		}

		
		if (seq.empty())
		{
			std::cout << "WAIT" << std::endl;
		}
		else
		{
			Possibility order = seq[0];
			if (grid[order.y][order.x] != ET_EMPTY) {
				std::cout << "WAIT" << std::endl;
			}
			else {
				seq.erase(seq.begin());// seq->pop_front()
				applyExplosion(grid, order.x, order.y, false);
				std::cout << order.x << " " << order.y << endl;
			}

			// Write an action using cout. DON'T FORGET THE "<< endl"
			// To debug: cerr << "Debug messages..." << endl;

			//cout << "3 0" << endl;
		}

	}

}