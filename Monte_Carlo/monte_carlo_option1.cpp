#include "monte_carlo_option1.h"
#include "robot_navigation.h"

int MonteCarloOption1::PerformMonteCarlo(const OccupancyGridMap &OGMap, const POS_2D &StartPos, const POS_2D &Destination)
{
	this->_MoveCost = 1;
	this->_ObserveCost = 1;

	// Initialize tree
	{
		MONTE_CARLO_NODE *pCurNode = &this->_Tree.GetRoot();
		pCurNode->Reset();

		MonteCarloNodeData nodeData;
		nodeData.Action.SetObserveAction();
		nodeData.Certainty = 0;
		nodeData.ExpectedCost = GetInfiniteVal<MONTE_CARLO_NODE_DATA::COST_TYPE>();
		nodeData.ExpectedLength = GetInfiniteVal<MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH>();
		nodeData.IsDone = false;
		nodeData.NewCell = StartPos;
		nodeData.RemainingMapEntropy = OccupancyGridMap::CalculateEntropyFromMap(OGMap);
		nodeData.Value = -GetInfiniteVal<MONTE_CARLO_NODE_DATA::NODE_VALUE>();
		nodeData.NumVisits = 0;

		// Set root to Observe action of start position
		pCurNode->SetData(nodeData);

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
		Expansion();
		Simulation();
		Backtrack();
	}

#ifdef DEBUG	// DEBUG
	std::cout << "New Tree:" << std::endl;
	this->PrintTree();
#endif			// ~DEBUG

	return 1;
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
		for(++i; i<curNode.GetNumChildren(); ++i)
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
	const POS_2D &curPos = this->_CurBranchData.GetBotPos();
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
			for(auto i=this->_CurBranchData.FirstPosAfterObservation; i<this->_CurBranchData.PathTaken.size(); i++)
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
	{
		this->_CurBranchData.pCurNode->GetDataR().SetToDeadEnd();	// This node is a dead end, can't reach destination from here
		return 0;
	}

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
			MONTE_CARLO_NODE_DATA::MAP_ENTROPY remainingEntropy = this->_CurBranchData.GetNodeData().RemainingMapEntropy - OccupancyGridMap::CalculateEntropyFromCell(this->_CurBranchData.pOGMap->GetPixel(this->_CurBranchData.GetNodeData().NewCell));
			if(remainingEntropy < 0)
				remainingEntropy = 0;	// remove rounding errors

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
			this->PerformBacktrackStep(*(this->_CurBranchData.pCurNode));
		}
		else // otherwise just run simulation
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));

		// Move up again
		this->_CurBranchData.MoveUpOneNode();
	}

	return 1;
}

int MonteCarloOption1::Backtrack()
{
	MONTE_CARLO_NODE *pNodeToBacktrackTo = this->_CurBranchData.pCurNode;

	while(!this->_CurBranchData.AtRootNode())
	{
		// Perform backtrack operation
		this->PerformBacktrackStep(*pNodeToBacktrackTo);

		// Move data up to parent
		this->_CurBranchData.MoveUpOneNode();
		pNodeToBacktrackTo = this->_CurBranchData.pCurNode;
	}

	// Once more, for root node

	// Perform backtrack operation
	this->PerformBacktrackStep(*pNodeToBacktrackTo);

//	// Move data up to parent
//	this->_CurBranchData.MoveUpOneNode();
//	pNodeToBacktrackTo = this->_CurBranchData.pCurNode;

	return 1;
}

int MonteCarloOption1::NodeSimulation(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE &NodeToSimulate)
{
	// Make sure branch and node correspond
	if(BranchData.pCurNode != &NodeToSimulate)
		return -1;

	MonteCarloNodeData &curData = NodeToSimulate.GetDataR();
	curData.NumVisits = 1;		// first visit

	// Check if start position is reachable
	curData.Certainty = OccupancyGridMap::CalculateCertaintyFromLog(BranchData.DStarCostMap.MapData().GetPixel(BranchData.GetBotPos()));
	if(curData.Certainty <= OGM_PROB_MIN)
		this->SetNodeToDeadEnd(curData);
	else
	{
		// If not dead end, calculate expected length
		this->NodeSimulation_CalculateExpectedLength(BranchData, curData.ExpectedLength);

		// Calculate expected cost (TODO: Add number of uncertain cells * ObservationCost)
		curData.ExpectedCost = curData.ExpectedLength * this->_MoveCost;

		// Calculate node value
		this->CalculateNodeValue(NodeToSimulate, curData.Value);

		// Check if done ( this can be improved through checking if current path is also minLength path)
		if(curData.RemainingMapEntropy <= 0 ||
				BranchData.GetBotPos() == BranchData.Destination)
			curData.IsDone = true;
		else
			curData.IsDone = false;
	}

	return 1;
}

