#ifndef PLATE_HH
#define PLATE_HH

#pragma once

#include "surface.hh"

class Plate
{
public:
	std::vector<Surface> surfaces;
	Plate(int in_track_number, int in_sector_number, int in_sector_size)
	{
		surfaces.resize(2, Surface(in_track_number, in_sector_number, in_sector_size));
	}

	void print()
	{
		for (int i = 0; i < surfaces.size(); i++)
		{
			surfaces[i].print();
		}	
	}
};

#endif