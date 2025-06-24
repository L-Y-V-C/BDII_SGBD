#pragma once

#include "disk.hh"

class SectorIterator {
public:
    Disk& disk;
    size_t plateIndex = 0;
    size_t surfaceIndex = 0;
    size_t trackIndex = 0;
    size_t sectorIndex = 0;

    SectorIterator(Disk& d) : disk(d) {
        advanceToNextValid();
    }

    bool done() const {
        return plateIndex >= disk.plates.size();
    }

    void next() {
        if (done())
            return;
        sectorIndex++;
        advanceToNextValid();
    }

    Sector& currentSector() {
        return disk.plates[plateIndex].surfaces[surfaceIndex]
            .tracks[trackIndex].sectors[sectorIndex];
    }

    void advanceToNextValid() {
        while (plateIndex < disk.plates.size()) {
            Plate& plate = disk.plates[plateIndex];
            if (surfaceIndex >= plate.surfaces.size()) {
                plateIndex++;
                surfaceIndex = 0;
                trackIndex = 0;
                sectorIndex = 0;
                continue;
            }
            auto& surface = plate.surfaces[surfaceIndex];
            if (trackIndex >= surface.tracks.size()) {
                surfaceIndex++;
                trackIndex = 0;
                sectorIndex = 0;
                continue;
            }
            auto& track = surface.tracks[trackIndex];
            if (sectorIndex >= track.sectors.size()) {
                trackIndex++;
                sectorIndex = 0;
                continue;
            }
            return;
        }
    }
};