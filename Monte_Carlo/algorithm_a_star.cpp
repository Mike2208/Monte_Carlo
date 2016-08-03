#include "algorithm_a_star.h"

namespace ALGORITHM_A_STAR
{

	POS_DIST::POS_DIST(const POS_2D &_Position, const DISTANCE_TYPE &_Distance) : POS_2D (_Position), Distance(_Distance)
	{}

	void POSTOCHECK::AddElement(const POS_DIST &Element)
	{
		for(POSTOCHECK::iterator curPos = this->begin(); curPos != this->end(); ++curPos)
		{
			if(Element.Distance < curPos->Distance)
			{
				this->insert(curPos, Element);			// Add element at correct position

				return;
			}
		}

		// If nothing was found, insert at end
		this->push_back(Element);
	}
}
