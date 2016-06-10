#include "policy_monte_carlo_converter.h"
#include <queue>

struct BRANCHING_NODE
{
	const MONTE_CARLO_NODE *pMonteCarloNode;
	PolicyTree::POLICY_TREE_NODE *pPolicyNode;

	BRANCHING_NODE(const MONTE_CARLO_NODE *_pMonteCarloNode, PolicyTree::POLICY_TREE_NODE *_pPolicyNode) : pMonteCarloNode(_pMonteCarloNode), pPolicyNode(_pPolicyNode) {}
};

void PolicyMonteCarloConverter::ConvertMonteCarloToPolicyTree(const MONTE_CARLO_TREE_CLASS &MonteCarloTree, PolicyTree &NewPolTree)
{
	PolicyTree::POLICY_TREE_CLASS policyData;
	const MONTE_CARLO_NODE::DATA_TYPE *pCurData;
	PolicyTree::POLICY_TREE_NODE *pNextPolicyNode;

	// Start at root of monte carlo tree and continue through tree
	const MONTE_CARLO_NODE *pCurMonteCarloNode = &(MonteCarloTree.GetRoot());
	PolicyTree::POLICY_TREE_NODE  *pCurPolicyNode = &(policyData.GetRoot());

	// Skip over first two children, they are unnecessary for policy
	pCurMonteCarloNode = pCurMonteCarloNode->GetChild(0);
	pCurMonteCarloNode = pCurMonteCarloNode->GetChild(0);
	pCurData = &(pCurMonteCarloNode->GetData());

	pCurPolicyNode->SetData(POLICY_TREE::NODE_DATA(pCurData->Action, pCurData->NewCell));

	std::vector<BRANCHING_NODE> nodesToRevertTo;

	// Add nodes to policy tree
	while(1)		// Break condition after inner while loop
	{
		// Go through the tree depth first and add nodes
		while(pCurMonteCarloNode->GetNumChildren() > 0)
		{
			// Add child node to policy, and move to
			pCurData = &(pCurMonteCarloNode->GetChild(0)->GetData());
			pNextPolicyNode = pCurPolicyNode->AddChild(POLICY_TREE::NODE_DATA(pCurData->Action, pCurData->NewCell));

			// If this is a branching node, store the branch for later
			if(pCurData->Action.IsObserveAction())
			{
				nodesToRevertTo.push_back(BRANCHING_NODE(pCurMonteCarloNode->GetChild(1), pCurPolicyNode));
			}

			// Move to child
			pCurPolicyNode = pNextPolicyNode;
			pCurMonteCarloNode = pCurMonteCarloNode->GetChild(0);
		}

		if(nodesToRevertTo.size() <= 0)
			break;			// Break if no more branches need to be traversed

		// After going through one leaf, revert to last branch and go down other branch
		pCurMonteCarloNode = nodesToRevertTo.back().pMonteCarloNode;
		pCurPolicyNode = nodesToRevertTo.back().pPolicyNode;

		// Add separating branch and move down it
		pCurData = &(pCurMonteCarloNode->GetChild(1)->GetData());
		pCurPolicyNode		= pCurPolicyNode->AddChild(POLICY_TREE::NODE_DATA(pCurData->Action, pCurData->NewCell));
		pCurMonteCarloNode	= pCurMonteCarloNode->GetChild(1);

		nodesToRevertTo.pop_back();
	}

	NewPolTree.MoveTree(std::move(policyData));		// Move tree to new storage
}
