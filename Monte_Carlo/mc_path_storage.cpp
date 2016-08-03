#include "mc_path_storage.h"

MCPathStorage::MCPathStorage(const PATH_ID &MaxStoredPaths) : _MaxStoredPaths(MaxStoredPaths)
{
}

const MCPathStorage::PATH_DATA &MCPathStorage::GetPath(const PATH_ID &PathID)const
{
	return this->at(PathID);
}

int MCPathStorage::SetPath(const PATH_ID &PathID, const PATH_DATA &PathData)
{
	// Check if path exists
	if(this->size()-1 >= PathID)
	{
		if(this->size()-1 == PathID)
			this->push_back(PathData);

		return -1;
	}

	if(this->at(PathID).IsFree == true)
		this->_NumFreePathsInArray--;

	this->at(PathID) = PathData;
	this->at(PathID).IsFree = false;

	return 1;
}

int MCPathStorage::SetPath(const PATH_ID &PathID, PATH_DATA &&PathData)
{
	// Check if path exists
	if(this->size()-1 >= PathID)
	{
		if(this->size()-1 == PathID)
			this->push_back(std::move(PathData));

		return -1;
	}

	if(this->at(PathID).IsFree == true)
		this->_NumFreePathsInArray--;

	this->at(PathID) = std::move(PathData);
	this->at(PathID).IsFree = false;

	return 1;
}

MCPathStorage::PATH_ID MCPathStorage::AddPath(const PATH_DATA &PathData)
{
	// If size is not yet fully used, fill it before looking for free IDs
	if(this->size() < this->_MaxStoredPaths)
	{
		this->push_back(PathData);

		return this->size()-1;
	}
	else
	{
		for(PATH_STORAGE::iterator curPathData = this->begin();  curPathData != this->end(); ++curPathData)
		{
			if(curPathData->IsFree)
			{
				this->_NumFreePathsInArray--;

				*curPathData = PathData;
				curPathData->IsFree = false;

				return static_cast<PATH_ID>(curPathData - this->begin());
			}
		}
	}

	// If no more space is free, return ERROR
	return this->_MaxStoredPaths;
}

void MCPathStorage::ClearPath(const PATH_ID &PathID)
{
	// Check that path exists before clearing it
	if(this->size() <= PathID)
		return;

	// Check if path was already cleared
	if(this->at(PathID).IsFree)
		return;

	// If this is end, erase last entry and continue erasing old entries
	if(PathID == this->size()-1)
	{
		PATH_ID curID = PathID;
		this->erase(this->end()-1);
		curID--;

		while(curID != static_cast<PATH_ID>(-1) && this->at(curID).IsFree)
		{
			this->erase(this->end()-1);
			curID--;

			this->_NumFreePathsInArray--;
		}
	}
	else
	{
		this->at(PathID).clear();
		this->at(PathID).IsFree = true;

		this->_NumFreePathsInArray++;
	}

	return;
}

MCPathStorage::PATH_DATA &MCPathStorage::GetTempPath()
{
	return this->_TempPath;
}

const MCPathStorage::PATH_DATA &MCPathStorage::GetTempPath()const
{
	return this->_TempPath;
}
void MCPathStorage::SetTempPath(const PATH_DATA &PathData)
{
	this->_TempPath = PathData;
}

bool MCPathStorage::IsPathStored(const PATH_ID &PathID) const
{
	if(this->size() <= PathID)
		return false;

	return !this->at(PathID).IsFree;
}

bool MCPathStorage::IsFreeSpaceAvailable(PATH_ID *FreePathID) const
{
	// If size is not yet fully used, fill it before looking for free IDs
	if(this->size() < this->_MaxStoredPaths)
	{
		if(FreePathID != nullptr)
			*FreePathID = this->size();

		return true;
	}
	else
	{
		for(PATH_STORAGE::const_iterator curPathData = this->begin();  curPathData != this->end(); ++curPathData)
		{
			if(curPathData->IsFree)
			{
				if(FreePathID != nullptr)
					*FreePathID = static_cast<PATH_ID>(curPathData - this->begin());
				return true;
			}
		}
	}

	// If no more space is free, return ERROR
	if(FreePathID != nullptr)
		*FreePathID = this->_MaxStoredPaths;
	return false;
}

const MCPathStorage::PATH_ID &MCPathStorage::GetMaxStoredPaths() const
{
	return this->_MaxStoredPaths;
}
