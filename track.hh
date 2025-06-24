#ifndef TRACK_HH
#define TRACK_HH

#pragma once

#include <vector>
#include "sector.hh"

class Track {
public:
	std::vector<Sector> sectors;
	Track(int trackNumbers) {
		sectors.resize(trackNumbers);
	}
};

#endif