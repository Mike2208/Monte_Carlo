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
#include "policy_tree.h"
#include "mc_path_storage.h"
#include "algorithm_a_star.h"

namespace MONTE_CARLO_OPTION2
{
	typedef DISTRICT_MAP::ID			DISTRICT_ID;
	typedef PolicyTree					POLICY_DATA_TYPE;
	typedef MCPathStorage::PATH_DATA	PATH_DATA;

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
			bool IsCompleteMove() const		{ return (this->_Action == 0b00000010 ? 1 : 0); }
			bool IsIncompleteMove() const	{ return (this->_Action == 0b00000011 ? 1 : 0); }
			bool IsObserveAction() const	{ return (this->_Action == 0b00000100 ? 1 : 0); }

			bool IsObserveResult() const	{ return (this->_Action &  0b00001000 ? 1 : 0); }
			bool IsFreeResult() const		{ return (this->_Action == 0b00001000 ? 1 : 0); }
			bool IsOccupiedResult() const	{ return (this->_Action == 0b00001001 ? 1 : 0); }

			void SetAction(const NODE_ACTION_DATA_TYPE Action) { this->_Action = Action; }
			NODE_ACTION_DATA_TYPE GetAction() const { return this->_Action; }

			NODE_ACTION(const NODE_ACTION_DATA_TYPE Action) : _Action(Action) {}
			NODE_ACTION() = default;
			NODE_ACTION &operator=(const NODE_ACTION_DATA_TYPE Action) { this->_Action = Action; return *this; }

			bool operator==(const NODE_ACTION &S)const { return (this->_Action == S._Action); }
			bool operator!=(const NODE_ACTION &S)const { return (this->_Action != S._Action); }

		private:
			NODE_ACTION_DATA_TYPE _Action;			// Stores the action that will be taken in this node
	};

	// Tree used to store nodes
	struct NODE_DATA;
