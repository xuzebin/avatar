#ifndef BUFFERDATA_H
#define BUFFERDATA_H

namespace avt {
    class BufferData {
    public:
        virtual ~BufferData();
        virtual void decodeBuffer(const char* buffer) = 0;
    protected:
        BufferData();
    };
}

#endif /* BUFFERDATA_H */
