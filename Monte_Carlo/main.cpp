#include "standard_definitions.h"

#include "algorithm_a_star.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "test_map_2d.h"

#include "png_convert_image_to_ogm.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	TestMap2D testMap;
	testMap.CreateMapsFromProbabilityPNGFile("test.png");

	testMap.GetOGMap().PrintMap("/tmp/testOGM.pgm");

	AlgorithmAStar tmpAStar;
	AlgorithmAStar::PATH_DATA tmpPath;
	AlgorithmAStar::DISTANCE_TYPE tmpDist;
	tmpAStar.CalculatePath(testMap.GetOGMap(), OGM_CELL_MAX, POS_2D(0,0), POS_2D(0,2), nullptr, &tmpPath, &tmpDist);

	return 1;
}
