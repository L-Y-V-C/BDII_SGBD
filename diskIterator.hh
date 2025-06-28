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
        for (int i = 0; i < inVector.size(); ++i) {
            positions[i].resize(inVector[i].size());
            for (int j = 0; j < inVector[i].size(); ++j) {
                positions[i][j] = std::stoi(inVector[i][j]);
            }
        }
        return positions;
    }

    std::vector<std::string> iterateAndExtractRegs(std::vector<std::vector<std::string>> inVector)
    {
        std::vector<std::string> result(inVector.size());
        std::vector<std::vector<int>> positions;
        positions = receiveIndexesFromMetadata(inVector);

        int half = (positions[0].size() - 1) / 2;
        std::vector<int> startIndexes(half, 0);
        std::vector<int> endIndexes(half, 0);

        for (int i = 0; i < positions.size(); i++) {
            std::vector<int> startIndexes(positions[i].begin() + 1, positions[i].begin() + 1 + half);
            std::vector<int> endIndexes(positions[i].begin() + 1 + half, positions[i].end());
            result[i] = extractReg(startIndexes, endIndexes);


            result[i] = result[i].substr(0, result[i].length() - 1); //To delete register separator ('/')

        }
        return result;
    }

    std::string extractReg(std::vector<int> start, std::vector<int> end)
    {
        std::string result = "";
        setIndexes(start);
        char cc;
        for (int i = 0; i < regSize; i++) {
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

private:
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