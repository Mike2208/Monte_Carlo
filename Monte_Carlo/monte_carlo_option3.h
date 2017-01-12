#ifndef MONTE_CARLO_OPTION3_H
#define MONTE_CARLO_OPTION3_H

/*	class MonteCarloOption3
 *		monte carlo along district edges
 */

#include <memory>

#include "standard_definitions.h"
#include "district_map_storage.h"
#include "tree_class.h"
#include "occupancy_grid_map.h"
#include "robot_data.h"
#include "policy_tree.h"
#include "mc_path_storage.h"
#include "algorithm_a_star.h"
#include "algorithm_d_star.h"
#include "d_star_map.h"
#include "quad_map.h"
#include "quad_d_star_maps.h"

namespace MONTE_CARLO_OPTION3
{
	typedef DISTRICT_MAP::ID			DISTRICT_ID;
	typedef PolicyTree					POLICY_DATA_TYPE;
	typedef MCPathStorage::PATH_DATA	PATH_DATA;

	const QUAD_MAP::ID DESTINATION_ID = GetInfiniteVal<QUAD_MAP::ID>();

	// Action that a robot can take at a node
	typedef unsigned char NODE_ACTION_DATA_TYPE;			// Data type used in node action
	const NODE_ACTION_DATA_TYPE NODE_ACTION_ERROR	= 0b00000000;		// Error value (shouldn't appear)
	const NODE_ACTION_DATA_TYPE NODE_ACTION_MOVE	= 0b00000100;		// Move to adjacent cell
	const NODE_ACTION_DATA_TYPE NODE_ACTION_JUMP	= 0b00000101;		// Jump to position (approximation, can be refined later on)
	const NODE_ACTION_DATA_TYPE NODE_ACTION_MOVE_PATH = 0b00000110;		// Move to a position (definite move, only along paths with obstacle prob. 0)
	const NODE_ACTION_DATA_TYPE NODE_ACTION_OBSERVE = 0b00001000;		// Observe a cell (check if cell is occupied or free)
	const NODE_ACTION_DATA_TYPE NODE_ACTION_RESULT_FREE		= 0b00010000;	// Record a path as free
	const NODE_ACTION_DATA_TYPE NODE_ACTION_RESULT_OCCUPIED = 0b00010001;	// Record a path as occupied
	const NODE_ACTION_DATA_TYPE NODE_ACTION_NONE	= 0b00100000;		// Take no action (usually at leaf)
	class NODE_ACTION
	{
		public:
			bool IsMovementAction() const	{ return (this->_Action &  0b00000100				? 1 : 0); }
			bool IsCompleteMove() const		{ return (this->_Action == NODE_ACTION_MOVE			? 1 : 0); }
			bool IsIncompleteMove() const	{ return (this->_Action == NODE_ACTION_JUMP			? 1 : 0); }
			bool IsMovePath() const			{ return (this->_Action == NODE_ACTION_MOVE_PATH	? 1 : 0); }
			bool IsObserveAction() const	{ return (this->_Action == NODE_ACTION_OBSERVE		? 1 : 0); }

			bool IsObserveResult() const	{ return (this->_Action &  0b00010000					? 1 : 0); }
			bool IsFreeResult() const		{ return (this->_Action == NODE_ACTION_RESULT_FREE		? 1 : 0); }
			bool IsOccupiedResult() const	{ return (this->_Action == NODE_ACTION_RESULT_OCCUPIED	? 1 : 0); }

			bool IsNoAction() const			{ return (this->_Action == NODE_ACTION_NONE);}

			void SetAction(const NODE_ACTION_DATA_TYPE Action)	{ this->_Action = Action; }
			NODE_ACTION_DATA_TYPE GetAction() const				{ return this->_Action; }

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
		DATA_EMPTY, DATA_MOVE, DATA_JUMP, DATA_MOVE_PATH, DATA_RESULT
	};

	// Virtual Object for extra data
	struct NODE_EXTRA_DATA_OBJECT
	{
		virtual ~NODE_EXTRA_DATA_OBJECT() {}

		// Deep Copy and clone functions
		virtual std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const = 0;
		virtual std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const = 0;
	};

	struct NODE_EXTRA_DATA
	{
			// Access extra data
			const NODE_EXTRA_DATA_OBJECT &GetExtraData() const;
			NODE_EXTRA_DATA_OBJECT &GetExtraData();

