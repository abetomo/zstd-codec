#pragma once

#include <memory>
#include "types.h"

extern "C" {
struct ZSTD_CDict_s;    // orginal struct of ZSTD_CDict
struct ZSTD_DDict_s;    // orginal struct of ZSTD_DDict
}

class ZstdCompressionDict {
public:
    ZstdCompressionDict(const Vec<u8> &dict_bytes, int level);

    ~ZstdCompressionDict();

    bool fail() const;

    ZSTD_CDict_s *get() const;

private:
    class Impl;

    std::unique_ptr<Impl> pimpl_;
};


class ZstdDecompressionDict {
public:
    ZstdDecompressionDict(const Vec<u8> &dict_bytes);

    ~ZstdDecompressionDict();

    bool fail() const;

    ZSTD_DDict_s *get() const;

private:
    class Impl;

    std::unique_ptr<Impl> pimpl_;
};

