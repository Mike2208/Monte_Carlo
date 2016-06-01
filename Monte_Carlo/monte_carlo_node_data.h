#ifndef MONTE_CARLO_NODE_DATA_H
#define MONTE_CARLO_NODE_DATA_H

#include "pos_2d.h"
#include "occupancy_grid_map.h"

namespace MONTE_CARLO_NODE_DATA
{
	typedef float NODE_VALUE;				// Value of this node
	typedef float EXPECTED_LENGTH;			// Expected length to goal
	typedef OGM_PROB_TYPE	CERTAINTY;		// Certainty of reaching goal from here
	typedef OGM_ENTROPY_TYPE MAP_ENTROPY;	// How much entropy is left in map?

	typedef char ACTION_TYPE;
	const ACTION_TYPE	ACTION_MOVE		= 0b00000001;		// Indicates Move action
	const ACTION_TYPE	ACTION_OBSERVE	= 0b00000010;		// Indicates Observe action
	const ACTION_TYPE	OBSERVE_RESULT	= 0b00000100;		// Indicates observation result
	const ACTION_TYPE	RESULT_FREE		= 0b00001100;		// Indicates free cell observed
	const ACTION_TYPE	RESULT_OCCUPIED	= 0b00010100;		// Indicates occupied cell observed

	// Action/Result at this node
	class NODE_ACTION
	{
		public:
			NODE_ACTION(const ACTION_TYPE &NewAction) : _Action(NewAction)	{}
			NODE_ACTION() = default;
			NODE_ACTION(const NODE_ACTION &S) = default;
			NODE_ACTION(NODE_ACTION &&S) = default;
			NODE_ACTION &operator=(const NODE_ACTION &S) = default;
			NODE_ACTION &operator=(NODE_ACTION &&S) = default;

			bool IsMoveAction() const		{ return (this->_Action == ACTION_MOVE		? 1:0); }
			bool IsObserveAction() const	{ return (this->_Action == ACTION_OBSERVE	? 1:0); }

			bool IsObserveResult() const	{ return (this->_Action & OBSERVE_RESULT	? 1:0);}
			bool IsCellOccupied() const		{ return (this->_Action == RESULT_OCCUPIED	? 1:0);}
			bool IsCellFree() const			{ return (this->_Action == RESULT_FREE		? 1:0);}

			void SetMoveAction()	{ this->_Action = ACTION_MOVE;		}
			void SetObserveAction()	{ this->_Action = ACTION_OBSERVE;	}

			void SetOccupiedResult() { this->_Action = RESULT_OCCUPIED; }
			void SetFreeResult()	{ this->_Action = RESULT_FREE; }

		private:
			ACTION_TYPE _Action;
	};
}

class MonteCarloNodeData
{
		typedef MONTE_CARLO_NODE_DATA::ACTION_TYPE ACTION_TYPE;
		typedef MONTE_CARLO_NODE_DATA::MAP_ENTROPY MAP_ENTROPY;
	public:
		MonteCarloNodeData(const POS_2D &_NewCell, const ACTION_TYPE &_Action, const MAP_ENTROPY &_RemainingMapEntropy) : Action(_Action), NewCell(_NewCell), IsDone(false), RemainingMapEntropy(_RemainingMapEntropy) {}
		MonteCarloNodeData() = default;
		MonteCarloNodeData(const MonteCarloNodeData &S) = default;
		MonteCarloNodeData(MonteCarloNodeData &&S) = default;
		MonteCarloNodeData &operator=(const MonteCarloNodeData &S) = default;
		MonteCarloNodeData &operator=(MonteCarloNodeData &&S) = default;

		MONTE_CARLO_NODE_DATA::NODE_VALUE	Value;				// Value of this node (used during selection phase)

		MONTE_CARLO_NODE_DATA::NODE_ACTION	Action;				// Action/Result of this node
		POS_2D	NewCell;									// Cell that was observed/moved to

		MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH	ExpectedLength;	// Expected length to dest from here
		MONTE_CARLO_NODE_DATA::CERTAINTY			Certainty;		// Certainty of reaching node from here

		bool	IsDone;										// Have all possible variations beneath this node been checked?
		MONTE_CARLO_NODE_DATA::MAP_ENTROPY	RemainingMapEntropy;// How much entropy is left in node
};

#endif // MONTE_CARLO_NODE_DATA_H
