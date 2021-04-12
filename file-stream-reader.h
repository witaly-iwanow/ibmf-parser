#ifndef FILE_STREAM_READER_H
#define FILE_STREAM_READER_H

#include <fstream>
#include <memory>

#include "ibmf/stream-reader.h"

class FileStreamReader:
    public IBMF::StreamReader
{
public:
    FileStreamReader(const std::string& fileName)
    {
        std::string errorMsg;

        std::ifstream file(fileName, std::ifstream::binary);
        if (file.good())
        {
            file.seekg(0, std::ios::end);
            _bufSize = file.tellg();
            if (_bufSize > 0)
            {
                _buf = std::unique_ptr<char[]>{ new(std::nothrow) char[_bufSize] };
                if (_buf)
                {
                    file.seekg(0);
                    if (!file.read(_buf.get(), _bufSize))
                        errorMsg = "File read failed";
                }
                else
                    errorMsg = "Not enough memory";
            }
            else
                errorMsg = "File's empty / seek failure";
        }
        else
            errorMsg = "Can't open file";

        if (!errorMsg.empty())
        {
            std::cerr << errorMsg << "\n";
            _buf = nullptr;
            _bufSize = -1;
        }
    }

    int64_t Read(char* buf, int64_t bufSize) override
    {
        if (_readOffset < _bufSize)
        {
            const auto bytesToRead = std::min(bufSize, _bufSize - _readOffset);
            if (bytesToRead > 0 && buf)
            {
                std::memcpy(buf, _buf.get() + _readOffset, bytesToRead);
                _readOffset += bytesToRead;

                return bytesToRead;
            }
        }

        return 0;
    }

    int64_t CurrentOffset() const override { return _readOffset; }
    bool SeekTo(int64_t offset) override
    {
        if (offset >= 0 && offset <= _bufSize)
        {
            _readOffset = offset;
            return true;
        }
        return false;
    }

    int64_t BytesAvailable() const override { return _bufSize - _readOffset; }

private:
    std::unique_ptr<char[]> _buf;
    int64_t _bufSize = -1;
    int64_t _readOffset = 0;
};

#endif // FILE_STREAM_READER_H
