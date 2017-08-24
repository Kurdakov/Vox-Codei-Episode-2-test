// test1.cpp
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <iterator>
#include <deque>

using namespace std;

float pingpong(int time,int length)
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

int main()
{
//test 1 - parametrically derive pos of Node given current time in steps
//consider we know - length of travel ( horizontal or vertical ) init pos and direction
//we need simple formula to derive current position of node for any given frame( not running full simulation each time using additions and range checks )
//we have a ping pong formula ( hat tip http://answers.unity3d.com/questions/150347/what-exactly-does-timetime-do-in-mathfpingpong.html )
//which allows us to derive pos on segment, given time and length of segment, but we need also move in computed initial direction for Node from
//initial position
	


	int length = 11;
	int nodepos1 = 7;//moves left
	int nodepos2 = 6;//moves right
	//then, obviously
	int additionalTimefornode1 = length + length - nodepos1;
	int additionalTimefornode2 = nodepos2;

	
	for (int time = 0; time < 22; time++)
	{
		int val1 = pingpong(time+ additionalTimefornode1, length);
		int val2 = pingpong(time+ nodepos2, length);
	}
    //test ok
	
	//TODO
	//the above is just for full line travel ( horizontal or vertical )
	//with unpassable nodes - use either lerp or more additional vars
	

}