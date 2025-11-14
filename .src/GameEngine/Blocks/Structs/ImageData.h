#pragma once

#include <string>
#include <filesystem>

struct ImageData
{ 
    std::string name; 
    std::filesystem::path path; 
    int w = 0, h = 0, channels = 0;
    unsigned char *data = nullptr;

    ~ImageData();
};