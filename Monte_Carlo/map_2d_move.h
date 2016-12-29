#ifndef MAP_2D_MOVE_H
#define MAP_2D_MOVE_H

#include "standard_definitions.h"
#include "map_2d.h"

template<class T>
class Map2DMove : public virtual Map2D<T>
{
	public:
		typedef std::vector<T> CELL_STORAGE;
		typedef typename std::vector<T>::size_type CELL_STORAGE_INDEX;
		typedef T CELL_TYPE;

		Map2DMove(Map2D<T> &OriginalMap, const POS_2D &MoveMapToPos) : _pOriginalMap(&OriginalMap), _MovePosition(MoveMapToPos) {}

		Map2DMove(const Map2DMove<T> &S) : _pOriginalMap(S._pOriginalMap), _MovePosition(S._MovePosition) {}
		Map2DMove(Map2DMove<T> &&S) : _pOriginalMap(std::move(S._pOriginalMap)), _MovePosition(std::move(S._MovePosition)) {}
		Map2DMove<T> &operator=(const Map2DMove<T> &S) = default;
		Map2DMove<T> &operator=(Map2DMove<T> &&S) = default;


		void SetPixel(const POS_2D &Position, const T &Value) override { return this->_pOriginalMap->SetPixel(Position+this->_MovePosition, Value); }

		// Only enable if not bool type, as vector<bool> doesn't do references
		typename std::enable_if<!std::is_same<bool,T>::value, T&>::type GetPixelR(const POS_2D &Position) override { return this->_pOriginalMap->GetPixelR(Position+this->_MovePosition); }
		typename std::enable_if<!std::is_same<bool,T>::value, const T&>::type GetPixel(const POS_2D &Position) const override { return this->_pOriginalMap->GetPixel(Position+this->_MovePosition); }

		int GetPixel(const POS_2D &Position, T &Value) const override { return this->_pOriginalMap->GetPixel(Position+this->_MovePosition, Value); }

		void SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const T &Value) override { return this->_pOriginalMap->SetPathToValue(StartPos+this->_MovePosition, EndPos+this->_MovePosition, Value); }

		// Iterator access
		const CELL_STORAGE_INDEX GetElementPosInStorage(const POS_2D &Position) const override { return this->_pOriginalMap->GetElementPosInStorage(Position+this->_MovePosition); }
		//const T &GetPixelFromStorage(const CELL_STORAGE_INDEX &ElementPosition) const override { return static_cast<Map2D<T>>(*this).Map2D<T>::GetPixelFromStorage(ElementPosition); }

		bool IsInMap(const POS_2D &Position) const override { return this->_pOriginalMap->IsInMap(Position+this->_MovePosition); }

	private:

		Map2D<T>		*_pOriginalMap;			// Original map to use
		POS_2D	_MovePosition;			// Position map is moved to
};


#endif // MAP_2D_MOVE_H