			void SetExtraData(const NODE_EXTRA_DATA_OBJECT &ExtraData);
			void SetExtraData(std::unique_ptr<NODE_EXTRA_DATA_OBJECT> &&ExtraData);

			void DeleteExtraData();

			// Constructors
			NODE_EXTRA_DATA() = default;
			NODE_EXTRA_DATA(const NODE_EXTRA_DATA_OBJECT &ExtraDataObject);
			NODE_EXTRA_DATA(std::unique_ptr<NODE_EXTRA_DATA_OBJECT> &&ExtraDataObject);

			// Copy operators
			NODE_EXTRA_DATA(const NODE_EXTRA_DATA &S);
			NODE_EXTRA_DATA &operator=(const NODE_EXTRA_DATA &S);

			// Move operators
			NODE_EXTRA_DATA(NODE_EXTRA_DATA &&S) = default;
			NODE_EXTRA_DATA &operator=(NODE_EXTRA_DATA &&S) = default;

		private:

			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> _pExtraData = nullptr;
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

			NODE_VALUE_TYPE	Value = 0;				// Value of this node (used during selection phase)

			EXPECTED_LENGTH_TYPE	ExpectedLength;	// Expected length to dest from here
			CERTAINTY_TYPE			Certainty;		// Certainty of reaching dest from here

			EXPECTED_LENGTH_TYPE	ExpectedLengthPolicy;		// Expected Length until policy completion
			EXPECTED_LENGTH_TYPE	CertaintyPolicy;			// Certainty that policy will be completed

			COST_TYPE		ExpectedCost;	// Cost to reach dest from here
			NUM_VISIT_TYPE	NumVisits = 1;	// Number of times this node was visited by MCTS

			size_t	NumDivisions = 0;					// Number of times quad of bot is divided here
			bool	IsDone = false;						// Have all possible variations beneath this node been checked?

			bool IsDeadEnd() const;		// Returns whether this node is a dead end
			//void SetToDeadEnd();		// Sets node to dead end

			CERTAINTY_TYPE GetCertatintyBeforeNodeAction() const;
			EXPECTED_LENGTH_TYPE GetLengthBeforeNodeAction() const;

#ifdef DEBUG	// DEBUG
			void PrintNodeData(const unsigned int &NodeDepth) const;
#endif			// ~DEBUG

			NODE_DATA() = default;
			NODE_DATA(const NODE_ACTION _Action, const NODE_VALUE_TYPE &_Value, const EXPECTED_LENGTH_TYPE	&_ExpectedLength, const CERTAINTY_TYPE &_Certainty, const COST_TYPE	&_ExpectedCost, const NUM_VISIT_TYPE &_NumVisits, bool	IsDone, NODE_EXTRA_DATA &&_ExtraData);

			NODE_DATA(const NODE_ACTION _Action);
			NODE_DATA(const NODE_ACTION _Action, const NODE_EXTRA_DATA &_ExtraData);
			NODE_DATA(const NODE_ACTION _Action, NODE_EXTRA_DATA &&_ExtraData);

			//void WriteToFile(std::fstream &FileData, int &SavedSize) const;	// Just writes the data in a way that it can be retrieved
			//void ReadFromFile(std::fstream &FileData);		// Read T into NewData from FileData

		private:
	};

	// Constants
	const NODE_VALUE_TYPE MIN_NODE_VALUE = 0;

	// Extra Data in nodes

	// Data for jumping over unscannded node and setting path to 1
	struct NODE_EXTRA_DATA_JUMP final : public virtual NODE_EXTRA_DATA_OBJECT			// Extra data in leaf
	{
		//POS_2D TargetPosition;			// Position we would like to reach
		std::vector<POS_2D> PathData;	// Path Data

		//POS_2D StartPosition;			// Start position of this jump

		//QUAD_MAP::ID NextQuad;

		CERTAINTY_TYPE RecentPathLogCertainty;		// Certainty of path since last observation
		EXPECTED_LENGTH_TYPE RecentPathLength;	// Length of path since last observation

		NODE_EXTRA_DATA_JUMP() = default;
		NODE_EXTRA_DATA_JUMP(NODE_EXTRA_DATA_JUMP &&S) = default;
		NODE_EXTRA_DATA_JUMP &operator=(NODE_EXTRA_DATA_JUMP &&S) = default;
		~NODE_EXTRA_DATA_JUMP() = default;

