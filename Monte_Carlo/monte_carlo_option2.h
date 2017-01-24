#ifndef MONTE_CARLO_OPTION2_H
#define MONTE_CARLO_OPTION2_H

/*	class MonteCarloOption2
 *
 */

#include "check_conditions.h"

#include "standard_definitions.h"
#include "map_2d.h"
#include "occupancy_grid_map.h"
#include "quad_d_star_maps.h"

#include "monte_carlo_branch_data.h"
#include "monte_carlo_node_data.h"
#include "policy_data.h"

// Compare function for sorting children
class MONTE_CARLO_NODE_COMPARE
{
	public:
		bool operator()(const MONTE_CARLO_NODE &Node1, const MONTE_CARLO_NODE &Node2)
		{
			if(Node1.GetData().ExpectedLength < Node2.GetData().ExpectedLength)
				return true;
			else
				return false;
		}
};

class MonteCarloOption2
{
	public:
		//MonteCarloOption2() = default;
		MonteCarloOption2(const std::vector<OccupancyGridMap> &Maps);

		int PerformMonteCarlo(const POS_2D &_StartPos, const POS_2D &Destination, const float ExploreParam, const CheckConditions &StopConditions, const char* const PolicyFileName, PolicyData &NewPolicy);

		const MONTE_CARLO_TREE_CLASS &GetTree() const { return this->_Tree; }

#ifdef DEBUG	// DEBUG
		void PrintTree() { this->_Tree.PrintTree_DepthFirst(); }
#endif			// ~DEBUG

	private:

		POS_2D _StartPos, _DestPos;
		std::vector<MonteCarloBranchData> _PrevBranches;		// Stores previous branches
		MonteCarloBranchData	_CurBranchData;		// Stores data of current path
		MonteCarloBranchData	_StartBranch;
		MONTE_CARLO_TREE_CLASS	_Tree;				// Monte Carlo Tree

		// Cost Parameters
		MONTE_CARLO_NODE_DATA::COST_TYPE	_MoveCost = 1;
		MONTE_CARLO_NODE_DATA::COST_TYPE	_ObserveCost = 1;

		int Selection();		// Selection phase
		int Expansion();
		int Simulation();
		int Backtrack();

		void CalculateNodeValue(const MONTE_CARLO_NODE &NodeData, MONTE_CARLO_NODE_DATA::NODE_VALUE &Value) const;

		// Simulation functions
		int NodeSimulation(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE &NodeToSimulate);
		int NodeSimulation_CalculateExpectedLength(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH &ExpectedLength) const;
		void SetNodeToDeadEnd(MonteCarloNodeData &NodeData);

		// Backtrack functions
		int PerformBacktrackStep(MONTE_CARLO_NODE &NodeToBacktrackTo);
		void SortChildren(MONTE_CARLO_NODE &ParentNode);

		PolicyData CreatePolicyFromTree(const char *FileName);
		void CreatePolicyFromTreeStep( MonteCarloBranchData &Branch, PolicyData::TREE_NODE &PolicyNode );
};

#endif // MONTE_CARLO_OPTION2_H
