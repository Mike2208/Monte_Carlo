#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

/*	Contains test functions
 *
 */

#include "standard_definitions.h"
#include "monte_carlo_option1.h"
#include "png_convert_image_to_ogm.h"

#include "graph_class.h"

#include <fstream>
#include <iostream>


namespace TEST_FUNCTIONS
{
	int TestPNGFileReader();

	int TestMonteCarlo1();
}



#endif // TEST_FUNCTIONS_H