#pragma once

#include <functional>
#include "types.h"

using StreamCallback = std::function<void(const Vec<u8>&)>;

class ZstdCompressionDict;
class ZstdDecompressionDict;

class ZstdCompressStream
{
public:
    ZstdCompressStream();
    ~ZstdCompressStream();

    bool begin(int level);
    bool begin(const ZstdCompressionDict& cdict);
    bool transform(const Vec<u8>& chunk, const StreamCallback& callback);
    bool flush(const StreamCallback& callback);
    bool end(const StreamCallback& callback);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

class ZstdDecompressStream
{
public:
    ZstdDecompressStream();
    ~ZstdDecompressStream();

    bool begin();
    bool begin(const ZstdDecompressionDict& ddict);
    bool transform(const Vec<u8>& chunk, const StreamCallback& callback);
    bool flush(const StreamCallback& callback);
    bool end(const StreamCallback& callback);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};
