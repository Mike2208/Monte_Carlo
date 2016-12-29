#ifndef D_STAR_MAP_H
#define D_STAR_MAP_H

#include "standard_definitions.h"
#include "algorithm_d_star.h"
#include "map_2d.h"


template<class T>
class DStarMap
{
	public:
		typedef float PATH_LENGTH_TYPE;

		//DStarMap() = default;

		const Map2D<T> &MapData() const { return this->_DStarMap; }		// Allows for read only access to map
		const POS_2D &ZeroPos() const	{ return this->_ZeroPos; }			// Allows for read only access to zero position

		void CalculateDStarCostMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &MinVal, const T &MaxVal)
		{ this->_ZeroPos = ZeroPos; return AlgorithmDStar<T>::CalculateDStarCostMap(OriginalMap, ZeroPos, MinVal, MaxVal, this->_DStarMap); }

		static int CalculateMinCostPathLength(const DStarMap<T> &DStarMap, const POS_2D &StartPos, PATH_LENGTH_TYPE &PathLength, const T *const CutOffCellVal = nullptr);
		int CalculateMinCostPathLength(const POS_2D &StartPos, PATH_LENGTH_TYPE &PathLength, const T *const CutOffCellVal = nullptr) const
		{ return DStarMap<T>::CalculateMinCostPathLength((*this), StartPos, PathLength, CutOffCellVal); }

		void UpdateMap_SinglePos(const Map2D<T> &UpdatedOriginalMap, const POS_2D &UpdatePos)		// Update one position of D* Map with new value
		{ AlgorithmDStar<T>::UpdateMap_SingleValue(UpdatedOriginalMap, UpdatePos, this->_DStarMap); }

	private:

		POS_2D		_ZeroPos;
		Map2D<T>	_DStarMap;
};

#include "d_star_map_templates.cpp"

#endif // D_STAR_MAP_H
