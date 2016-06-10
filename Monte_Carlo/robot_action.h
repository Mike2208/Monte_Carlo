#ifndef ROBOT_ACTION_H
#define ROBOT_ACTION_H

namespace ROBOT_ACTION
{
	typedef char ACTION_TYPE;
	const ACTION_TYPE	ACTION_MOVE		= 0b00000001;		// Indicates Move action
	const ACTION_TYPE	ACTION_OBSERVE	= 0b00000010;		// Indicates Observe action
	const ACTION_TYPE	OBSERVE_RESULT	= 0b00000100;		// Indicates observation result
	const ACTION_TYPE	RESULT_FREE		= 0b00001100;		// Indicates free cell observed
	const ACTION_TYPE	RESULT_OCCUPIED	= 0b00010100;		// Indicates occupied cell observed
}

// Action/Result at this node
class RobotAction
{
	public:
		RobotAction(const ROBOT_ACTION::ACTION_TYPE &NewAction) : _Action(NewAction)	{}
		RobotAction() = default;
		RobotAction(const RobotAction &S) = default;
		RobotAction(RobotAction &&S) = default;
		RobotAction &operator=(const RobotAction &S) = default;
		RobotAction &operator=(RobotAction &&S) = default;

		bool IsMoveAction() const		{ return (this->_Action == ROBOT_ACTION::ACTION_MOVE	? 1:0); }
		bool IsObserveAction() const	{ return (this->_Action == ROBOT_ACTION::ACTION_OBSERVE	? 1:0); }

		bool IsObserveResult() const	{ return (this->_Action & ROBOT_ACTION::OBSERVE_RESULT		? 1:0);}
		bool IsCellOccupied() const		{ return (this->_Action == ROBOT_ACTION::RESULT_OCCUPIED	? 1:0);}
		bool IsCellFree() const			{ return (this->_Action == ROBOT_ACTION::RESULT_FREE		? 1:0);}

		void SetMoveAction()	{ this->_Action = ROBOT_ACTION::ACTION_MOVE;		}
		void SetObserveAction()	{ this->_Action = ROBOT_ACTION::ACTION_OBSERVE;		}

		void SetOccupiedResult() { this->_Action = ROBOT_ACTION::RESULT_OCCUPIED;	}
		void SetFreeResult()	{ this->_Action = ROBOT_ACTION::RESULT_FREE;		}

		bool operator==(const RobotAction &i) const { return (this->_Action == i._Action); }

#ifdef DEBUG	// DEBUG
		std::ostream& PrintAction(std::ostream& os) const;
#endif			// ~DEBUG

	private:
		ROBOT_ACTION::ACTION_TYPE _Action;
};


#endif // ROBOT_ACTION_H
