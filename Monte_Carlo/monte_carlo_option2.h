#ifndef MONTE_CARLO_OPTION2_H
#define MONTE_CARLO_OPTION2_H

/*	class MonteCarloOption2
 *		monte carlo along district edges
 */

#include "standard_definitions.h"
#include "district_map_storage.h"
#include "tree_class.h"
#include "occupancy_grid_map.h"
#include "robot_data.h"

namespace MONTE_CARLO_OPTION2
{
	typedef DISTRICT_MAP::ID	DISTRICT_ID;

	// Action that a robot can take at a node
	typedef unsigned char NODE_ACTION_DATA_TYPE;			// Data type used in node action
	const NODE_ACTION_DATA_TYPE NODE_ACTION_ERROR	= 0b00000000;		// Error value
	const NODE_ACTION_DATA_TYPE NODE_ACTION_MOVE	= 0b00000010;		// Move to adjacent cell
	const NODE_ACTION_DATA_TYPE NODE_ACTION_JUMP	= 0b00000011;		// Jump to position (approximation, can be refined later on)
	const NODE_ACTION_DATA_TYPE NODE_ACTION_OBSERVE = 0b00000100;		// Observe a cell
	const NODE_ACTION_DATA_TYPE NODE_ACTION_RESULT_FREE		= 0b00001000;	// Record a path as free
	const NODE_ACTION_DATA_TYPE NODE_ACTION_RESULT_OCCUPIED = 0b00001001;	// Record a path as occupied
	class NODE_ACTION
	{
		public:
			bool IsMovementAction() const	{ return (this->_Action &  0b00000010 ? 1 : 0); }
			bool IsIncompleteMove() const	{ return (this->_Action == 0b00000011 ? 1 : 0); }
			bool IsObserveAction() const	{ return (this->_Action == 0b00000100 ? 1 : 0); }

			bool IsObserveResult() const	{ return (this->_Action &  0b00001000 ? 1 : 0); }
			bool IsFreeResult() const		{ return (this->_Action == 0b00001000 ? 1 : 0); }
			bool IsOccupiedResult() const	{ return (this->_Action == 0b00001001 ? 1 : 0); }

			void SetAction(const NODE_ACTION_DATA_TYPE Action) { this->_Action = Action; }

			NODE_ACTION(const NODE_ACTION_DATA_TYPE Action) : _Action(Action) {}
			NODE_ACTION() = default;

		private:
			NODE_ACTION_DATA_TYPE _Action;			// Stores the action that will be taken in this node
	};

	// All data stored in one node
	typedef float NODE_VALUE_TYPE;
	typedef float EXPECTED_LENGTH_TYPE;
	typedef float CERTAINTY_TYPE;
	typedef float COST_TYPE;
	typedef float MAP_ENTROPY_TYPE;
	typedef unsigned int NUM_VISIT_TYPE;
	struct NODE_DATA
	{
			NODE_VALUE_TYPE	Value;				// Value of this node (used during selection phase)

			NODE_ACTION	Action;				// Action/Result of this node

			void		*NodeData;					// Extra Node Data stored here
			unsigned int NodeDataSize;		// Size of extra node data

			EXPECTED_LENGTH_TYPE	ExpectedLength;	// Expected length to dest from here
			CERTAINTY_TYPE			Certainty;		// Certainty of reaching node from here

			COST_TYPE		ExpectedCost;	// Cost to reach dest from here
			NUM_VISIT_TYPE	NumVisits;

			bool	IsDone;										// Have all possible variations beneath this node been checked?

			bool IsDeadEnd() const;		// Returns whether this node is a dead end
			void SetToDeadEnd();		// Sets node to dead end

#ifdef DEBUG	// DEBUG
			void PrintNodeData(const unsigned int &NodeDepth) const;
#endif			// ~DEBUG

			// Custom constructors due to NodeData
			NODE_DATA();
			NODE_DATA(const NODE_DATA &S) noexcept;
			NODE_DATA(NODE_DATA &&S) noexcept;
			NODE_DATA &operator=(const NODE_DATA &S) noexcept;
			NODE_DATA &operator=(NODE_DATA &&S) noexcept;
			~NODE_DATA() noexcept;
	};

