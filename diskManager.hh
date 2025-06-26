#pragma once

#include "disk.hh"

class DiskManager
{
public:
    Disk& disk;

    DiskManager(Disk& d) :
        disk(d), plateIndex(0), surfaceIndex(0), trackIndex(0), sectorIndex(0), char_index(0)
    {
        advanceToNextValid();
    }

    void next()
    {
        if (is_disk_full())
            return;

        char_index++;
        disk.current_char_pos++;
        advanceToNextValid();
    }

    char& current_char()
    {
        return disk.plates[plateIndex].surfaces[surfaceIndex]
            .tracks[trackIndex].sectors[sectorIndex].data[char_index];
    }

    void set_data(char in_char)
    {
        if (!is_disk_full())
        {
            char &a = current_char();
            a = in_char;
        }
    }

    std::vector<size_t> get_position()
    {
        return std::vector<size_t>({ plateIndex, surfaceIndex, trackIndex, sectorIndex, char_index });
    }

private:

    size_t plateIndex,
           surfaceIndex,
           trackIndex,
           sectorIndex,
           char_index;

    void advanceToNextValid()
    {
        while (plateIndex < disk.plates.size())
        {
            Plate& plate = disk.plates[plateIndex];
            if (surfaceIndex >= plate.surfaces.size())
            {
                plateIndex++;
                surfaceIndex = 0;
                trackIndex = 0;
                sectorIndex = 0;
                continue;
            }
            auto& surface = plate.surfaces[surfaceIndex];
            if (trackIndex >= surface.tracks.size())
            {
                surfaceIndex++;
                trackIndex = 0;
                sectorIndex = 0;
                continue;
            }
            auto& track = surface.tracks[trackIndex];
            if (sectorIndex >= track.sectors.size())
            {
                trackIndex++;
                sectorIndex = 0;
                continue;
            }

            auto& sector = track.sectors[sectorIndex];
            if (char_index >= sector.data.size())
            {
                char_index = 0;
                sectorIndex++;
                continue;
            }
            return;
        }
    }

    bool is_disk_full() const
    {
        return plateIndex >= disk.plates.size();
    }
};