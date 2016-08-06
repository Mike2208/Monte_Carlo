#include "standard_definitions.h"

#include "algorithm_a_star.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "test_map_2d.h"

#include "png_convert_image_to_ogm.h"

#include <fstream>
#include <iostream>

using namespace std;

template<class T>
void PrintPath(const AlgorithmAStar::PATH_DATA &Path, const T &ValueToSet, Map2D<T> &Map)
{
	for(const auto &curPos : Path)
	{
		Map.SetPixel(curPos, ValueToSet);
	}
}

int main()
{
	TestMap2D testMap(0, 100, 10);
	testMap.CreateMapsFromProbabilityPNGFile("test.png");

	unsigned int numCells = 0;
	for(const auto curData : testMap.GetOGMap().GetCellStorage())
	{
		if(curData != OGM_CELL_MIN && curData != OGM_CELL_MAX && curData != 10)
		{
			std::cout << std::to_string(curData) << std::endl;
			numCells++;
		}
	}

	//testMap.GetOGMap().PrintMap("/tmp/testOGM.pgm");
	testMap.GetRealMap().PrintMap("/tmp/testREAL.pbm");

	AlgorithmAStar tmpAStar;
	AlgorithmAStar::PATH_DATA tmpPath;
	AlgorithmAStar::DISTANCE_TYPE tmpDist;
	tmpAStar.CalculatePath(testMap.GetRealMap(), true, POS_2D(836,testMap.GetOGMap().GetHeight()-1-491), POS_2D(950,testMap.GetOGMap().GetHeight()-1-490), nullptr, &tmpPath, &tmpDist);

	PrintPath(tmpPath, true, testMap.GetRealMapR());

	testMap.GetRealMap().PrintMap("/tmp/testREAL_Path.pbm");

	return 1;
}
