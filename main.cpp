#include <iostream>
#include <clocale>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "third-party/pugixml/pugixml.hpp"

#include "ibmf/parser.h"

#include "file-stream-reader.h"
#include "base64-to-file.h"


static int imageCount = 0;

struct ImageWalker:
    pugi::xml_tree_walker
{
    virtual bool for_each(pugi::xml_node& node)
    {
        if (std::string(node.name()) == "smpte:image")
        {
            ++imageCount;
            std::string imageName(node.attribute("xml:id").value());
            if (imageName.empty())
            {
                std::ostringstream fixedWidthNumber;
                fixedWidthNumber << std::setw(3) << std::setfill('0') << imageCount;
                imageName = "image" + fixedWidthNumber.str();
            }
            std::string imageType(node.attribute("imagetype").value());
            if (imageType.empty())
                imageType = "png";
            else
                std::transform(imageType.begin(), imageType.end(), imageType.begin(), [](unsigned char c){ return std::tolower(c); });  // I just don't like PNG as extension

            const auto bytesWritten = Base64ToFile(node.text().get(), imageName + "." + imageType);
            if (bytesWritten < 0)
                std::cerr << "Couldn't write image to disk\n";
            else
                std::cout << imageName << "." << imageType << " written to disk (" << bytesWritten << " bytes)\n";
        }

        return true; // continue traversal
    }
};

void SaveMdatImages(IBMF::StreamReader& stream, int64_t mdatSize)
{
    if (stream.BytesAvailable() > 0 && stream.BytesAvailable() >= mdatSize)
    {
        std::unique_ptr<char[]> buf{ new(std::nothrow) char[mdatSize] };
        if (buf)
        {
            stream.Read(buf.get(), mdatSize);

            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_buffer_inplace(buf.get(), mdatSize);

            if (result)
            {
                ImageWalker walker;
                doc.traverse(walker);
            }
            else
                std::cerr << "Parsing failed, error: " << result.description() << "\n";
        }
        else
            std::cerr << "Can't allocate mdat parsing buffer\n";
    }
}

void ParseMdat(IBMF::StreamReader& stream, const IBMF::Box& box)
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

        constexpr int mdatPrintLimit = 2048;
        int64_t bytesPrinted = 0;
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
            bytesPrinted += toRead;
            bytesLeft -= toRead;

            if (bytesLeft > 100 && bytesPrinted >= mdatPrintLimit)
            {
                std::cout << "\n(" << bytesLeft << " more bytes)\n";
                break;
            }
        }

        std::cout << "=== mdat content end ===================\n";

        stream.SeekTo(box.offset + box.headerLen);
        SaveMdatImages(stream, box.size - box.headerLen);
    }
    else
    {
        for (const auto& child: box.children)
            ParseMdat(stream, child);
    }
}


int main(int argc, char* argv[])
{
    const std::string firstArg(argc > 1 ? argv[1] : "-h");
    if (firstArg == "-h" || firstArg == "--help")
    {
        std::cerr << "Usage: " << argv[0] << " input <input2> <input3> .. \n  input files are expected to be ISO/IEC base media (.mp4/.mov/.3gp)\n";
        return -1;
    }

#if defined(ENABLE_TESTS)
    if (Base64DecodeUnitTests() < 0)
    {
        std::cerr << "Base64 decoding tests failed\n";
        return -1;
    }

    if (IBMF::StreamToHostUnitTests() < 0)
    {
        std::cerr << "Stream-to-host tests failed\n";
        return -1;
    }

    std::cout << "Internal tests passed\n";
#endif

    for (int i = 1; i < argc; ++i)
    {
        FileStreamReader stream(argv[i]);
        if (stream.BytesAvailable() <= 0)
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

        std::setlocale(LC_CTYPE, ".UTF-8");
        for (const auto& box: boxes)
            ParseMdat(stream, box);
    }

    return 0;
}
