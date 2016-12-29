#ifndef QUAD_MAP_H
#define QUAD_MAP_H

#include "standard_definitions.h"
#include "map_2d.h"

namespace QUAD_MAP
{
	typedef size_t ID;

	struct ID_RECTANGLE
	{
		POS_2D BottomLeftPos;				// Bottom Left Position
		POS_2D_TYPE Width;					// Width of quad
		POS_2D_TYPE Height;					// Height of quad

		bool IsPosInRectangle(const POS_2D &Pos) const;
	};

	struct ID_DATA : public ID_RECTANGLE
	{
		std::vector<ID> AdjacentIDs;		// Adjacent Quads

		bool EraseID(const ID IDToErase);
	};

	// Constants
	const ID INVALID_ID = GetInfiniteVal<ID>();

	const size_t BOTTOM_LEFT_QUAD = 0;
	const size_t TOP_LEFT_QUAD = 1;
	const size_t TOP_RIGHT_QUAD = 2;
	const size_t BOTTOM_RIGHT_QUAD = 3;
}

class QuadMap
{
	public:
		typedef QUAD_MAP::ID ID;
		typedef QUAD_MAP::ID_RECTANGLE ID_RECTANGLE;
		typedef QUAD_MAP::ID_DATA ID_DATA;

		QuadMap(const POS_2D_TYPE MapWidth, const POS_2D_TYPE MapHeight);

		void ResetToOneQuad();				// Reset map to one quad

		bool DivideQuad(const ID QuadID);			// Divide a quad into four smaller ones if size is big enough

		ID GetIDAtPos(const POS_2D &Position) const;		// Get ID of quad at position
		const Map2D<ID> &GetIDMap() const;					// Access to IDMap

		const ID_DATA &GetIDData(const ID QuadID) const;	// Get IDData of quad
		const std::vector<ID> &GetAdjacentIDs(const ID QuadID) const;		// Get adjacent ID of quad

	private:

		Map2D<ID> _Map;			// Map containing IDs of quads

		std::vector<ID_DATA>	_IDData;			// Stores ID sizes and adjacencies
};

#endif // QUAD_MAP_H
