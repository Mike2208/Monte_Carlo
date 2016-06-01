#ifndef MONTE_CARLO_BRANCH_DATA_H
#define MONTE_CARLO_BRANCH_DATA_H

/*	class MonteCarloBranchData
 *		stores values for one run-through
 */

#include "monte_carlo_node_data.h"
#include "monte_carlo_definitions.h"

#include "map_2d.h"
#include "occupancy_grid_map.h"
#include "d_star_map.h"

#include <vector>

class MonteCarloBranchData
{
		typedef	std::vector<POS_2D>::size_type	PATH_ITERATOR;
		typedef unsigned int	VISIT_COUNTER_TYPE;
		typedef OGM_LOG_TYPE	LOG_MAP_TYPE;

	public:
		MonteCarloBranchData();

		void Reset();

		void SetMonteCarloObjectives(const OccupancyGridMap &Map, const POS_2D &Destination);

		void ResetBranchDataToRoot(const MONTE_CARLO_TREE_CLASS &Tree);

		int	MoveDownOneNode(const MONTE_CARLO_NODE::CHILD_ID &NextNodeID);		// Moves down the tree towards leaves
		int	MoveUpOneNode();			// Moves up the tree towards the root

		const MonteCarloNodeData &GetNodeData() const { return this->pCurNode->GetData(); }

		POS_2D				Destination;	// Necessary for D* map calculation

		MONTE_CARLO_NODE	*pCurNode;		// Current node of this branch

		std::vector<POS_2D>	PathTaken;					// Path of robot to reach this node
		PATH_ITERATOR		FirstMoveAfterObservation;	// Stores where the last observation took place

		const Map2D<OGM_CELL_TYPE>	*pOGMap;		// Occupancy Grid Map
		Map2D<LOG_MAP_TYPE>			LogMap;			// Converted Occupancy Grid Map to log values for D* algorithm ( log values let us add probabilities instead of multiplying )

		DStarMap<LOG_MAP_TYPE>		DStarCostMap;	// Used to store D* calculations

		Map2D<VISIT_COUNTER_TYPE>	NumVisitsMap;	// Keeps track of visits to map position

	private:

		void UpdateBranchData(const MONTE_CARLO_NODE &NextNode);	// Move down towrds leaves
		void RevertBranchData(const MONTE_CARLO_NODE &PrevNode);		// Move up towards root
};

#endif // MONTE_CARLO_BRANCH_DATA_H
