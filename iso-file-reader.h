#ifndef ISO_FILE_READER_H
#define ISO_FILE_READER_H

#include <string>

class IsoFileReader
{
public:
    IsoFileReader(const std::string& fileName);
    virtual ~IsoFileReader() {}

    int64_t GetFileSize() const { return _fileSize; }

private:
    std::string     _fileName;
    int64_t         _fileSize = -1;
};

#endif // ISO_FILE_READER_H
