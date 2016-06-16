#include "standard_definitions.h"

#include "monte_carlo_option1.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "png_convert_image_to_ogm.h"

#include "test_map_2d.h"
#include "test_policy.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	//OccupancyGridMap curTestMap;
	//PNGConvertImageToOGM::ConvertPNGToOGM("testMap.png", curTestMap);

	TestMap2D::SCALING_FACTOR scale = 2;

	POS_2D startPos(100,50);
	POS_2D destPos(101,50);

	TestMap2D testMaps;
	testMaps.CreateMapsFromProbabilityPNGFile("testMap.png");

	testMaps.ScaleMapsDownByFactor(scale);
	startPos = POS_2D(startPos.X/scale, startPos.Y/scale);
	destPos = POS_2D(destPos.X/scale, destPos.Y/scale);

	testMaps.GetOGMap().PrintMap("/tmp.pgm");

	MonteCarloOption1 testMonteCarlo;
	testMonteCarlo.PerformMonteCarlo(testMaps.GetOGMap(), startPos, destPos);

	PolicyTree testPolicy;
	PolicyMonteCarloConverter::ConvertMonteCarloToPolicyTree(testMonteCarlo.GetTree(), testPolicy);

	TestPolicy policyTester;
	policyTester.PerformTest(testPolicy, startPos, destPos, testMaps.GetRealMap());

	return 1;
}
