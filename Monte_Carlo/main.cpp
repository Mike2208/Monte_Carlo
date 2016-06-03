#include "standard_definitions.h"
#include "monte_carlo_option1.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	std::ofstream out("/tmp/tmp.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

	Map2D<OGM_CELL_TYPE> tMap;
	tMap.ResetMap(3,3, OGM_CELL_MIN);
	tMap.SetPixel(POS_2D(1,0), OGM_CELL_MAX/4);
	tMap.SetPixel(POS_2D(1,1), OGM_CELL_MAX/3);
	tMap.SetPixel(POS_2D(1,2), OGM_CELL_MAX/2);

	OccupancyGridMap testMap;
	testMap.SetMap(tMap);

	MonteCarloOption1 test;
	test.PerformMonteCarlo(testMap, POS_2D(0,0), POS_2D(2,2));

	std::cout.rdbuf(coutbuf); //reset to standard output again
}
