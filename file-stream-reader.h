#ifndef FILE_STREAM_READER_H
#define FILE_STREAM_READER_H

#include <fstream>
#include <memory>

#include "ibmf/stream-reader.h"

// not thread-safe as it's not currently needed
class FileStreamReader:
    public IBMF::StreamReader
{
public:
    FileStreamReader(const std::string& fileName);

    int64_t Read(char* buf, int64_t bufSize) override;

    // offsets are always assumed to be calculated from the beginning of the file
    int64_t CurrentOffset() const override { return _readFileOffset; }
    bool SeekTo(int64_t offset) override;

    int64_t BytesAvailable() const override;

private:
    void InvalidateReadBuffer();
    void FillReadBuffer();
    int ReadFromBuffer(char* buf, int64_t bufSize);

    mutable std::ifstream _file;

    // the best test scenario is to set it to a very low value
    // (it does work correctly with _allocSize = 1)
    static constexpr int _allocSize = 64 * 1024;
    std::unique_ptr<char[]> _buf;
    int64_t _bytesInBuffer = 0;
    int64_t _bufFileOffset = -1;
    int64_t _readFileOffset = 0;
};

#endif // FILE_STREAM_READER_H
