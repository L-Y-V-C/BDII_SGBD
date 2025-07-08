#pragma once

#include "disk.hh"

class DiskIterator
{
public:

    DiskIterator(Disk& d, int in_reg_size = 0) :
        disk(d), plateIndex(0), surfaceIndex(0), trackIndex(0), sectorIndex(0), char_index(0), regSize(in_reg_size)
    { }

    void next()
    {
        if (is_disk_full())
            return;
        char_index++;
        advanceToNextValid();
    }

    std::vector<size_t> get_position()
    {
        return std::vector<size_t>({ plateIndex, surfaceIndex, trackIndex, sectorIndex, char_index });
    }

    void set_data(char in_char)
    {
        if (!is_disk_full())
        {
            char& a = current_char();
            a = in_char;
        }
    }

    // to str to int
    std::vector<std::vector<int>> receiveIndexesFromMetadata(std::vector<std::vector<std::string>> inVector)
    {
        std::vector<std::vector<int>> positions(inVector.size());
        for (int i = 0; i < inVector.size(); ++i)
        {
            positions[i].resize(inVector[i].size());
            for (int j = 0; j < inVector[i].size(); ++j)
                positions[i][j] = std::stoi(inVector[i][j]);
        }
        return positions;
    }

    std::vector<std::vector<std::string>> iterateAndExtractRegs(std::vector<std::vector<std::string>> inVector)
    {
        std::vector<std::vector<std::string>> result(inVector.size());

        std::vector<std::vector<int>> positions;
        positions = receiveIndexesFromMetadata(inVector);

        int fields_limit = (positions[0].size() - 1) / 10;
        for (int i = 0; i < inVector.size(); i++)
        {
            for (int j = 0; j < fields_limit; j++)
            {
                int start = (10 * j) + 1;

                std::vector<int> startIndexes({positions[i][start], positions[i][start + 1], positions[i][start + 2], positions[i][start + 3], positions[i][start + 4] });
                std::vector<int> endIndexes({ positions[i][start + 5], positions[i][start + 5 + 1], positions[i][start + 5 + 2], positions[i][start + 5 + 3], positions[i][start + 5 + 4] });
                result[i].push_back(extractReg(startIndexes, endIndexes));
            }
        }

        
        return result;
    }

    std::string extractReg(std::vector<int> start, std::vector<int> end)
    {
        std::string result = "";
        setIndexes(start);
        
        char cc{'\0'};

        while (true)
        {
            // Check if finished
            bool equal{ true };

            std::vector<size_t> cur_pos = get_position();


            for (int j = 0; j < cur_pos.size(); j++)
            {
                if (cur_pos[j] != end[j]) // Llegó al final
                {
                    equal = false;
                    break;
                }
            }

            if (equal)
                break;

            cc = current_char();
            result.push_back(cc);
            next();

        }
        return result;
    }
    
    char& current_char()
    {
        return disk.plates[plateIndex].surfaces[surfaceIndex]
            .tracks[trackIndex].sectors[sectorIndex].data[char_index];
    }

public:
    Disk& disk;
    std::vector<int> fieldSizes;
    int regSize;

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

    void setIndexes(std::vector<int> Indexes)
    {
        plateIndex = Indexes[0];
        surfaceIndex = Indexes[1];
        trackIndex = Indexes[2];
        sectorIndex = Indexes[3];
        char_index = Indexes[4];
    }

    void resetIndexes()
    {
        plateIndex = surfaceIndex = trackIndex = sectorIndex = char_index = 0;
    }

    bool is_disk_full() const
    {
        return plateIndex >= disk.plates.size();
    }

    

};