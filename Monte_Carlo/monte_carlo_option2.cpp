#include "monte_carlo_option2.h"
#include "robot_navigation.h"

#include <time.h>

MonteCarloOption2::MonteCarloOption2(const std::vector<OccupancyGridMap> &Maps) : _CurBranchData(Maps.at(0).GetWidth(), Maps.at(0).GetHeight(), _PrevBranches), _StartBranch(Maps.at(0).GetWidth(), Maps.at(0).GetHeight(), _PrevBranches)
{
}

int MonteCarloOption2::PerformMonteCarlo(const POS_2D &StartPos, const POS_2D &Destination, const float ExploreParam, const CheckConditions &StopConditions, const char *const PolicyFileName, PolicyData &NewPolicy)
{
	this->_MoveCost = 1;
	this->_ObserveCost = 1;

	this->_CurBranchData.Constant = ExploreParam;

	// Initialize tree
	{
		MONTE_CARLO_NODE *pCurNode = &this->_Tree.GetRoot();
		pCurNode->Reset();

		MonteCarloNodeData nodeData;
		nodeData.Action.SetObserveAction();
		nodeData.Certainty = 0;
		nodeData.PolicyCertainty = 0;
		nodeData.ExpectedCost = GetInfiniteVal<MONTE_CARLO_NODE_DATA::COST_TYPE>();
		nodeData.ExpectedLength = GetInfiniteVal<MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH>();
		nodeData.PolicyExpectedLength = GetInfiniteVal<MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH>();
		nodeData.IsDone = false;
		nodeData.NewCell = StartPos;
		//nodeData.RemainingMapEntropy = this->_CurBranchData. OccupancyGridMap::CalculateE(OGMaps);
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
	this->_CurBranchData.SetMonteCarloObjectives(*this->_CurBranchData.pOGMaps, Destination);
	this->_CurBranchData.ResetBranchDataToRoot(this->_Tree);

	this->_StartBranch = this->_CurBranchData;
	this->_StartPos = StartPos;
	this->_DestPos = Destination;

	// Perform run throughs until done
	const auto startTime = time(nullptr);
	const MONTE_CARLO_NODE &rootNode = this->_Tree.GetRoot();
	const auto &rootData = rootNode.GetData();
	while(!StopConditions.AreConditionsMet(rootData.IsDone, rootData.NumVisits, time(nullptr)-startTime, rootData.PolicyCertainty, rootData.Certainty, rootData.MapCertainty))
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

	NewPolicy = this->CreatePolicyFromTree(PolicyFileName);

	return 1;
}

int MonteCarloOption2::Selection()
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

int MonteCarloOption2::Expansion()
{
	const POS_2D &curPos = this->_CurBranchData.GetBotPos();
	bool newNodeAdded = false;

	// Get surrounding cell values
	for(const auto &moveDirection : NavigationOptions)
	{
		const POS_2D adjacentPos = curPos+moveDirection;

		// Check that position is valid
		OGM_LOG_TYPE adjacentProb;
		if(this->_CurBranchData.CertaintyLogMap.GetPixel(adjacentPos, adjacentProb) < 0)
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
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(adjacentPos, ROBOT_ACTION::ACTION_MOVE, this->_CurBranchData.pCurNode->GetData().RemainingMapEntropy));
		}
		else if(adjacentProb < OGM_LOG_MAX)
		{
			// if position is unknown, perform observation action
			newNodeAdded = true;
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(adjacentPos, ROBOT_ACTION::ACTION_OBSERVE, this->_CurBranchData.pCurNode->GetData().RemainingMapEntropy));
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

int MonteCarloOption2::Simulation()
{
	MONTE_CARLO_NODE *const pCurNode = this->_CurBranchData.pCurNode;

	this->_CurBranchData.UpdateCostMap();

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
			MONTE_CARLO_NODE_DATA::MAP_ENTROPY remainingEntropy = this->_CurBranchData.GetNodeData().RemainingMapEntropy - OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateProbValueFromCertaintyLog(this->_CurBranchData.CertaintyLogMap.GetPixel(this->_CurBranchData.GetNodeData().NewCell)));
			if(remainingEntropy < 0)
				remainingEntropy = 0;	// remove rounding errors

			// Add free observation and simulate it
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(this->_CurBranchData.GetNodeData().NewCell, ROBOT_ACTION::RESULT_FREE, remainingEntropy));
			this->_CurBranchData.MoveDownOneNode(0);
			this->_CurBranchData.UpdateCostMap();
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));
			this->_CurBranchData.MoveUpOneNode();

			// Add occupied observation and simulate it
			this->_CurBranchData.pCurNode->AddChild(MonteCarloNodeData(this->_CurBranchData.GetNodeData().NewCell, ROBOT_ACTION::RESULT_OCCUPIED, remainingEntropy));
			this->_CurBranchData.MoveDownOneNode(1);
			this->_CurBranchData.UpdateCostMap();
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));
			this->_CurBranchData.MoveUpOneNode();

			// Backtrack once from these two nodes to combine simulations
			this->PerformBacktrackStep(*(this->_CurBranchData.pCurNode));
		}
		else // otherwise just run simulation
		{
			this->_CurBranchData.UpdateCostMap();
			this->NodeSimulation(this->_CurBranchData, *(this->_CurBranchData.pCurNode));
		}

		// Move up again
		this->_CurBranchData.MoveUpOneNode();
	}

	return 1;
}