//	typedef TreeClass<NODE_DATA>	TREE_CLASS;
//	typedef TREE_CLASS::TREE_NODE	TREE_NODE;

	// Extra data of nodes
	enum NODE_EXTRA_DATA_TYPE
	{
		DATA_EMPTY, DATA_OBSTACLE, DATA_LEAF, DATA_MOVE
	};

	struct NODE_EXTRA_DATA_MOVE;			// Extra data to store move orders
	struct NODE_EXTRA_DATA_OBSTACLE;		// Extra data used to create obstacle on maps
	struct NODE_EXTRA_DATA_LEAF;			// Extra data in leaf
	struct NODE_EXTRA_DATA
	{
		const NODE_EXTRA_DATA_LEAF *GetExtraLeafData() const;
		NODE_EXTRA_DATA_LEAF *GetExtraLeafData();
		//void SetExtraData(const NODE_EXTRA_DATA_LEAF &ExtraLeafData);
		void SetExtraData(NODE_EXTRA_DATA_LEAF &&ExtraLeafData);

		const NODE_EXTRA_DATA_OBSTACLE *GetExtraObstacleData() const;
		NODE_EXTRA_DATA_OBSTACLE *GetExtraObstacleData();
		//void SetExtraData(const NODE_EXTRA_DATA_OBSTACLE &ExtraObstacleData);
		void SetExtraData(NODE_EXTRA_DATA_OBSTACLE &&ExtraObstacleData);

		const NODE_EXTRA_DATA_MOVE *GetExtraMoveData() const;
		NODE_EXTRA_DATA_MOVE *GetExtraMoveData();
		//void SetExtraData(const NODE_EXTRA_DATA_MOVE &ExtraMoveData);
		void SetExtraData(NODE_EXTRA_DATA_MOVE &&ExtraMoveData);

		void DeleteExtraData();

		// Custom constructors due to NODE_EXTRA_DATA memory allocation
		NODE_EXTRA_DATA();
		//NODE_EXTRA_DATA(const NODE_EXTRA_DATA &S) noexcept = delete;
		NODE_EXTRA_DATA(NODE_EXTRA_DATA &&S) noexcept;
		//NODE_EXTRA_DATA &operator=(const NODE_EXTRA_DATA &S) noexcept = delete;
		NODE_EXTRA_DATA &operator=(NODE_EXTRA_DATA &&S) noexcept;
		~NODE_EXTRA_DATA() noexcept;

		//NODE_EXTRA_DATA(const NODE_EXTRA_DATA_LEAF &_LeafData);
		NODE_EXTRA_DATA(NODE_EXTRA_DATA_LEAF &&_LeafData) noexcept;
		//NODE_EXTRA_DATA(const NODE_EXTRA_DATA_OBSTACLE &_ObstacleData);
		NODE_EXTRA_DATA(NODE_EXTRA_DATA_OBSTACLE &&_ObstacleData);
		//NODE_EXTRA_DATA(const NODE_EXTRA_DATA_MOVE &_MoveData);
		NODE_EXTRA_DATA(NODE_EXTRA_DATA_MOVE &&_MoveData);

		private:
			void *ExtraData;					// Storage of extra data
			NODE_EXTRA_DATA_TYPE DataType;		// Extra data type

			void ChangeDataType(const NODE_EXTRA_DATA_TYPE &NewType);

			// Typesafe deletes
			void DeleteExtraObstacleData();
			void DeleteExtraLeafData();
			void DeleteExtraMoveData();

			void SetExtraData(const NODE_EXTRA_DATA_LEAF &ExtraLeafData);
			void SetExtraData(const NODE_EXTRA_DATA_OBSTACLE &ExtraObstacleData);
			void SetExtraData(const NODE_EXTRA_DATA_MOVE &ExtraMoveData);

			NODE_EXTRA_DATA(const NODE_EXTRA_DATA &S) noexcept;
			NODE_EXTRA_DATA &operator=(const NODE_EXTRA_DATA &S) noexcept;

			NODE_EXTRA_DATA(const NODE_EXTRA_DATA_LEAF &_LeafData);
			NODE_EXTRA_DATA(const NODE_EXTRA_DATA_OBSTACLE &_ObstacleData);
			NODE_EXTRA_DATA(const NODE_EXTRA_DATA_MOVE &_MoveData);

			friend NODE_DATA;
			friend NODE_EXTRA_DATA_LEAF;
	};

	// All data stored in one node
	typedef float NODE_VALUE_TYPE;
	typedef float EXPECTED_LENGTH_TYPE;
	typedef float CERTAINTY_TYPE;
	typedef float COST_TYPE;
	typedef float MAP_ENTROPY_TYPE;
	typedef unsigned int NUM_VISIT_TYPE;
	struct NODE_DATA : public NODE_EXTRA_DATA
	{
			NODE_ACTION	Action;				// Action/Result of this node

			NODE_VALUE_TYPE	Value;				// Value of this node (used during selection phase)

			EXPECTED_LENGTH_TYPE	ExpectedLength;	// Expected length to dest from here
			CERTAINTY_TYPE			Certainty;		// Certainty of reaching dest from here

			COST_TYPE		ExpectedCost;	// Cost to reach dest from here
			NUM_VISIT_TYPE	NumVisits;

			POS_2D			Position;		// Position this node is referring to

			bool	IsDone;										// Have all possible variations beneath this node been checked?

			bool IsDeadEnd() const;		// Returns whether this node is a dead end
			void SetToDeadEnd();		// Sets node to dead end

#ifdef DEBUG	// DEBUG
			void PrintNodeData(const unsigned int &NodeDepth) const;
#endif			// ~DEBUG

			NODE_DATA() = default;
			NODE_DATA(const NODE_ACTION _Action, const NODE_VALUE_TYPE &_Value, const EXPECTED_LENGTH_TYPE	&_ExpectedLength, const CERTAINTY_TYPE &_Certainty, const COST_TYPE	&_ExpectedCost, const NUM_VISIT_TYPE &_NumVisits, const POS_2D &_Position, bool	IsDone, NODE_EXTRA_DATA &&_ExtraData);

			NODE_DATA(const NODE_ACTION _Action, const POS_2D &_Position);
			NODE_DATA(const NODE_ACTION _Action, const POS_2D &_Position, const NODE_EXTRA_DATA &_ExtraData);
			NODE_DATA(const NODE_ACTION _Action, const POS_2D &_Position, NODE_EXTRA_DATA &&_ExtraData);

		private:

			friend NODE_EXTRA_DATA;
	};

	// Constants
	const NODE_VALUE_TYPE MIN_NODE_VALUE = 0;

	// Extra Data in nodes
	struct NODE_EXTRA_DATA_LEAF				// Extra data in leaf
	{
		POS_2D TargetPosition;			// Position we would like to reach
		POS_2D FollowUpPosition;		// Position we would like to move to after reaching target (used for switching districts)

		CERTAINTY_TYPE RecentPathCertainty;		// Certainty of path since last observation
		EXPECTED_LENGTH_TYPE RecentPathLength;	// Length of path since last observation

		MC_PATH_STORAGE::PATH_ID StoredPathID;	// ID of stored path
		MCPathStorage *PathStorage;				// Pointer to path storage

		void SetExtraData(const NODE_EXTRA_DATA &ExtraData);
		void SetExtraData(NODE_EXTRA_DATA &&ExtraData);
		const NODE_EXTRA_DATA *GetExtraData() const;
		NODE_EXTRA_DATA *GetExtraData();

		NODE_EXTRA_DATA_LEAF() = default;
		NODE_EXTRA_DATA_LEAF(NODE_EXTRA_DATA_LEAF &&S) = default;
		NODE_EXTRA_DATA_LEAF &operator=(NODE_EXTRA_DATA_LEAF &&S) = default;
		~NODE_EXTRA_DATA_LEAF();

		explicit operator const NODE_EXTRA_DATA_MOVE*() const;
		explicit operator NODE_EXTRA_DATA_MOVE*();
		explicit operator const NODE_EXTRA_DATA_OBSTACLE*() const;
		explicit operator NODE_EXTRA_DATA_OBSTACLE*();

		private:
			NODE_EXTRA_DATA *ExtraData = nullptr;			// Pointer to more data in leaf

			NODE_EXTRA_DATA_LEAF(const NODE_EXTRA_DATA_LEAF &S);				// Only allow move to prevent paths being copied
			NODE_EXTRA_DATA_LEAF &operator=(const NODE_EXTRA_DATA_LEAF &S);		// Only allow move to prevent paths being copied

			friend NODE_EXTRA_DATA;
	};

	struct OBSTACLE_DATA : public POS_2D
	{
		CERTAINTY_TYPE OccupancyPercentage;		// Certainty of occupying this space
	};

	struct NODE_EXTRA_DATA_OBSTACLE : public std::vector<OBSTACLE_DATA>			// Contains all positions of obstacle and their corresonding observed percentages
	{
	};

	struct NODE_EXTRA_DATA_MOVE : public PATH_DATA					// Contains robot move orders in order
	{
		NODE_EXTRA_DATA_MOVE(const PATH_DATA &TotalPath, const PATH_DATA::ID &NextPosInPathID, const PATH_DATA::ID &EndPosInPathID);
		NODE_EXTRA_DATA_MOVE() = default;
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

		TRAVERSED_PATH_SINGLE(const POS_2D &Pos1, const POS_2D &Pos2) : std::array<POS_2D,2>({{Pos1, Pos2}}) {}
	};
	struct TRAVERSED_PATH : public std::vector<TRAVERSED_PATH_SINGLE>
	{
		typedef TRAVERSED_PATH::size_type ID;
		bool PathContainsBidirectionalMove(const POS_2D &Pos1, const POS_2D &Pos2) const;
	};

	// Data of current branch
	typedef OccupancyGridMap				OGM_MAP;
	typedef Map2D<OGM_PROB_TYPE>			PROB_MAP;
	const PROB_MAP::CELL_TYPE				MAX_CERTAINTY	= OGM_PROB_MAX;
	const PROB_MAP::CELL_TYPE				MIN_CERTAINTY	= OGM_PROB_MIN;
	const PROB_MAP::CELL_TYPE				CELL_OCCUPIED	= MAX_CERTAINTY;
	const PROB_MAP::CELL_TYPE				CELL_FREE		= MIN_CERTAINTY;
	typedef OGM_LOG_MAP_TYPE				LOG_MAP;
	typedef Map2D<NUM_VISIT_TYPE>			NUM_VISIT_MAP;
	typedef DistrictMap						DISTRICT;
	typedef DistrictMapStorage				DISTRICT_STORAGE;
	typedef std::vector<DISTRICT_ID>		VISITED_DISTRICTS;
	typedef VISITED_DISTRICTS::size_type	VISITED_DISTRICTS_ID;
	typedef RobotData						ROBOT_POSITION_DATA;
	struct BRANCH_DATA
	{
		//typedef OGM_MAP::CELL_TYPE  OGM_CELL_TYPE;
		typedef PROB_MAP::CELL_TYPE PROB_CELL_TYPE;
		typedef PROB_MAP PROBABILITY_MAP;

		const OGM_MAP *pOriginalMap;				// Original Map
		const DISTRICT_STORAGE *pDistrictStorage;	// Pointer to district data

		TREE_NODE *pCurNode;						// Pointer to current node used

		MAP_ENTROPY_TYPE	RemainingMapEntropy;	// Map Entropy remaining in this node

		PROBABILITY_MAP		CurMapData;				// Current Map after branch operations have been executed
		LOG_MAP				CurLogData;				// Current Map (in log form) after branch operations have been executed
		NUM_VISIT_MAP		VisitMap;				// Counter for number of visits to positions in map

		ROBOT_POSITION_DATA	CurBotData;				// Current robot pose

//		CONNECTION_STORAGE	AdjacentDistricts;		// Stores all adjacent districts

//		VISITED_DISTRICTS		VisitedDistricts;			// Previously visited districs
//		VISITED_DISTRICTS_ID	DistrictAfterObservation;	// First district after last observation

		TRAVERSED_PATH		PrevPath;				// Robot path that was taken in this branch
		TRAVERSED_PATH::ID	PathsAfterObservation;	// First path after last observation

		TRAVERSED_PATH		FailedPaths;			// Paths that have already failed and shouldn't be tried again
		TRAVERSED_PATH		SuccessPaths;			// Paths that have already succeeded and can be traversed again quickly

		TRAVERSED_PATH_SINGLE *pCurJumpPath;			// If currently in jump path, that is tored here

		const DISTRICT &GetCurDistrict() const;		// Returns current district
		DISTRICT_ID GetCurDistrictID() const;		// Returns current district ID
		DISTRICT_ID GetDistrictIDAtPos(const POS_2D &Position) const;

		MCPathStorage StoredPathData;				// Temporary Storage of paths

		const PROB_CELL_TYPE &GetCurMapData(const POS_2D &Position) const { return this->CurMapData.GetPixel(Position); }

		void StepDownOneNode(const TREE_NODE::CHILD_ID &ChildID);
		void StepUpOneNode();

		void Init(const OGM_MAP &OriginalMap, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData, const DISTRICT_STORAGE &DistrictStorage);

		BRANCH_DATA(const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths);

		CERTAINTY_TYPE MinPathCertainty;		// Certainty threshold after which obstacle is said to be detected
		EXPECTED_LENGTH_TYPE MinPathLength;		// Length threshold after which obstacle is said to be detected

		EXPECTED_LENGTH_TYPE ObstacleLength;	// Length of obstacle that should be created (perpendicular to bot direction)
		EXPECTED_LENGTH_TYPE ObstacleHeight;	// Height of obstacle that should be created (parallel to bot direction)
		CERTAINTY_TYPE MinObstacleCertainty;	// Minimum certainty that a created obstacle should have
		CERTAINTY_TYPE MaxObstacleCertainty;	// Maximum certatinty that a created obstacle should have

		NODE_VALUE_TYPE Constant;				// Constant in node value calc
		COST_TYPE MoveActionCost;				// Cost of a move action
		COST_TYPE ObserveActionCost;			// Cost of an observe action

		AlgorithmAStar	AStarMap;				// Map for A* Calculations

		private:

			void SetMapPosition(const POS_2D &Position, const OGM_MAP::CELL_TYPE &NewValue);
			void SetMapPosition(const POS_2D &Position, const CERTAINTY_TYPE &NewValue);

			void UpdateBranchData(const TREE_NODE &NextNode);	// Move down towrds leaves
			void RevertBranchData(const TREE_NODE &PrevNode);		// Move up towards root
	};
}


