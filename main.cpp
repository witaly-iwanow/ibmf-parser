#include <iostream>

#include "ibmf/parser.h"

#include "file-stream-reader.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " input\n  input is expected to be ISO/IEC base media file (.mp4/.mov/.3gp)\n";
        return -1;
    }

    FileStreamReader streamer(argv[1]);
    if (streamer.BytesAvailable() < 0)
        return -1;

    std::cout << "Successfully opened " << argv[1] << ", " << streamer.BytesAvailable() << " bytes" << std::endl;

    std::vector<IBMF::Box> boxes;
    std::string errorMsg;
    if (IBMF::ParseFile(streamer, boxes, errorMsg) < 0)
    {
        std::cerr << "Parsing failed, error message: " << errorMsg << "\n";
        return -1;
    }

    std::cout << "File structure:\n";
    for (const auto& box: boxes)
        std::cout << box.ToString("", "..") << std::endl;

    return 0;
}
