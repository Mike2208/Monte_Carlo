#ifndef MONTE_CARLO_NODE_DATA_H
#define MONTE_CARLO_NODE_DATA_H

#include "standard_definitions.h"
#include "pos_2d.h"
#include "occupancy_grid_map.h"
#include "robot_action.h"

namespace MONTE_CARLO_NODE_DATA
{
	typedef float NODE_VALUE;				// Value of this node
	typedef float EXPECTED_LENGTH;			// Expected length to goal
	typedef OGM_PROB_TYPE	CERTAINTY;		// Certainty of reaching goal from here
	typedef OGM_ENTROPY_TYPE MAP_ENTROPY;	// How much entropy is left in map

	typedef float COST_TYPE;

#ifdef DEBUG	// DEBUG
		static std::ostream& operator<<(std::ostream& os, const NODE_ACTION& obj) { return obj.PrintAction(os); }
#endif			// ~DEBUG
}

class MonteCarloNodeData
{
		typedef ROBOT_ACTION::ACTION_TYPE ACTION_TYPE;
		typedef MONTE_CARLO_NODE_DATA::MAP_ENTROPY MAP_ENTROPY;
	public:

		MonteCarloNodeData(const POS_2D &_NewCell, const ACTION_TYPE &_Action, const MAP_ENTROPY &_RemainingMapEntropy) : Action(_Action), NewCell(_NewCell), IsDone(false), RemainingMapEntropy(_RemainingMapEntropy) {}

		MonteCarloNodeData() = default;
		MonteCarloNodeData(const MonteCarloNodeData &S) = default;
		MonteCarloNodeData(MonteCarloNodeData &&S) = default;
		MonteCarloNodeData &operator=(const MonteCarloNodeData &S) = default;
		MonteCarloNodeData &operator=(MonteCarloNodeData &&S) = default;

		MONTE_CARLO_NODE_DATA::NODE_VALUE	Value;				// Value of this node (used during selection phase)

		RobotAction	Action;				// Action/Result of this node
		POS_2D	NewCell;				// Cell that was observed/moved to

		MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH	ExpectedLength;	// Expected length to dest from here
		MONTE_CARLO_NODE_DATA::CERTAINTY			Certainty;		// Certainty of reaching node from here

		MONTE_CARLO_NODE_DATA::COST_TYPE		ExpectedCost;	// Cost to reach dest from here
		unsigned int NumVisits;

		bool	IsDone;										// Have all possible variations beneath this node been checked?
		MONTE_CARLO_NODE_DATA::MAP_ENTROPY	RemainingMapEntropy;// How much entropy is left in node

		bool IsDeadEnd() const;		// Returns whether this node is a dead end
		void SetToDeadEnd();		// Sets node to dead end

#ifdef DEBUG	// DEBUG
		void PrintNodeData(const unsigned int &NodeDepth) const;
#endif			// ~DEBUG

};

#endif // MONTE_CARLO_NODE_DATA_H
