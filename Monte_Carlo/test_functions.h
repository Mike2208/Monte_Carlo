#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

/*	Contains test functions
 *
 */

#include "standard_definitions.h"
#include "monte_carlo_option2.h"
#include "png_convert_image_to_ogm.h"

#include "graph_class.h"
#include "test_map_2d.h"
#include "test_policy.h"
#include "policy_monte_carlo_converter.h"
#include "algorithm_voronoi_fields.h"
#include "algorithm_a_star.h"

#include <fstream>
#include <iostream>


namespace TEST_FUNCTIONS
{
	int TestPNGFileReader();

	int TestMonteCarlo1();

	int TestPolicy();

	int TestVoronoiField();

	int TestPath();
}



#endif // TEST_FUNCTIONS_H
