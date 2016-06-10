#ifndef POLICY_TREE_H
#define POLICY_TREE_H

/*	class PolicyTree
 *		stores the policy tree that should be followed
 */

#include "standard_definitions.h"
#include "tree_class.h"
#include "robot_action.h"
#include "pos_2d.h"

namespace POLICY_TREE
{
	struct NODE_DATA
	{
		RobotAction Action;		// Action that bot should perform at this node
		POS_2D		Position;	// Position at which to perform action

		NODE_DATA(const RobotAction &_Action, const POS_2D &_Position) : Action(_Action), Position(_Position) {}
		NODE_DATA() = default;
		NODE_DATA(const NODE_DATA &S) = default;
		NODE_DATA(NODE_DATA &&S) = default;
		NODE_DATA &operator=(const NODE_DATA &S) = default;
		NODE_DATA &operator=(NODE_DATA &&S) = default;
	};
}

class PolicyTree
{
	public:
		typedef POLICY_TREE::NODE_DATA	POLICY_TREE_NODE_DATA;
		typedef TreeNode<POLICY_TREE_NODE_DATA> POLICY_TREE_NODE;
		typedef TreeClass<POLICY_TREE_NODE_DATA> POLICY_TREE_CLASS;

		void SetTree(const POLICY_TREE_CLASS &NewTree);		// Sets tree data
		void MoveTree(POLICY_TREE_CLASS &&NewTree);			// Moves Tree to this function

		void ResetBotToStart();				// Resets bot to start position

		const RobotAction &GetNextRobotAction() const;		// Returns next robot action
		const POS_2D &GetNextRobotPosition() const;			// Returns position of next robot action
		void GetNextRobotAction(RobotAction &NextBotAction, POS_2D &NextBotPosition) const;

		int MoveToNextBotAction(const RobotAction *CurrentActionResult = NULL);		// Moves to next robot action in tree (requires result of previous operation, is cell occupied or free)
		int ReturnToPreviousBotAction();		// Moves up one node to previous action

	private:

		POLICY_TREE_CLASS		_TreeData;		// Stores policy tree
		const POLICY_TREE_NODE	*_NextActionNode;		// Current position of bot in tree
};

#endif // POLICY_TREE_H