int MonteCarloOption1::NodeSimulation_CalculateExpectedLength(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH &ExpectedLength) const
{
	// Hack for transfer from unsigned int -> float
	typename DStarMap<OGM_LOG_TYPE>::PATH_LENGTH_TYPE tmpExpecptedLength;
	bool destReachable = (BranchData.DStarCostMap.CalculateMinCostPathLength(BranchData.GetBotPos(), tmpExpecptedLength, &(OGM_LOG_MAX)) >= 0 ? 1:0 );
	ExpectedLength = tmpExpecptedLength;

	if(!destReachable)
		return -1;

	return 1;
}

void MonteCarloOption1::CalculateNodeValue(const MONTE_CARLO_NODE &Node, MONTE_CARLO_NODE_DATA::NODE_VALUE &Value) const
{
	// If this node is finished, set node value to bad
	MonteCarloNodeData const &nodeData = Node.GetData();
	if(nodeData.IsDone || nodeData.ExpectedLength <= 0 || nodeData.Certainty <= OGM_PROB_MIN)
		Value = -GetInfiniteVal<MONTE_CARLO_NODE_DATA::NODE_VALUE>();
	else
	{
		Value = nodeData.Certainty/nodeData.ExpectedLength;
	}
}

void MonteCarloOption1::SetNodeToDeadEnd(MonteCarloNodeData &NodeData)
{
	NodeData.SetToDeadEnd();
}

int MonteCarloOption1::PerformBacktrackStep(MONTE_CARLO_NODE &NodeToBacktrackTo)
{
	// Check that node corresponds to branch data
	if(&(NodeToBacktrackTo) != this->_CurBranchData.pCurNode)
		return -1;

	// If no children where created, just simulate this node
	if(NodeToBacktrackTo.GetNumChildren() == 0)
	{
		//this->NodeSimulation(this->_CurBranchData, NodeToBacktrackTo);
		return 0;
	}

	bool allPathsDone = true;

	// Sort Children according to expected length
	NodeToBacktrackTo.SortChildren(MONTE_CARLO_NODE_COMPARE());

	MonteCarloNodeData &curData = NodeToBacktrackTo.GetDataR();

	// Determine current node action, and select proper backtrack method
	if(NodeToBacktrackTo.GetData().Action.IsObserveAction())
	{
		curData.ExpectedLength = 0;
		curData.ExpectedCost = 0;
		curData.Certainty = 0;

		// For observation action, check that no more than two children are available (only possible results are occupied/unoccupied)
		if(NodeToBacktrackTo.GetNumChildren() > 2)
			return -1;

		// Go throug both children
		for(const auto &curChildNode : NodeToBacktrackTo.GetStorage())
		{
			const MonteCarloNodeData &curChildData = curChildNode.GetData();

			// Check whether path is finished
			if(!curChildData.IsDone)
				allPathsDone = false;

			if(curChildData.IsDeadEnd())
				continue;		// Skip this one if it is dead end

			// Calculate new expected length and cost
			curData.ExpectedLength += (OGM_PROB_MAX - curData.Certainty)*curChildData.ExpectedLength;
			curData.ExpectedCost += (OGM_PROB_MAX - curData.Certainty)*curChildData.ExpectedCost;

			// Calculate new certainty
			OGM_PROB_TYPE cellProb = OccupancyGridMap::CalculateProbValFromCell(this->_CurBranchData.pOGMap->GetPixel(curChildData.NewCell));
			if(curChildData.Action.IsCellFree())
				curData.Certainty += (OGM_PROB_MAX-cellProb)*curChildData.Certainty;
			else
				curData.Certainty += (cellProb)*curChildData.Certainty;
		}

		curData.ExpectedCost += this->_ObserveCost;
	}
	else
	{
		// If node action isn't observe, just take values of best node
		const MonteCarloNodeData &bestChildData = NodeToBacktrackTo.GetChild(0)->GetData();

		curData.ExpectedLength = bestChildData.ExpectedLength;
		curData.ExpectedCost = bestChildData.ExpectedCost;
		curData.Certainty = bestChildData.Certainty;

		if(curData.Action.IsMoveAction())
		{
			curData.ExpectedLength += 1;
			curData.ExpectedCost += this->_MoveCost;
		}

		// Go through all children and check if all paths are done
		for(const auto &curChildNode : NodeToBacktrackTo.GetStorage())
		{
			if(!curChildNode.GetData().IsDone)
				allPathsDone = false;
		}
	}

	// increment visit counter
	curData.NumVisits++;

	// Check if all paths are done
	if(allPathsDone)
		curData.IsDone = true;
	else
		curData.IsDone = false;

	// Recalculate node value
	this->CalculateNodeValue(NodeToBacktrackTo, curData.Value);

	return 1;
}
