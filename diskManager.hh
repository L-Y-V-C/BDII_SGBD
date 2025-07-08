#pragma once

#include "disk.hh"
#include "diskIterator.hh"

class DiskManager
{
public:
    
    DiskManager(Disk& d) :
        disk(d), iterator(d)
    { }

    void next() { 
        iterator.next();
        disk.current_char_pos++;
    }

    char& current_char() { return iterator.current_char(); }

    void set_data(char in_char) { iterator.set_data(in_char); }

    std::vector<size_t> get_position() { return iterator.get_position(); }

    Disk& get_disk() { return disk; }

private:
    Disk& disk;
    DiskIterator iterator;
};