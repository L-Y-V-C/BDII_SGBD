#ifndef SECTOR_HH
#define SECTOR_HH

#pragma once

#include <string>
#include <iostream>

class Sector {
public:
	std::string data;
	Sector() {}
	Sector(int sectorSize)
	{
		data.resize(sectorSize, '\0');
	}

	void reset() { data.clear(); }

	void print()
	{
		std::cout << data << "\n";
	}
};

#endif