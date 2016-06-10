#ifndef TEST_POLICY_H
#define TEST_POLICY_H

/*	class TestPolicy
 *		tests a created policy against a real map
 */

#include "standard_definitions.h"
#include "map_2d.h"
#include "policy_tree.h"

class TestPolicy
{
	public:

		int PerformTest(PolicyTree &BotPolicy, const POS_2D &StartPos, const POS_2D &Destination, const Map2D_Discrete &MapData);

	private:
};

#endif // TEST_POLICY_H
