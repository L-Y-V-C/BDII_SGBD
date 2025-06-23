#ifndef SURFACE_HH
#define SURFACE_HH

#pragma once

#include "track.hh"

class Surface {
public:
	std::vector<Track> tracks;
	Surface(int surfaceNumbers) {
		tracks.resize(surfaceNumbers);
	}
};

#endif