#include <iostream>

#include "iso-file-reader.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " input\n  input is expected to be ISO/IEC base media file (.mp4/.mov/.3gp)\n";
        return -1;
    }

    IsoFileReader ifr(argv[1]);
    if (ifr.GetFileSize() > 0)
        std::cout << "Successfully opened " << argv[1] << ", " << ifr.GetFileSize() << " bytes" << std::endl;

    return 0;
}