class MonteCarloOption2
{
		typedef MONTE_CARLO_OPTION2::TREE_CLASS			TREE_CLASS;
		typedef MONTE_CARLO_OPTION2::TREE_NODE			TREE_NODE;
		typedef TREE_NODE::DATA_TYPE					NODE_DATA;
		typedef MONTE_CARLO_OPTION2::NODE_EXTRA_DATA NODE_EXTRA_DATA;
		typedef MONTE_CARLO_OPTION2::NODE_EXTRA_DATA_LEAF NODE_EXTRA_DATA_LEAF;
		typedef MONTE_CARLO_OPTION2::NODE_EXTRA_DATA_OBSTACLE NODE_EXTRA_DATA_OBSTACLE;
		typedef MONTE_CARLO_OPTION2::NODE_EXTRA_DATA_MOVE NODE_EXTRA_DATA_MOVE;
		typedef MONTE_CARLO_OPTION2::BRANCH_DATA		BRANCH_DATA;
		typedef MONTE_CARLO_OPTION2::DISTRICT_ID		DISTRICT_ID;
		typedef MONTE_CARLO_OPTION2::CONNECTION_STORAGE	CONNECTION_STORAGE;
		typedef CONNECTION_STORAGE::CONNECTION_DAT		CONNECTION_DATA;
		typedef MCPathStorage::PATH_DATA				PATH_DATA;
		typedef MONTE_CARLO_OPTION2::EXPECTED_LENGTH_TYPE		EXPECTED_LENGTH_TYPE;
		typedef MONTE_CARLO_OPTION2::CERTAINTY_TYPE		CERTAINTY_TYPE;
		typedef MONTE_CARLO_OPTION2::COST_TYPE		COST_TYPE;
		typedef MONTE_CARLO_OPTION2::NODE_VALUE_TYPE	NODE_VALUE_TYPE;
		typedef MONTE_CARLO_OPTION2::NUM_VISIT_TYPE		NUM_VISITS_TYPE;
	public:
		MonteCarloOption2(const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths);

