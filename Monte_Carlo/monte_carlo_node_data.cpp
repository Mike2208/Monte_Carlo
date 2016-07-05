#include "monte_carlo_node_data.h"

#ifdef DEBUG	// DEBUG
#include <iostream>
#endif			// ~DEBUG


bool MonteCarloNodeData::IsDeadEnd() const
{
	// If certainty to reach dest is zero, this is dead end
	if(this->Certainty <= OGM_PROB_MIN)
		return true;

	return false;
}

void MonteCarloNodeData::SetToDeadEnd()
{
	this->Certainty = 0;
	this->ExpectedLength = GetInfiniteVal<MONTE_CARLO_NODE_DATA::EXPECTED_LENGTH>();
	this->ExpectedCost = GetInfiniteVal<MONTE_CARLO_NODE_DATA::COST_TYPE>();
	this->IsDone = true;

	this->Value = -GetInfiniteVal<MONTE_CARLO_NODE_DATA::NODE_VALUE>();		// Dead end value
}

#ifdef DEBUG	// DEBUG
void MonteCarloNodeData::PrintNodeData(const unsigned int &NodeDepth) const
{
	std::cout << PrintTabs(NodeDepth) << " " << "Node Cell: " << this->NewCell << std::endl;
	std::cout << PrintTabs(NodeDepth) << " " << "Action: " << this->Action << std::endl;
	std::cout << PrintTabs(NodeDepth) << " " << "Done: " << this->IsDone << std::endl;
	std::cout << PrintTabs(NodeDepth) << " " << "Expected Length: " << this->ExpectedLength << std::endl;
	std::cout << PrintTabs(NodeDepth) << " " << "Certainty: " << this->Certainty << std::endl;
}
#endif			// ~DEBUG
