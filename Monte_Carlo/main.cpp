#include "standard_definitions.h"

#include "monte_carlo_option1.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "png_convert_image_to_ogm.h"

#include "test_map_2d.h"
#include "test_policy.h"
#include "test_functions.h"

#include "algorithm_voronoi_fields.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	OccupancyGridMap testMap;
	testMap.ResetMap(10,10, 0);
	testMap.SetPixel(POS_2D(2,2), OGM_CELL_MAX);
	testMap.SetPixel(POS_2D(7,7), OGM_CELL_MAX);
	testMap.SetPixel(POS_2D(2,7), OGM_CELL_MAX);
	testMap.SetPixel(POS_2D(7,2), OGM_CELL_MAX);
	//PNGConvertImageToOGM::ConvertPNGToOGM("test.png", testMap);

	testMap.PrintMap("/tmp/testOGM.pgm");

	DistrictMap testDistrict;
	testDistrict.SetFreeDistrict(testMap.GetWidth(), testMap.GetHeight(), 0);
	//testDistrict.SetGlobalMapPosition(POS_2D(0,0));
	//testDistrict.SetID(0);

	ALGORITHM_VORONOI_FIELDS::ID nextFreeID = 1;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE freeDistricts, occupiedDistricts;
	AlgorithmVoronoiFields<OccupancyGridMap::CELL_TYPE>::CalculateVoronoiField(testMap, 90, 0, testDistrict, occupiedDistricts, freeDistricts, nextFreeID);

	return 1;
}
