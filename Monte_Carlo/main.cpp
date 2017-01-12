#include "standard_definitions.h"

#include "algorithm_a_star.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "test_map_2d.h"

#include "png_convert_image_to_ogm.h"

#include "robot_data.h"

#include "monte_carlo_option3.h"
#include "file_tree.h"

#include "monte_carlo_dstar_maps.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	RobotData bot;

	OccupancyGridMap testOGM;
	PNGConvertImageToOGM::ConvertPNGToOGM("/home/mike/Master_Thesis/Monte_Carlo/build-Monte_Carlo-Desktop-Debug/NumVisitTest.png", testOGM);

	testOGM.PrintMap("print.pbm");

	RobotData curBot;
	curBot.SetGlobalBotPosition(POS_2D(0,0));

	OGM_LOG_MAP_TYPE testLogMap;
	OccupancyGridMap::CalculateCertaintyLogMapFromCellMap(testOGM, testLogMap);
	MonteCarloDStarMaps testMaps(testLogMap, POS_2D(50,99), POS_2D(50,25));
	testMaps.ResetMaps(testLogMap);

	testMaps.RatioToGoalMap().PrintMap("printMap.pbm", 100, 0);

//	MonteCarloOption1 testMc1;
//	testMc1.PerformMonteCarlo(testOGM, POS_2D(50,25), POS_2D(50,99));

	MonteCarloOption3 testMC(testOGM, curBot, 100);
	testMC.PerformMonteCarlo(POS_2D(0,0), POS_2D(1,0));
}