		int Selection();
		void Expansion();
		void Simulation();
		void Backtrack();

	private:
		TREE_CLASS	_MCTree;		// Monte Carlo Tree
		BRANCH_DATA	_Branch;		// Data of current branch

		void SimulateCurrentLeaf();
		void Backtrack_Step();

		// Extra functions
		int CalculatePath(const BRANCH_DATA &BranchData, const POS_2D &StartPos, const POS_2D &Destination, PATH_DATA *const PathTaken, EXPECTED_LENGTH_TYPE *const ExpectedLength, CERTAINTY_TYPE *const Certainty, COST_TYPE *const Cost);		// Calculate path taken
		int FollowPathUntilObstacle(const BRANCH_DATA &BranchData, const PATH_DATA &PathToTake, const CERTAINTY_TYPE &MinCertainty, const EXPECTED_LENGTH_TYPE &MinLength, CERTAINTY_TYPE *const PathCertainty, EXPECTED_LENGTH_TYPE *const PathLength, PATH_DATA::ID *const EndPosID, POS_2D *const ObstaclePos);		// Follows path until path certainty is below threshold AND path length is above threshold
		NODE_EXTRA_DATA_OBSTACLE CreateObstacleAtPos(const POS_2D &ObstaclePosition, const POS_2D &BotPosition, const EXPECTED_LENGTH_TYPE &ObstacleLength, const EXPECTED_LENGTH_TYPE &ObstacleHeight, const CERTAINTY_TYPE &MinObstacleCertainty, const CERTAINTY_TYPE &MaxObstacleCertainty);
		void CalculateMoveNodeData(const BRANCH_DATA &BranchData, NODE_DATA &NodeToCalculate);

		void RunSimulation(const POS_2D &BotPos, const POS_2D &Destination, const BRANCH_DATA &MapData, const NUM_VISITS_TYPE &NumParentsVisit, NODE_DATA &Result);		// Run the simulation from the given position, then save the data in node data
		void CalculateNodeValue(const NODE_DATA &Data, const NODE_VALUE_TYPE &Constant, const NUM_VISITS_TYPE &NumParentsVisit, NODE_VALUE_TYPE &Value);

		const TREE_NODE *FindBestChildNode(const TREE_NODE &CurNode, bool &AllDone) const;		// Selects the best child node out of all available ones
		NODE_VALUE_TYPE CalculateComparableChildValue(const NODE_DATA &ChildNodeData) const;			// Calculates the value of this child in backtrack steps
};

#endif // MONTE_CARLO_OPTION2_H