int MonteCarloOption2::Backtrack()
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

int MonteCarloOption2::NodeSimulation(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE &NodeToSimulate)
{
	// Make sure branch and node correspond
	if(BranchData.pCurNode != &NodeToSimulate)
		return -1;

	MonteCarloNodeData &curData = NodeToSimulate.GetDataR();
	curData.NumVisits = 1;		// first visit

	// Check if start position is reachable
	curData.Certainty = OccupancyGridMap::CalculateCertaintyProbFromLog(BranchData.DStarCostMap.GetDStarRatioCertaintyMapToGoal().GetPixel(BranchData.GetBotPos()));
	if(curData.Certainty <= OGM_PROB_MIN)
	{
		this->SetNodeToDeadEnd(curData);

		curData.PolicyCertainty = 1;
		curData.PolicyExpectedLength = 0;
	}
	else
	{
		// If not dead end, calculate expected length
		this->NodeSimulation_CalculateExpectedLength(BranchData, curData.ExpectedLength);

		// Calculate expected cost (TODO: Add number of uncertain cells * ObservationCost)
		curData.ExpectedCost = curData.ExpectedLength * this->_MoveCost;

		// Calculate node value
		this->CalculateNodeValue(NodeToSimulate, curData.Value);

		curData.PolicyCertainty = curData.Certainty;
		curData.PolicyExpectedLength = curData.ExpectedLength;

		// Check if done ( this can be improved through checking if current path is also minLength path)
		if(curData.PolicyCertainty >= 1 ||
				BranchData.GetBotPos() == BranchData.Destination)
			curData.IsDone = true;
		else
			curData.IsDone = false;
	}

	curData.MapCertainty = 0;
	for(const auto curCert : BranchData.MapCertaintiesNormalized)
	{
		if(curData.MapCertainty < curCert)
			curData.MapCertainty = curCert;
	}

	return 1;
}

int MonteCarloOption2::NodeSimulation_CalculateExpectedLength(const MonteCarloBranchData &BranchData, MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH &ExpectedLength) const
{
	// Hack for transfer from unsigned int -> float
	ExpectedLength = BranchData.DStarCostMap.GetDStarRatioDistMapToGoal().GetPixel(BranchData.GetBotPos());

	// Return error if length is infinity
	if(ExpectedLength >= OGM_LOG_MAX)
		return -1;

	return 1;
}

void MonteCarloOption2::CalculateNodeValue(const MONTE_CARLO_NODE &Node, MONTE_CARLO_NODE_DATA::NODE_VALUE &Value) const
{
	// If this node is finished, set node value to bad
	MonteCarloNodeData const &nodeData = Node.GetData();
	auto numParentsVisit = nodeData.NumVisits;
	if(Node.GetParent() != nullptr)
		numParentsVisit = Node.GetParent()->GetData().NumVisits;
	if(nodeData.IsDone || nodeData.PolicyExpectedLength <= 0 || nodeData.PolicyCertainty >= OGM_PROB_MAX)
		Value = -GetInfiniteVal<MONTE_CARLO_NODE_DATA::NODE_VALUE>();
	else
	{
		Value = -(nodeData.ExpectedLength/nodeData.Certainty + this->_CurBranchData.Constant * std::sqrt(static_cast<double>(nodeData.NumVisits)/numParentsVisit));
	}
}

void MonteCarloOption2::SetNodeToDeadEnd(MonteCarloNodeData &NodeData)
{
	NodeData.SetToDeadEnd();
}

