#ifndef DISK_HH
#define DISK_HH

#pragma once

#include "plate.hh"

class Disk
{
public:
	std::vector<Plate> plates;
	size_t current_char_pos;

	Disk(int in_plate_number, int in_track_number, int in_sector_number, int in_sector_size):
		plate_number(in_plate_number),
		track_number(in_track_number),
		sector_number(in_sector_number),
		sector_size(in_sector_size),
		current_char_pos(0)
	{
		total_space = plate_number * 2 * track_number * sector_number * sector_size;
		plates.resize(plate_number, Plate(track_number, sector_number, sector_size));
	}

	int get_remnant_space() { return total_space - current_char_pos; }

	void print_disk()
	{
		for (auto i : plates)
		{
			std::cout << "PLATE\n";
			i.print();
			std::cout << "PLATE\n";
		}
			


	}

private:
	int plate_number, track_number, sector_number, sector_size;
	size_t total_space;
	
};

#endif