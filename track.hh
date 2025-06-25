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
		for (auto i : sectors)
		{
			std::cout << "SECTORS\n";
			i.print();
			std::cout << "sectors\n";
		}
	}
};

#endif