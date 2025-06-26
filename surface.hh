#ifndef SURFACE_HH
#define SURFACE_HH

#pragma once

#include "track.hh"

class Surface
{
public:
	std::vector<Track> tracks;
	Surface(int in_track_number, int in_sector_number, int in_sector_size)
	{
		tracks.resize(in_track_number, Track(in_sector_number, in_sector_size));
	}

	void print()
	{
		for (int i = 0; i < tracks.size(); i++)
		{
			tracks[i].print();
		}
	}
	
};

#endif