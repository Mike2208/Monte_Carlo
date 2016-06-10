#include "policy_tree.h"

void PolicyTree::SetTree(const POLICY_TREE_CLASS &NewTree)
{
	this->_TreeData = NewTree;
	this->ResetBotToStart();
}

void PolicyTree::MoveTree(POLICY_TREE_CLASS &&NewTree)
{
	this->_TreeData = std::move(NewTree);
}

void PolicyTree::ResetBotToStart()
{
	this->_NextActionNode = &(this->_TreeData.GetRoot());
}

const RobotAction &PolicyTree::GetNextRobotAction() const
{
	return this->_NextActionNode->GetData().Action;
}

const POS_2D &PolicyTree::GetNextRobotPosition() const
{
	return this->_NextActionNode->GetData().Position;
}

void PolicyTree::GetNextRobotAction(RobotAction &NextBotAction, POS_2D &NextBotPosition) const
{
	NextBotAction = this->_NextActionNode->GetData().Action;
	NextBotPosition = this->_NextActionNode->GetData().Position;
}

int PolicyTree::MoveToNextBotAction(const RobotAction *CurrentActionResult)
{
	// If no result was given, only advance if there is no branching decision here
	if(CurrentActionResult == NULL)
	{
		if(this->_NextActionNode->GetNumChildren() == 1)
		{
			this->_NextActionNode = this->_NextActionNode->GetChild(0);
			return 0;
		}
		else
			return -1;		// Error, the result of the previous action needs to be given
	}
	else
	{
		// Go through children and find corresponding result
		for(const auto &curChild : this->_NextActionNode->GetStorage())
		{
			if(curChild.GetData().Action == *CurrentActionResult)
			{
				this->_NextActionNode = &(curChild);
				return 1;
			}
		}

		return -2;		// Error, given result does not correspond to any stored observations
	}
}

int PolicyTree::ReturnToPreviousBotAction()
{
	// Check if this is first action of policy tree
	if(this->_NextActionNode->GetParent() == NULL)
		return -1;

	this->_NextActionNode = this->_NextActionNode->GetParent();
	return 1;
}
