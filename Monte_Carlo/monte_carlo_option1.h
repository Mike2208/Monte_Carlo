#ifndef MONTE_CARLO_OPTION1_H
#define MONTE_CARLO_OPTION1_H

#include "standard_definitions.h"
#include "map_2d.h"
#include "occupancy_grid_map.h"
#include "algorithm_d_star.h"

#include "monte_carlo_branch_data.h"
#include "monte_carlo_node_data.h"

class MonteCarloOption1
{
	public:
		MonteCarloOption1();

		int PerformMonteCarlo(const OccupancyGridMap &OGMap, const POS_2D &StartPos, const POS_2D &Destination);

	private:

		MonteCarloBranchData	_CurBranchData;		// Stores data of current path
		MONTE_CARLO_TREE_CLASS	_Tree;				// Monte Carlo Tree

		int Selection();		// Selection phase
		int Expansion();
		int Simulation();

		int NodeSimulation(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE &NodeToSimulate);
};

#endif // MONTE_CARLO_OPTION1_H
