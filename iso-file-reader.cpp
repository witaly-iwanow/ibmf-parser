#include <iostream>
#include <fstream>

#include "iso-file-reader.h"

IsoFileReader::IsoFileReader(const std::string& fileName):
    _fileName(fileName)
{
    std::ifstream file(fileName, std::ifstream::binary);
    if (file.good())
    {
        file.seekg(0, std::ios::end);
        _fileSize = file.tellg();
    }
    else
        std::cerr << "Fatal error, can't open " << fileName << "\n";
}
