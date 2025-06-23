#ifndef SECTOR_HH
#define SECTOR_HH

#pragma once

#include <string>

class Sector {
public:
	std::string data;
	Sector() {}
	Sector(int sectorSize, std::string in_data) {
		data.resize(sectorSize);
		data = in_data;
	}
};

#endif