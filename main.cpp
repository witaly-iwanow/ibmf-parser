#include <iostream>

#include "ibmf/parser.h"

#include "file-stream-reader.h"

void PrintMdat(IBMF::StreamReader& stream, const IBMF::Box& box)
{
    if (box.type == "mdat")
    {
        constexpr int coutBufSize = 1024;
        char coutBuf[coutBufSize];

        if (!stream.SeekTo(box.offset + box.headerLen))
        {
            std::cerr << "mdat seek error\n";
            return;
        }

        std::cout << "\n=== mdat content begin =================\n";

        int64_t bytesLeft = box.size - box.headerLen;
        while (bytesLeft > 0)
        {
            const auto toRead = (bytesLeft >= coutBufSize) ? coutBufSize : bytesLeft;
            if (stream.Read(coutBuf, toRead) < toRead)
            {
                std::cerr << "mdat read error\n";
                return;
            }

            std::cout << std::string(coutBuf, toRead);
            bytesLeft -= toRead;
        }
        std::cout << "=== mdat content end ===================\n";
    }
    else
    {
        for (const auto& child: box.children)
            PrintMdat(stream, child);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " input\n  input is expected to be ISO/IEC base media file (.mp4/.mov/.3gp)\n";
        return -1;
    }

    FileStreamReader stream(argv[1]);
    if (stream.BytesAvailable() < 0)
        return -1;

    std::cout << "Successfully opened " << argv[1] << ", " << stream.BytesAvailable() << " bytes" << std::endl;

    std::vector<IBMF::Box> boxes;
    std::string errorMsg;
    if (IBMF::ParseFile(stream, boxes, errorMsg) < 0)
    {
        std::cerr << "Parsing failed, error message: " << errorMsg << "\n";
        return -1;
    }

    std::cout << "File structure:\n";
    for (const auto& box: boxes)
        std::cout << box.ToString("", "..") << std::endl;

    for (const auto& box: boxes)
        PrintMdat(stream, box);

    return 0;
}