int MonteCarloOption2::PerformBacktrackStep(MONTE_CARLO_NODE &NodeToBacktrackTo)
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
		curData.PolicyExpectedLength = 0;
		curData.PolicyCertainty = 0;
		curData.ExpectedLength = 0;
		curData.ExpectedCost = 0;
		curData.Certainty = 0;

		curData.MapCertainty = 0;

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
			curData.PolicyExpectedLength += (OGM_PROB_MAX - curData.PolicyCertainty)*curChildData.PolicyExpectedLength;
			curData.ExpectedCost += (OGM_PROB_MAX - curData.Certainty)*curChildData.ExpectedCost;

			// Calculate new certainty
			OGM_PROB_TYPE cellProb = OccupancyGridMap::CalculateProbValueFromCertaintyLog(this->_CurBranchData.CertaintyLogMap.GetPixel(curChildData.NewCell));
			if(curChildData.Action.IsCellFree())
			{
				curData.Certainty += (OGM_PROB_MAX-cellProb)*curChildData.Certainty;
				curData.PolicyCertainty += (OGM_PROB_MAX-cellProb)*curChildData.PolicyCertainty;

				curData.MapCertainty += (OGM_PROB_MAX-cellProb)*curChildData.MapCertainty;
			}
			else
			{
				curData.Certainty += (cellProb)*curChildData.Certainty;
				curData.PolicyCertainty += (cellProb)*curChildData.PolicyCertainty;

				curData.MapCertainty += cellProb*curChildData.MapCertainty;
			}
		}

		curData.ExpectedCost += this->_ObserveCost;
	}
	else
	{
		// If node action isn't observe, just take values of best node
		const MonteCarloNodeData &bestChildData = NodeToBacktrackTo.GetChild(0)->GetData();

		curData.PolicyCertainty = bestChildData.PolicyCertainty;
		curData.PolicyExpectedLength = bestChildData.PolicyExpectedLength;
		curData.ExpectedLength = bestChildData.ExpectedLength;
		curData.ExpectedCost = bestChildData.ExpectedCost;
		curData.Certainty = bestChildData.Certainty;

		curData.MapCertainty = bestChildData.MapCertainty;

		if(curData.Action.IsMoveAction())
		{
			curData.PolicyExpectedLength += 1;
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

PolicyData MonteCarloOption2::CreatePolicyFromTree(const char *FileName)
{
	auto branch = this->_StartBranch;

	// Convert tree to policy
	PolicyData::TREE_NODE rootNode;

	// Create header
	const auto &rootData = branch.pCurNode->GetData();
	PolicyData::FILE_HEADER header;
	header.DestPos = this->_DestPos;
	header.StartPos = this->_StartPos;
	header.GoalCertainty = rootData.Certainty;
	header.LengthToGoal = rootData.ExpectedLength;
	header.MapHeight = branch.CertaintyLogMap.GetHeight();
	header.MapWidth = branch.CertaintyLogMap.GetWidth();
	header.LengthToPolicyCompletion = rootData.PolicyExpectedLength;
	header.PolicyCertainty = rootData.PolicyCertainty;

	// Convert current node to policy node
	this->CreatePolicyFromTreeStep(branch, rootNode);

	// Write policy to file and return it
	PolicyData newPolicy;
	newPolicy.WritePolicyToFile(FileName, header, rootNode);
	return newPolicy;
}

void MonteCarloOption2::CreatePolicyFromTreeStep( MonteCarloBranchData &Branch, PolicyData::TREE_NODE &PolicyNode )
{
	this->_PrevBranches.clear();

	auto *&ppCurNode = Branch.pCurNode;
	const auto *pCurLeafData = &ppCurNode->GetData();

	auto *pPolData = &PolicyNode.GetDataR();

	// Get map state
	OccupancyGridMap::CalculateProbMapFromCertaintyLogMap(pPolData->MapState, Branch.CertaintyLogMap);

	// Find next observe action or leaf
	while(!pCurLeafData->Action.IsObserveAction() && !ppCurNode->IsLeaf())
	{
		// Move down to best child
		Branch.MoveDownOneNode(0);
		pCurLeafData = &ppCurNode->GetData();
	}

	if(pCurLeafData->Action.IsObserveAction())
	{
		// At observe action, create observation
		pPolData->MovePos = Branch.GetBotPos();
		pPolData->ScanPos = pCurLeafData->NewCell;

		// Create two children
		auto *pFreeNode = PolicyNode.AddChild(PolicyData::NODE_DATA());
		auto *pOccupiedNode = PolicyNode.AddChild(PolicyData::NODE_DATA());

		// Add two children
		auto occupiedBranch = Branch;

		if(ppCurNode->GetChild(0)->GetData().Action.IsCellFree())
		{
			// Free node
			Branch.MoveDownOneNode(0);
			this->CreatePolicyFromTreeStep(Branch, *pFreeNode);

			// Occupied node
			occupiedBranch.MoveDownOneNode(1);
			this->CreatePolicyFromTreeStep(occupiedBranch, *pOccupiedNode);
		}
		else
		{
			// Free node
			Branch.MoveDownOneNode(1);
			this->CreatePolicyFromTreeStep(Branch, *pFreeNode);

			// Occupied node
			occupiedBranch.MoveDownOneNode(0);
			this->CreatePolicyFromTreeStep(occupiedBranch, *pOccupiedNode);
		}
	}
	else
	{
		// At leaf, store data
		pPolData->MovePos = Branch.GetBotPos();
		pPolData->ScanPos = Branch.Destination;

		pPolData->MapID = 0;
		float bestCert = 0;
		for(size_t curMapID = 0; curMapID < Branch.MapCertaintiesNormalized.size(); ++curMapID)
		{
			if(Branch.MapCertaintiesNormalized[curMapID] > bestCert)
			{
				bestCert = Branch.MapCertaintiesNormalized[curMapID];
				pPolData->MapID = curMapID;
			}
		}

		// At leaf, compute policy output
		if(pCurLeafData->PolicyCertainty >= OGM_PROB_MAX)
		{
			if(pCurLeafData->Certainty >= OGM_PROB_MAX)
				pPolData->PolicyState = POLICY_DATA::POLICY_SUCCESS;		// Return success
			else
				pPolData->PolicyState = POLICY_DATA::POLICY_BLOCKED;		// Return no path available
		}
		else
		{
			pPolData->PolicyState = POLICY_DATA::POLICY_UNKNOWN;		// Return error
		}
	}
}
