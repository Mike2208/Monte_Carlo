#ifndef DISTRICT_TREE_H
#define DISTRICT_TREE_H

/*	class DistrictTree
 *		contains data on how districts where created
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"
#include "tree_class_permanent.h"

namespace DISTRICT_TREE
{
	struct NODE_DATA
	{
		DISTRICT_MAP_DEFINITIONS::ID	ID;		// ID of district

		NODE_DATA(const DISTRICT_MAP_DEFINITIONS::ID &_ID) : ID(_ID) {}
	};

	typedef TreeClassPermanent<NODE_DATA>	TREE_CLASS;
	typedef TREE_CLASS::TREE_NODE	TREE_NODE;

	const TREE_NODE::ID INVALID_ID = GetInfiniteVal<TREE_NODE::ID>();
}

class DistrictTree : public DISTRICT_TREE::TREE_CLASS
{
	public:

	private:
};

#endif // DISTRICT_TREE_H
