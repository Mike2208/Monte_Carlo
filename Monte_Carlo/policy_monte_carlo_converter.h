#ifndef POLICY_MONTE_CARLO_CONVERTER_H
#define POLICY_MONTE_CARLO_CONVERTER_H

/*	class PolicyMonteCarloConverter
 *		converts a monte carlo tree to a policy tree
 */

#include "standard_definitions.h"
#include "monte_carlo_definitions.h"
#include "policy_tree.h"

class PolicyMonteCarloConverter
{
	public:

		static void ConvertMonteCarloToPolicyTree(const MONTE_CARLO_TREE_CLASS &MonteCarloTree, PolicyTree &NewPolTree);
};

#endif // POLICY_MONTE_CARLO_CONVERTER_H