		NODE_EXTRA_DATA_JUMP(const NODE_EXTRA_DATA_JUMP &S) = default;				// Only allow move to prevent paths being copied
		NODE_EXTRA_DATA_JUMP &operator=(const NODE_EXTRA_DATA_JUMP &S) = default;		// Only allow move to prevent paths being copied

		private:

			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;

			friend NODE_EXTRA_DATA;
	};

	// Move along already scanned path to target position
	struct NODE_EXTRA_DATA_MOVE_PATH final : public NODE_EXTRA_DATA_OBJECT
	{
		public:
			POS_2D TargetPosition;			// Position we would like to reach
			MOVE_DIST_TYPE PathLength;		// Length of path to target position

			NODE_EXTRA_DATA_MOVE_PATH() = default;
			NODE_EXTRA_DATA_MOVE_PATH(const POS_2D &_TargetPos, const MOVE_DIST_TYPE _PathLength) : TargetPosition(_TargetPos), PathLength(_PathLength)
			{}
			~NODE_EXTRA_DATA_MOVE_PATH() = default;

		private:

			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;
	};

	struct OBSTACLE_DATA : public POS_2D
	{
		CERTAINTY_TYPE OccupancyPercentage;		// Certainty of occupying this space

		OBSTACLE_DATA(const POS_2D &_Position, const CERTAINTY_TYPE &_OccupancyPercentage);
	};

	// Update obstacle data
	struct NODE_EXTRA_DATA_OBSTACLE final : public std::vector<OBSTACLE_DATA>, private virtual NODE_EXTRA_DATA_OBJECT			// Contains all positions of obstacle and their corresonding observed percentages
	{
			NODE_EXTRA_DATA_OBSTACLE() = default;
			NODE_EXTRA_DATA_OBSTACLE(std::vector<OBSTACLE_DATA> &&Obstacles) : std::vector<OBSTACLE_DATA>(std::move(Obstacles)) {}

		private:
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;
	};

	struct NODE_EXTRA_DATA_OBSTACLE_SINGLE final : public virtual NODE_EXTRA_DATA_OBJECT			// Contains all positions of obstacle and their corresonding observed percentages
	{
			POS_2D ObstacleCell;			// Position of obstacle
			OGM_DISCRETE_TYPE IsOccupied;		// Is cell occupied or free

			NODE_EXTRA_DATA_OBSTACLE_SINGLE(const POS_2D &_Position, OGM_DISCRETE_TYPE _IsOccupied) : ObstacleCell(_Position), IsOccupied(_IsOccupied)
			{}

		private:
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;
	};

	struct NODE_EXTRA_DATA_OBSERVE_ACTION : public virtual NODE_EXTRA_DATA_OBJECT
	{
			POS_2D ObservationPoint;		// Point that is observed
			OGM_LOG_TYPE ObservationFreeProbabilityLog;			// Log Probability that pos is free

			NODE_EXTRA_DATA_OBSERVE_ACTION() = default;
			NODE_EXTRA_DATA_OBSERVE_ACTION(const POS_2D _ObservationPoint, const OGM_LOG_TYPE _ObservationFreeProbabilityLog) : ObservationPoint(_ObservationPoint), ObservationFreeProbabilityLog(_ObservationFreeProbabilityLog)
			{}
			NODE_EXTRA_DATA_OBSERVE_ACTION(const NODE_EXTRA_DATA_OBSERVE_ACTION &S) = default;
			NODE_EXTRA_DATA_OBSERVE_ACTION(NODE_EXTRA_DATA_OBSERVE_ACTION &&S) = default;
			~NODE_EXTRA_DATA_OBSERVE_ACTION() = default;

		private:
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;
	};

	// Move to an adjacent cell
	struct NODE_EXTRA_DATA_MOVE final : public virtual NODE_EXTRA_DATA_OBJECT					// Contains robot move orders in order
	{
			POS_2D NewPos;

			NODE_EXTRA_DATA_MOVE(const POS_2D &MoveDestination);
			NODE_EXTRA_DATA_MOVE() = default;
			~NODE_EXTRA_DATA_MOVE() = default;

		private:
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Create() const override;
			std::unique_ptr<NODE_EXTRA_DATA_OBJECT> Clone() const override;
	};

