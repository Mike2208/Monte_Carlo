#include "standard_definitions.h"

#include "monte_carlo_option1.h"
#include "policy_monte_carlo_converter.h"
#include "policy_tree.h"

#include "png_convert_image_to_ogm.h"

#include "test_map_2d.h"
#include "test_policy.h"

#include "algorithm_voronoi_fields.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	OccupancyGridMap testMap;
	PNGConvertImageToOGM::ConvertPNGToOGM("test.png", testMap);

	DistrictMap testDistrict;
	testDistrict.ResetMap(testMap.GetWidth(), testMap.GetHeight(), DISTRICT_MAP::IN_DISTRICT);
	testDistrict.SetGlobalMapPosition(POS_2D(0,0));
	testDistrict.SetID(0);

	ALGORITHM_VORONOI_FIELDS::ID nextFreeID = 1;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE freeDistricts, occupiedDistricts;
	AlgorithmVoronoiFields<OccupancyGridMap::CELL_TYPE>::CalculateVoronoiField(testMap, testDistrict, occupiedDistricts, freeDistricts, nextFreeID);

	return 1;
}
