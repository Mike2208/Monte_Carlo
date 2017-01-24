#include "standard_definitions.h"

#include "algorithm_a_star.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "test_map_2d.h"

#include "png_convert_image_to_ogm.h"

#include "robot_data.h"

#include "monte_carlo_option2.h"
#include "monte_carlo_option3.h"
#include "file_tree.h"

#include "monte_carlo_dstar_maps.h"

#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>
#include <string>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
	POS_2D start(0,0),dest(0,0);
	float exploreParam = 0;
	std::string mapFileName;
	std::string policyOutputFileName;
	bool useQAMCTS = true;

	CheckConditions stopConditions;

	// Options
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("explore,e", po::value<float>(&exploreParam), "set explore parameter")
		("startx", po::value<POS_2D_TYPE>(&start.X), "set start position X value")
		("starty", po::value<POS_2D_TYPE>(&start.Y), "set start position Y value")
		("destX", po::value<POS_2D_TYPE>(&dest.X), "set start position X value")
		("destY", po::value<POS_2D_TYPE>(&dest.Y), "set start position Y value")
		("mapfile", po::value<std::string>(&mapFileName), "map file with data")
		("mapfile", po::value<std::string>(&mapFileName), "map file with data")
		("policyfile", po::value<std::string>(&policyOutputFileName), "output policy file")
		("amcts", "use AMCTS instead of QAMCTS")
		("complete", "Wait until algorithm completes")
		("repetitions", po::value<size_t>(&stopConditions._NumRepetitions),"Wait for a certain number of repetitions")
		("time", po::value<size_t>(&stopConditions._MaxSeconds),"Stop after x seconds")
		("polcert", po::value<float>(&stopConditions._MinPolCertainty),"Wait until policy certainty reaches value")
		("goalcert", po::value<float>(&stopConditions._MinGoalCertainty),"Wait until goal certainty reaches value")
		("mapcert", po::value<float>(&stopConditions._MinMapCertainty),"Wait until map certainty reaches value")
	;

	po::variables_map vm;
	try { po::store(po::parse_command_line(argc, argv, desc), vm);}
	catch(std::exception e)
	{
	 std::cout << "Error reading input\n";
	 return -4;
	}

	po::notify(vm);

	if (vm.count("help"))
	{
	 cout << desc << "\n";
	 return 1;
	}

	// Check algorithm to use
	if(vm.count("amcts"))
	{
		useQAMCTS = false;
	}
	else
		useQAMCTS = true;

	if(vm.count("complete"))
	{
		stopConditions._WaitUntilDone = true;
	}
	else
		stopConditions._WaitUntilDone = false;

	// Get OGMs
	std::fstream mapFiles;
	mapFiles.open(mapFileName, std::fstream::in);
	if(!mapFiles.is_open())
	{
		std::cout << "ERROR: Map File invalid\n";
		return -1;
	}

	std::vector<OccupancyGridMap> testOGMs;
	bool endReached = false;
	do
	{
		char curVal;
		std::string curFileName;
		do
		{
			mapFiles.read(&curVal, sizeof(char));

			if(curVal == std::char_traits<char>::eof())
			{
				endReached = true;
				break;
			}

			curFileName += curVal;
		}
		while(curVal != '\n');

		curFileName += '\0';

		if(curFileName.size() > 0)
		{
			OccupancyGridMap curOGM;
			if(PNGConvertImageToOGM::ConvertPNGToOGM("/home/mike/Master_Thesis/Monte_Carlo/build-Monte_Carlo-Desktop-Debug/test.png", curOGM) >= 0)
				testOGMs.push_back(std::move(curOGM));
		}
	}
	while(!endReached);

	if(testOGMs.empty())
	{
		std::cout << "ERROR: No input map specified\n";
		return -2;
	}

	const auto mapWidth = testOGMs[0].GetWidth();
	const auto mapHeight = testOGMs[0].GetHeight();
	for(const auto &curOGM : testOGMs)
	{
		if(curOGM.GetWidth() != mapWidth || curOGM.GetHeight() != mapHeight)
		{
			std::cout << "ERROR: Maps don't match\n";
			return -3;
		}
	}

	if(!testOGMs[0].IsInMap(start) || !testOGMs[0].IsInMap(dest))
	{
		std::cout << "ERROR: Position outside of maps\n";
		return -3;
	}

	// Create robot
	RobotData curBot;
	curBot.SetGlobalBotPosition(start);

	PolicyData newPolicy;

	if(!useQAMCTS)
	{
		// Use AMCTS
		MonteCarloOption2 testMc2(testOGMs);
		testMc2.PerformMonteCarlo(start, dest, exploreParam, stopConditions, &policyOutputFileName.front(), newPolicy);
	}
	else
	{
		// Use QAMCTS
		MonteCarloOption3 testMC(testOGMs, curBot, 100);
		testMC.PerformMonteCarlo(start, dest, exploreParam, stopConditions, &policyOutputFileName.front(), newPolicy);
	}
}
