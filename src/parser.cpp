#include "ibmf/parser.h"

namespace IBMF
{

// other option is to use ntohl() which is fairly portable, but it gets
// messy when 64-bit integer support is needed
static inline uint32_t streamToHostU32(const char* data)
{
    const uint8_t* sbe = reinterpret_cast<const uint8_t*>(data);
    return (sbe[0] << 24) | (sbe[1] << 16) | (sbe[2] << 8) | sbe[3];
}

static inline uint64_t streamToHostU64(const char* data)
{
    return ((uint64_t)streamToHostU32(data) << 32) | streamToHostU32(data + 4);
}

static int ParseBox(StreamReader& stream, Box& box, std::string& errorMsg)
{
    errorMsg.clear();

    char readBuf[8];
    if (stream.BytesAvailable() > 0)
    {
        box.offset = stream.CurrentOffset();

        if (stream.Read(readBuf, 8) != 8)
        {
            errorMsg = "Failed to read box header";
            return -1;
        }

        box.type = std::string(readBuf + 4, 4);
        box.size = streamToHostU32(readBuf);
        if (box.size == 1)
        {
            if (stream.Read(readBuf, 8) != 8)
            {
                errorMsg = "Failed to read largesize field";
                return -1;
            }
            box.size = streamToHostU64(readBuf);
        }
        else if (box.size == 0)
            box.size = stream.BytesAvailable() + 8; // till the end of the file

        if (box.size < (stream.CurrentOffset() - box.offset))
        {
            errorMsg = "Invalid box size";
            return -1;
        }
    }
    else
    {
        if (stream.BytesAvailable() < 0)
            errorMsg = "Stream in wrong state";
        return -1;
    }

    return 0;
}

int ParseFile(StreamReader& stream, std::vector<Box>& boxes, std::string& errorMsg)
{
    int result = 0;
    while (stream.BytesAvailable() > 0)
    {
        boxes.emplace_back();
        auto& box = boxes.back();

        result = ParseBox(stream, box, errorMsg);
        if (result < 0)
        {
            boxes.pop_back();
            break;
        }

        stream.SeekTo(box.offset + box.size);
    }

    return result;
}

#if defined(ENABLE_TESTS)
int streamToHostUnitTests()
{
    char t[8] = { static_cast<char>(0xF1), static_cast<char>(0xF2), static_cast<char>(0xF3), static_cast<char>(0xF4),
        0x10, 0x20, 0x30, 0x40 };

    if (streamToHostU32(t) != 4059231220)
        return -1;

    if (streamToHostU32(t + 4) != 270544960)
        return -1;

    if (streamToHostU64(t) != 17434265337072726080ull)
        return -1;

    return 0;
}
#endif

};  // namespace IBMF