	// Tree used to store nodes
	typedef TreeClass<NODE_DATA>	TREE_CLASS;
	typedef TREE_CLASS::TREE_NODE	TREE_NODE;

//	// Storage of all adjacent district connections that can be moved to
//	struct CONNECTION_DATA
//	{
//		POS_2D ConnectionPos;
//		DISTRICT_ID ConnectedID;

//		CERTAINTY_TYPE ConnectionCertainty;
//	};

//	struct CONNECTION_STORAGE : public std::vector<CONNECTION_DATA>
//	{
//		typedef CONNECTION_DATA CONNECTION_DAT;
//	};

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
		bool PartialPathContainsBidirectionalMove(const POS_2D &Pos1, const POS_2D &Pos2, const ID &StartPathID) const;
	};

	// Data of current branch
	typedef OccupancyGridMap				OGM_MAP;
	typedef Map2D<OGM_PROB_TYPE>			PROB_MAP;
	const PROB_MAP::CELL_TYPE				MAX_CERTAINTY	= OGM_PROB_MAX;
	const PROB_MAP::CELL_TYPE				MIN_CERTAINTY	= OGM_PROB_MIN;
	const PROB_MAP::CELL_TYPE				CELL_OCCUPIED	= MAX_CERTAINTY;
	const PROB_MAP::CELL_TYPE				CELL_FREE		= MIN_CERTAINTY;
	const TREE_NODE::CHILD_ID				OBSERVE_RESULT_FREE_ID = 0;
	const TREE_NODE::CHILD_ID				OBSERVE_RESULT_OCCUPIED_ID = 1;
	typedef float							DIST_TYPE;
	typedef OGM_LOG_MAP_TYPE				LOG_MAP;
	typedef Map2D<NUM_VISIT_TYPE>			NUM_VISIT_MAP;
	typedef DistrictMap						DISTRICT;
	typedef DistrictMapStorage				DISTRICT_STORAGE;
	typedef std::vector<DISTRICT_ID>		VISITED_DISTRICTS;
	typedef VISITED_DISTRICTS::size_type	VISITED_DISTRICTS_ID;
	typedef RobotData						ROBOT_POSITION_DATA;
	typedef Map2D<DIST_TYPE>				D_STAR_DIST_MAP;		// D* Map for distance
	typedef DStarMap<OGM_LOG_TYPE>			D_STAR_PROB_MAP;		// D* Map for probability
	typedef Map2D<OGM_LOG_TYPE>				D_STAR_RATIO_MAP;		// D* Map for dist/reliability ratio
	struct BRANCH_DATA
	{
		//typedef OGM_MAP::CELL_TYPE  OGM_CELL_TYPE;
		typedef PROB_MAP::CELL_TYPE PROB_CELL_TYPE;
		typedef PROB_MAP PROBABILITY_MAP;

		const OGM_MAP *pOriginalMap;				// Original Map

		//LOG_MAP				CurLogData;				// Current Map (in log form) after branch operations have been executed
		LOG_MAP				CurCertaintyLogData;	// Current Map (in reverse log form) after branch operations have been executed
//		NUM_VISIT_MAP		VisitMap;				// Counter for number of visits to positions in map

		TREE_NODE *pCurNode;						// Pointer to current node used

		MAP_ENTROPY_TYPE	RemainingMapEntropy;	// Map Entropy remaining in this node

		QuadDStarMaps		DStarMaps;				// Quad map and DStar maps to destination/from current bot position

		ROBOT_POSITION_DATA	CurBotData;				// Current robot pose

		std::vector<POS_2D>	PrevPath;				// Previously visited paths
		std::vector<QuadMap::ID>	VisitedQuads;	// Quads visited since last observation
		std::vector<POS_2D>	PosToUpdate;			// Positions to update to resync D* maps

		POS_2D Start;								// Start position of robot
		POS_2D Destination;							// Destination to reach

		MCPathStorage *pPathStorage;				// Pointer to path storage

		POS_2D ObservationPoint;					// Saves a previous observation point
		TREE_NODE::CHILD_ID ObservationID;			// Saves whether this was a free or occupied observation
		bool PrevNodeWasObserveAction = false;		// Saves whether previous node was an observation action

		void StepDownOneNodeSimple(const TREE_NODE::CHILD_ID &ChildID);			// Only update curNode
		void StepDownOneNodeSync();												// Sync again after stepping down
		void StepDownOneNode(const TREE_NODE::CHILD_ID &ChildID);
		//void SetStepDownToUnsynced();
		void StepUpOneNodeSimple();

		bool IsNodeSynced() const;

		void SyncDStarMaps(const bool SyncDestMaps = true, const bool SyncStartMaps = true);							// Sync up D* maps with path data
		void SimulateObserveObservation(const POS_2D &Position, const OGM_DISCRETE_TYPE CellOccupied);		// Simulate whether a cell is occupied or free

		void Init(const OGM_MAP &OriginalMap, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData);

		CERTAINTY_TYPE MinPathCertainty = 0.5;		// Certainty threshold after which obstacle is said to be detected
		EXPECTED_LENGTH_TYPE MinPathLength = 20;		// Length threshold after which obstacle is said to be detected

		EXPECTED_LENGTH_TYPE ObstacleLength = 1;	// Length of obstacle that should be created (perpendicular to bot direction)
		EXPECTED_LENGTH_TYPE ObstacleHeight = 1;	// Height of obstacle that should be created (parallel to bot direction)
		CERTAINTY_TYPE MinObstacleCertainty = MAX_CERTAINTY;	// Minimum certainty that a created obstacle should have
		CERTAINTY_TYPE MaxObstacleCertainty = MAX_CERTAINTY;	// Maximum certatinty that a created obstacle should have

		EXPECTED_LENGTH_TYPE MinMoveFraction = 0.5;		// Fraction of total path that an incomplete move must be for it to be explored before anything else

		NODE_VALUE_TYPE Constant = 1;				// Constant in node value calc
		COST_TYPE MoveActionCost = 1;				// Cost of a move action
		COST_TYPE ObserveActionCost = 1;			// Cost of an observe action

		BRANCH_DATA(const OGM_MAP &MapData, RobotData &_RobotData, MCPathStorage *const PathStorage);
		BRANCH_DATA(const BRANCH_DATA &S) = default;
		BRANCH_DATA(BRANCH_DATA &&S) = default;
		BRANCH_DATA &operator=(const BRANCH_DATA &S) = default;
		BRANCH_DATA &operator=(BRANCH_DATA &&S) = default;

		private:

			void SetMapPositionFromCertaintyLogValue(const POS_2D &Position, const OGM_LOG_TYPE &CertaintyValue);			// Set a map position to either 1 or 0
			void SetMapPositionDiscrete(const POS_2D &Position, const bool &NewValue);		// Set a map position to either 1 or 0

			//void UpdateBranchData(const TREE_NODE &NextNode);	// Move down towards leaves
			void RevertBranchDataSimple();						// Simple move up, can't be reversed!

			bool _StepDownPossible = true;						// Check to make sure a step down is possible
			char _StepDownSynced = true;						// Is the current node synced with the current data?
	};
}


