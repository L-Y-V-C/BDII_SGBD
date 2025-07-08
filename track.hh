#ifndef TRACK_HH
#define TRACK_HH

#pragma once

#include <vector>
#include "sector.hh"

class Track {
public:
	std::vector<Sector> sectors;
	Track(int in_sector_number, int in_sector_size)
	{
		sectors.resize(in_sector_number, Sector(in_sector_size));
	}

	void print()
	{
		for (int i = 0; i < sectors.size(); i++)
		{
			sectors[i].print();
		}
	}

	void clear()
	{
		for (int i = 0; i < sectors.size(); i++)
			sectors[i].clear();
		
		sectors.clear();

	}

};

#endif