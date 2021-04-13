#include <iostream>
#include <cstring>
#include <string>

#include "file-stream-reader.h"

FileStreamReader::FileStreamReader(const std::string& fileName):
    _file(fileName, std::ifstream::binary)
{
    std::string errorMsg;
    if (_file)
    {
        _file.seekg(0, std::ios::end);
        if (_file.tellg() <= 0)
            errorMsg = "File's empty / seek failure";
        else
        {
            _buf = std::unique_ptr<char[]>{ new(std::nothrow) char[_allocSize] };
            if (!_buf)
                errorMsg = "Buffer allocation failure";
        }
    }
    else
        errorMsg = "Can't open file";

    if (!errorMsg.empty())
        std::cerr << errorMsg << "\n";
}

int64_t FileStreamReader::Read(char* buf, int64_t bufSize)
{
    if (!buf || bufSize <= 0 || !_buf)
        return -1;

    int64_t totalBytesRead = 0;
    while (bufSize > 0)
    {
        if (_bytesInBuffer <= 0)
            FillReadBuffer();
        if (_bytesInBuffer <= 0)
            break;  // EOF

        const auto bytesRead = ReadFromBuffer(buf, bufSize);
        if (bytesRead <= 0)
            break;

        buf += bytesRead;
        bufSize -= bytesRead;
        totalBytesRead += bytesRead;
    }

    return totalBytesRead;
}

bool FileStreamReader::SeekTo(int64_t offset)
{
    if (offset == _readFileOffset)
        return true;

    if (offset >= 0)
    {
        // check if we're still inside the internal buffer,
        // if not we need to invalidate it
        if (offset >= _bufFileOffset && offset < (_bufFileOffset + _bytesInBuffer))
        {
            _readFileOffset = offset;
            return true;
        }

        InvalidateReadBuffer();
        if (_file)
        {
            _file.seekg(offset);
            _readFileOffset = _file.tellg();
            return (_readFileOffset == offset);
        }
    }

    return false;
}

int64_t FileStreamReader::BytesAvailable() const
{
    if (!_file)
        return -1;

    // trying to handle files being appended by other processes as we try to parse them
    _file.seekg(0, std::ios::end);
    const auto fileSize = _file.tellg();
    if (fileSize < 0 || fileSize < _readFileOffset)
        return -1;  // something went wrong, the file was probably overwritten by other process

    return fileSize - _readFileOffset;
}

void FileStreamReader::InvalidateReadBuffer()
{
    _bytesInBuffer = 0;
    _bufFileOffset = -1;
}

void FileStreamReader::FillReadBuffer()
{
    _file.seekg(_readFileOffset);
    if (_readFileOffset == _file.tellg())
    {
        _file.read(_buf.get(), _allocSize);
        _bytesInBuffer = _file.gcount();
        _bufFileOffset = _readFileOffset;
        _file.clear();  // reset EOF
    }
}

int FileStreamReader::ReadFromBuffer(char* buf, int64_t bufSize)
{
    auto bytesAvail = _bytesInBuffer - (_readFileOffset - _bufFileOffset);
    if (bytesAvail > bufSize)
        bytesAvail = bufSize;

    memcpy(buf, _buf.get() + (_readFileOffset - _bufFileOffset), bytesAvail);
    _readFileOffset += bytesAvail;

    if (_readFileOffset >= (_bufFileOffset + _bytesInBuffer))
        InvalidateReadBuffer();

    return static_cast<int>(bytesAvail);
}