	// Tree used to store nodes
	typedef TreeClass<NODE_DATA>	TREE_CLASS;
	typedef TREE_CLASS::TREE_NODE	TREE_NODE;

	// Storage of all adjacent district connections that can be moved to
	struct CONNECTION_DATA
	{
		POS_2D ConnectionPos;
		DISTRICT_ID ConnectedID;

		CERTAINTY_TYPE ConnectionCertainty;
	};

	struct CONNECTION_STORAGE : public std::vector<CONNECTION_DATA>
	{
		typedef CONNECTION_DATA CONNECTION_DAT;
	};

	// Data of previously traversed path
	struct TRAVERSED_PATH_SINGLE : public std::array<POS_2D,2>
	{
		bool IsBidirectionalPath(const POS_2D &Pos1, const POS_2D &Pos2) const;
	};
	struct TRAVERSED_PATH : public std::vector<TRAVERSED_PATH_SINGLE>
	{
		bool PathContainsBidirectionalMove(const POS_2D &Pos1, const POS_2D &Pos2) const;
	};

	// Data of current branch
	typedef OccupancyGridMap				OGM_MAP;
	typedef OGM_LOG_MAP_TYPE				LOG_MAP;
	typedef Map2D<NUM_VISIT_TYPE>			NUM_VISIT_MAP;
	typedef DistrictMap						DISTRICT;
	typedef DistrictMapStorage				DISTRICT_STORAGE;
	typedef std::vector<DISTRICT_ID>		VISITED_DISTRICTS;
	typedef VISITED_DISTRICTS::size_type	VISITED_DISTRICTS_ID;
	typedef RobotData						ROBOT_POSITION_DATA;
	struct BRANCH_DATA
	{
		const OGM_MAP *pOriginalMap;				// Original Map
		const DISTRICT_STORAGE *pDistrictStorage;	// Pointer to district data

		TREE_NODE *pCurNode;						// Pointer to current node used

		MAP_ENTROPY_TYPE	RemainingMapEntropy;	// Map Entropy remaining in this node

		OGM_MAP				CurMapData;				// Current Map after branch operations have been executed
		LOG_MAP				CurLogData;				// Current Map (in log form) after branch operations have been executed
		NUM_VISIT_MAP		VisitMap;				// Counter for number of visits to positions in map

		ROBOT_POSITION_DATA	CurBotData;				// Current robot pose

		CONNECTION_STORAGE	AdjacentDistricts;		// Stores all adjacent districts

		VISITED_DISTRICTS		VisitedDistricts;			// Previously visited districs
		VISITED_DISTRICTS_ID	DistrictAfterObservation;	// First district after last observation

		TRAVERSED_PATH		PrevPath;				// Robot path that was taken in this branch

		const DISTRICT &GetCurDistrict() const;		// Returns current district
		DISTRICT_ID GetCurDistrictID() const;		// Returns current district ID

		void StepDownOneNode(const TREE_NODE::CHILD_ID &ChildID);
		void StepUpOneNode();

		void Init(const OGM_MAP &OriginalMap, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData, const DISTRICT_STORAGE &DistrictStorage);

		private:

			void UpdateBranchData(const NODE_DATA &NextNode);	// Move down towrds leaves
			void RevertBranchData(const NODE_DATA &PrevNode);		// Move up towards root
	};
}


class MonteCarloOption2
{
		typedef MONTE_CARLO_OPTION2::TREE_CLASS			TREE_CLASS;
		typedef MONTE_CARLO_OPTION2::TREE_NODE			TREE_NODE;
		typedef TREE_NODE::DATA_TYPE					NODE_DATA;
		typedef MONTE_CARLO_OPTION2::BRANCH_DATA		BRANCH_DATA;
		typedef MONTE_CARLO_OPTION2::DISTRICT_ID		DISTRICT_ID;
		typedef MONTE_CARLO_OPTION2::CONNECTION_STORAGE	CONNECTION_STORAGE;
		typedef CONNECTION_STORAGE::CONNECTION_DAT		CONNECTION_DATA;
	public:
		MonteCarloOption2();

		int Selection();
		void Expansion();
		void Simulation();
		void Backtrack();

	private:
		TREE_CLASS	_MCTree;		// Monte Carlo Tree
		BRANCH_DATA	_Branch;		// Data of current branch
};

#endif // MONTE_CARLO_OPTION2_H