class MonteCarloOption3
{
		typedef MONTE_CARLO_OPTION3::TREE_CLASS			TREE_CLASS;
		typedef MONTE_CARLO_OPTION3::TREE_NODE			TREE_NODE;
		typedef TREE_NODE::DATA_TYPE					NODE_DATA;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_OBJECT NODE_EXTRA_DATA_OBJECT;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA NODE_EXTRA_DATA;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_JUMP NODE_EXTRA_DATA_JUMP;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_OBSTACLE NODE_EXTRA_DATA_OBSTACLE;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_OBSTACLE_SINGLE NODE_EXTRA_DATA_OBSTACLE_SINGLE;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_MOVE NODE_EXTRA_DATA_MOVE;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_MOVE_PATH NODE_EXTRA_DATA_MOVE_PATH;
		typedef MONTE_CARLO_OPTION3::NODE_EXTRA_DATA_OBSERVE_ACTION NODE_EXTRA_DATA_OBSERVE_ACTION;
		typedef MONTE_CARLO_OPTION3::BRANCH_DATA		BRANCH_DATA;
		typedef MONTE_CARLO_OPTION3::DISTRICT_ID		DISTRICT_ID;
		typedef MCPathStorage::PATH_DATA				PATH_DATA;
		typedef MONTE_CARLO_OPTION3::EXPECTED_LENGTH_TYPE		EXPECTED_LENGTH_TYPE;
		typedef MONTE_CARLO_OPTION3::CERTAINTY_TYPE		CERTAINTY_TYPE;
		typedef MONTE_CARLO_OPTION3::COST_TYPE		COST_TYPE;
		typedef MONTE_CARLO_OPTION3::NODE_VALUE_TYPE	NODE_VALUE_TYPE;
		typedef MONTE_CARLO_OPTION3::NUM_VISIT_TYPE		NUM_VISITS_TYPE;
		typedef MONTE_CARLO_OPTION3::DISTRICT_STORAGE	DISTRICT_STORAGE;
	public:
		MonteCarloOption3(const OccupancyGridMap &Map, RobotData _RobotData, const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths);

