#ifndef BASE64_TO_FILE_H
#define BASE64_TO_FILE_H

#include <fstream>
#include <array>

static constexpr unsigned char IllegalSymbol = 0xff;

int Base64ToFile(const char* data, const std::string& dstFileName)
{
    if (!data)
        return -1;

    std::ofstream dstStream;
    dstStream.open(dstFileName.c_str(), std::ios_base::trunc | std::ios::binary);
    if (dstStream.good() && dstStream.is_open())
    {
        static const std::string Base64Symbols("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
        unsigned char DecodeTable[256];
        memset(DecodeTable, IllegalSymbol, sizeof(DecodeTable));
        for (int i = 0; i < Base64Symbols.size(); ++i)
            DecodeTable[Base64Symbols[i]] = static_cast<unsigned char>(i);
        DecodeTable[0] = DecodeTable['='] = 0;

        constexpr int writeBufSize = 1024 * 1024;
        std::array<unsigned char, writeBufSize> writeBuf;
        int writeBufOffset = 0;
        int totalBytesWritten = 0;

        unsigned char quad[4];
        int quadIdx = 0;
        int padding = 0;
        bool endOfStream = false;
        for (int i = 0; !endOfStream; ++i)
        {
            // line breaks are not in the spec, but we'll allow them
            if (data[i] == '\r' || data[i] == '\n')
                continue;

            auto dec = DecodeTable[data[i]];
            if (dec != IllegalSymbol)
            {
                quad[quadIdx++] = dec;

                if (data[i] == '=')
                {
                    ++padding;
                    if (padding > 2)
                        return -1;
                }
                else if (data[i] == 0)
                {
                    endOfStream = true;

                    // pad manually if needed
                    if (quadIdx > 2)
                    {
                        ++padding;
                        while (quadIdx < 4)
                        {
                            quad[quadIdx++] = 0;
                            ++padding;
                        }
                    }
                    else if (quadIdx == 2)
                        return -1;  // not possible with base64 encoding
                }

                if (quadIdx == 4)
                {
                    writeBuf[writeBufOffset    ] = (quad[0] << 2) | (quad[1] >> 4);
                    writeBuf[writeBufOffset + 1] = (quad[1] << 4) | (quad[2] >> 2);
                    writeBuf[writeBufOffset + 2] = (quad[2] << 6) | quad[3];

                    writeBufOffset += 3 - padding;
                    quadIdx = 0;
                    padding = 0;

                    // no more space for the next iteration, flush the buffer
                    if ((writeBufOffset + 3) > writeBufSize)
                    {
                        dstStream.write(reinterpret_cast<const char*>(writeBuf.data()), writeBufOffset);
                        totalBytesWritten += writeBufOffset;
                        writeBufOffset = 0;
                    }
                }
            }
            else
                return -1;
        }

        // flush what is left in the buffer
        if (writeBufOffset > 0)
        {
            dstStream.write(reinterpret_cast<const char*>(writeBuf.data()), writeBufOffset);
            totalBytesWritten += writeBufOffset;
        }

        return totalBytesWritten;
    }

    return -1;
}

#if defined(ENABLE_TESTS)
int Base64DecodeUnitTests()
{
    // https://en.wikipedia.org/wiki/Base64#Decoding_Base64_with_padding
    const std::string s0("YW55IGNhcm5hbCBwbGVhc"); // illegal
    const std::string s1("YW55IGNhcm5hbCBwbGVhcw==");
    const std::string s2("YW55IGNhcm5hbCBwbGVhc3U=");
    const std::string s3("YW55IGNhcm5hbCBwbGVhc3Vy");
    const std::string s4("YW55IGNhcm5hbCBwbGVhc3VyZQ==");

    if (Base64ToFile(s0.c_str(), "s0.txt") != -1)
        return -1;

    if (Base64ToFile(s1.c_str(), "s1.txt") != 16)
        return -1;

    if (Base64ToFile(s2.c_str(), "s2.txt") != 17)
        return -1;

    if (Base64ToFile(s3.c_str(), "s3.txt") != 18)
        return -1;

    if (Base64ToFile(s4.c_str(), "s4.txt") != 19)
        return -1;

    // no padding
    const std::string s1np("YW55IGNhcm5hbCBwbGVhcw");
    const std::string s2np("YW55IGNhcm5hbCBwbGVhc3U");

    if (Base64ToFile(s1np.c_str(), "s1np.txt") != 16)
        return -1;

    if (Base64ToFile(s2np.c_str(), "s2np.txt") != 17)
        return -1;

    return 0;
}
#endif

#endif // BASE64_TO_FILE_H
