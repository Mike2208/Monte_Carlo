#include "standard_definitions.h"

#include "algorithm_a_star.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "test_map_2d.h"

#include "png_convert_image_to_ogm.h"

#include "monte_carlo_option2.h"
#include "file_tree.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	RobotData bot;

	OccupancyGridMap testOGM;
	PNGConvertImageToOGM::ConvertPNGToOGM("test.png", testOGM);

	DistrictMapStorage testDistrictStorage;
	testDistrictStorage.ResetDistricts<OccupancyGridMap::CELL_TYPE>(testOGM);

	MonteCarloOption2 testMC(bot, 100);
	testMC.PerformMonteCarlo(testOGM, testDistrictStorage, POS_2D(0,0), POS_2D(1,0));
}
