#ifndef PLATE_HH
#define PLATE_HH

#pragma once

#include "surface.hh"

class Plate {
public:
	std::vector<Surface> surfaces;
	Plate() {
		surfaces.resize(2);
	}
};

#endif