#ifndef DISK_HH
#define DISK_HH

#pragma once

#include "plate.hh"

class Disk {
public:
	std::vector<Plate> plates;
	Disk(int plateNumbers) {
		plates.resize(plateNumbers);
	}
};

#endif