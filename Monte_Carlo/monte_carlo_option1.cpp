#include "monte_carlo_option1.h"
#include "robot_navigation.h"

int MonteCarloOption1::PerformMonteCarlo(const OccupancyGridMap &OGMap, const POS_2D &StartPos, const POS_2D &Destination)
{
	// Initialize tree
	{
		MONTE_CARLO_NODE *pCurNode = &this->_Tree.GetRoot();
		pCurNode->Reset();

		MonteCarloNodeData nodeData;
		nodeData.Certainty = 0;
		nodeData.ExpectedLength = GetInfiniteVal<MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH>();
		nodeData.IsDone = false;
		nodeData.NewCell = StartPos;
		nodeData.RemainingMapEntropy = OccupancyGridMap::CalculateEntropyFromMap(OGMap);
		nodeData.Value = -GetInfiniteVal<MONTE_CARLO_NODE_DATA::NODE_VALUE>();
		nodeData.Action.SetObserveAction();

		// Add Observe action of start position
		pCurNode = pCurNode->AddChild(nodeData);

		// Add Observe result (start position is free)
		nodeData.Action.SetFreeResult();
		pCurNode = pCurNode->AddChild(nodeData);

		// Add move action ( move bot into start position )
		nodeData.Action.SetMoveAction();
		pCurNode->AddChild(nodeData);
	}

	// Initialize branch data
	this->_CurBranchData.SetMonteCarloObjectives(OGMap, Destination);
	this->_CurBranchData.ResetBranchDataToRoot(this->_Tree);

	// Perform run throughs until done
	const MONTE_CARLO_NODE &rootNode = this->_Tree.GetRoot();
	while(!rootNode.GetData().IsDone)
	{
		Selection();
	}
}

int MonteCarloOption1::Selection()
{
	// Revert to root if not yet there
	if(this->_CurBranchData.pCurNode != &(this->_Tree.GetRoot()))
		this->_CurBranchData.ResetBranchDataToRoot(this->_Tree);

	MONTE_CARLO_NODE *pBestNode = nullptr;

	// Select best leaf
	while(!this->_CurBranchData.pCurNode->IsLeaf())
	{
		MONTE_CARLO_NODE &curNode = *(this->_CurBranchData.pCurNode);
		MONTE_CARLO_NODE::CHILD_ID i = 0;

		// Set first child value that isn't done as best, then compare rest
		while(curNode.GetChild(i)->GetData().IsDone)
		{
			++i;
			if(i>=curNode.GetNumChildren())
				return -1;			// All nodes are done, parent node should have been set to done as well
		}
		pBestNode = curNode.GetChild(i);
		MONTE_CARLO_NODE::CHILD_ID	bestID = i;

		// Select best child of this node
		for(; i<curNode.GetNumChildren(); i++)
		{
			MONTE_CARLO_NODE *const pCurChild = curNode.GetChild(i);

			if(pCurChild->GetData().IsDone)
				continue;

			// If this child is better, select it
			if(pCurChild->GetData().Value > pBestNode->GetData().Value)
			{
				pBestNode = pCurChild;
				bestID = i;
			}
		}

		// Move down to best child
		this->_CurBranchData.MoveDownOneNode(bestID);

		// Continue until leaf is reached
	}

	return 1;
}

int MonteCarloOption1::Expansion()
{
	const POS_2D &curPos = this->_CurBranchData.PathTaken.back();
	bool newNodeAdded = false;

	// Get surrounding cell values
	for(const auto &moveDirection : NavigationOptions)
	{
		const POS_2D adjacentPos = curPos+moveDirection;

		// Check that position is valid
		OGM_LOG_TYPE adjacentProb;
		if(this->_CurBranchData.LogMap.GetPixel(adjacentPos, adjacentProb) < 0)
			continue;

		// Decide what to do on adjacent positions
		if(adjacentProb <= OGM_LOG_MIN)
		{
			// if position is 100% known to be free, move here if not yet recently traversed
			bool positionRecentlyTraversed = false;
			for(auto i=this->_CurBranchData.FirstMoveAfterObservation; i<this->_CurBranchData.PathTaken.size(); i++)
			{
				if(adjacentPos == this->_CurBranchData.PathTaken.at(i))
				{
					positionRecentlyTraversed = true;
					break;
				}
			}
			if(positionRecentlyTraversed)
				continue;

			// Add node for movement
			newNodeAdded = true;
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(adjacentPos, MONTE_CARLO_NODE_DATA::ACTION_MOVE, this->_CurBranchData.pCurNode->GetData().RemainingMapEntropy));
		}
		else if(adjacentProb < OGM_LOG_MAX)
		{
			// if position is unknown, perform observation action
			newNodeAdded = true;
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(adjacentPos, MONTE_CARLO_NODE_DATA::ACTION_OBSERVE, this->_CurBranchData.pCurNode->GetData().RemainingMapEntropy));
		}
		// else cell is occupied, ignore it
	}

	// Return whether new node was added
	if(!newNodeAdded)
		return 0;

	return 1;
}

int MonteCarloOption1::Simulation()
{
	MONTE_CARLO_NODE *const pCurNode = this->_CurBranchData.pCurNode;

	for(MONTE_CARLO_NODE::CHILD_ID i=0; i<pCurNode->GetNumChildren(); i++)
	{
		// Move to this node
		this->_CurBranchData.MoveDownOneNode(i);

		// If this node isn't leaf, return error
		if(!this->_CurBranchData.pCurNode->IsLeaf())
			return -1;

		// If this node is an observation action, add observation results
		if(this->_CurBranchData.GetNodeData().Action.IsObserveAction())
		{
			// Calculate remaining entropy by removing current cell entropy from total
			const MONTE_CARLO_NODE_DATA::MAP_ENTROPY remainingEntropy = this->_CurBranchData.GetNodeData().RemainingMapEntropy - OccupancyGridMap::CalculateEntropyFromCell(this->_CurBranchData.pOGMap->GetPixel(this->_CurBranchData.GetNodeData().NewCell));

			// Add free observation and simulate it
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(this->_CurBranchData.GetNodeData().NewCell, MONTE_CARLO_NODE_DATA::RESULT_FREE, remainingEntropy));
			this->_CurBranchData.MoveDownOneNode(0);
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));
			this->_CurBranchData.MoveUpOneNode();

			// Add occupied observation and simulate it
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(this->_CurBranchData.GetNodeData().NewCell, MONTE_CARLO_NODE_DATA::RESULT_OCCUPIED, remainingEntropy));
			this->_CurBranchData.MoveDownOneNode(1);
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));
			this->_CurBranchData.MoveUpOneNode();

			// Backtrack once from these two nodes to combine simulations
		}
		else // otherwise just run simulation
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));

		// Move up again
		this->_CurBranchData.MoveUpOneNode();
	}
}

int MonteCarloOption1::NodeSimulation(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE &NodeToSimulate)
{
	// Make sure branch and node correspond
	if(BranchData.pCurNode != &NodeToSimulate)
		return -1;

	// The simulation was mostly done through calculating D* map, just read result
}
