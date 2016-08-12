#ifndef MC_PATH_STORAGE_H
#define MC_PATH_STORAGE_H

#include "standard_definitions.h"
#include "pos_2d.h"

#include<vector>
#include<queue>

namespace MC_PATH_STORAGE
{
	struct PATH_DATA: public std::vector<POS_2D>	// One path
	{
		typedef PATH_DATA::size_type ID;
	};
	struct PATH_DATA_FREE : public PATH_DATA		// Storage of both path data and free flag
	{
		bool IsFree = false;

		PATH_DATA_FREE() = default;
		PATH_DATA_FREE(const PATH_DATA &PathData) : PATH_DATA(PathData) {}
	};

	typedef std::vector<PATH_DATA_FREE>	PATH_STORAGE;	// Storage of all paths
	typedef PATH_STORAGE::size_type	PATH_ID;		// ID of paths
	typedef std::vector<PATH_ID> FREE_ID_STORAGE;	// Storage of all free IDs
}

class MCPathStorage : private MC_PATH_STORAGE::PATH_STORAGE
{
	public:
		typedef MC_PATH_STORAGE::PATH_DATA PATH_DATA;
		typedef MC_PATH_STORAGE::PATH_STORAGE PATH_STORAGE;
		typedef MC_PATH_STORAGE::PATH_ID PATH_ID;

		MCPathStorage(const PATH_ID &MaxStoredPaths);

		const PATH_DATA &GetPath(const PATH_ID &PathID) const;
		PATH_DATA &GetPathR(const PATH_ID &PathID);
		int SetPath(const PATH_ID &PathID, const PATH_DATA &PathData);
		int SetPath(const PATH_ID &PathID, PATH_DATA &&PathData);

		PATH_ID AddPath(const PATH_DATA &PathData);
		PATH_ID AddPath(PATH_DATA &&PathData);
		void ClearPath(const PATH_ID &PathID);

		PATH_DATA &ReservePathWithTemp(PATH_ID *ReservedID = nullptr);		// Attempt to add a path, and use temp place if none is available

		bool IsTempStorage(const PATH_DATA *const PathData) const;			// Returns whether the given path is temp storage

		PATH_DATA &GetTempPath();
		const PATH_DATA &GetTempPath()const;
		void SetTempPath(const PATH_DATA &PathData);
		void SetTempPath(PATH_DATA &&PathData);

		bool IsPathStored(const PATH_ID &PathID) const;

		bool IsFreeSpaceAvailable(PATH_ID *FreePathID) const;

		const PATH_ID &GetMaxStoredPaths() const;

	private:

		const PATH_ID _MaxStoredPaths;				// Maximum amount of stored paths
		PATH_ID _NumFreePathsInArray = 0;			// Number of paths with IsFree set to true

		PATH_DATA _TempPath;		// Temporary path storage
};

#endif // MC_PATH_STORAGE_H