		int PerformMonteCarlo(const POS_2D &StartPos, const POS_2D &Destination);

		int Selection();
		int Expansion();
		int Simulation();
		int Backtrack();

	private:
		TREE_CLASS	_MCTree;		// Monte Carlo Tree
		BRANCH_DATA	_Branch;		// Data of current branch
		MCPathStorage _PathStorage;	// Path Storage

		void SimulateCurrentLeaf();
		void SimulateBranch(BRANCH_DATA &BranchData);
		void Backtrack_Step();

		// Extra functions
		//int CalculatePath(const BRANCH_DATA &BranchData, const POS_2D &StartPos, const POS_2D &Destination, PATH_DATA *const PathTaken, EXPECTED_LENGTH_TYPE *const ExpectedLength, CERTAINTY_TYPE *const Certainty, COST_TYPE *const Cost);		// Calculate path taken
		//int FollowPathUntilObstacle(const BRANCH_DATA &BranchData, const PATH_DATA &PathToTake, const CERTAINTY_TYPE &MinCertainty, const EXPECTED_LENGTH_TYPE &MinLength, const EXPECTED_LENGTH_TYPE &ScanRange, const POS_2D &StartPos, const CERTAINTY_TYPE &StartCertainty, const EXPECTED_LENGTH_TYPE &StartLength, const PATH_DATA::const_iterator *const NextForcedObstacleIterator, CERTAINTY_TYPE *const PathCertainty, EXPECTED_LENGTH_TYPE *const PathLength, PATH_DATA::ID *const EndPosID, POS_2D *const ObstaclePos);		// Follows path until path certainty is below threshold AND path length is above threshold
		NODE_EXTRA_DATA_OBSTACLE &&CreateObstacleAtPos(const BRANCH_DATA &BranchData, const POS_2D &ObstaclePosition, const POS_2D &BotPosition, const EXPECTED_LENGTH_TYPE &ObstacleLength, const EXPECTED_LENGTH_TYPE &ObstacleHeight, const CERTAINTY_TYPE &MinObstacleCertainty, const CERTAINTY_TYPE &MaxObstacleCertainty);				// Create an obstacle
		//void CalculateMoveNodeData(const BRANCH_DATA &BranchData, NODE_DATA &NodeToCalculate);

		std::vector<NODE_DATA> ExpandCurrentNode(const BRANCH_DATA &BranchData, const POS_2D &CurPos, const POS_2D &NextPos);			// Create node data for adjacent cells
		void ExpandScanNode(TREE_NODE &ScanNode);

		//void RunSimulation(const POS_2D &BotPos, const POS_2D &Destination, const BRANCH_DATA &MapData, const NUM_VISITS_TYPE &NumParentsVisit, NODE_DATA &Result);		// Run the simulation from the given position, then save the data in node data
		void CalculateNodeValue(const NODE_DATA &Data, const NODE_VALUE_TYPE &Constant, const NUM_VISITS_TYPE &NumParentsVisit, NODE_VALUE_TYPE &Value);

		inline bool CompareIncompleteMoveAndPath(const CERTAINTY_TYPE MoveCertainty, const EXPECTED_LENGTH_TYPE MoveLength, const CERTAINTY_TYPE PathCertainty, const EXPECTED_LENGTH_TYPE PathLength);			// Compares incomplete move with entire path to determine which is better to explore (1 means incomplete move, 0 means path)

		void PerformBacktrackStep(TREE_NODE &NodeToBacktrackTo);								// Perform one backtrack step to given node
		const TREE_NODE *FindBestChildNode(const TREE_NODE &CurNode, bool &AllDone) const;		// Selects the best child node out of all available ones
		NODE_VALUE_TYPE CalculateComparableChildValue(const NODE_DATA &ChildNodeData) const;			// Calculates the value of this child in backtrack steps
};

#endif // MONTE_CARLO_OPTION3_H
