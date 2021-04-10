#ifndef IBMF_STREAM_READER_H
#define IBMF_STREAM_READER_H

namespace IBMF
{

class StreamReader
{
public:
    virtual ~StreamReader() {}

    virtual int64_t Read(char* buf, int64_t bufSize) = 0;

    virtual int64_t CurrentOffset() const = 0;
    virtual bool SeekTo(int64_t offset) = 0;

    virtual int64_t BytesAvailable() const = 0;
};

};  // namespace IBMF

#endif // IBMF_STREAM_READER_H
