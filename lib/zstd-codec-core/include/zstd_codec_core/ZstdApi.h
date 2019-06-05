#pragma once

#include "types.h"

class ZstdApiResult {
public:
    static ZstdApiResult ok(usize code);
    static ZstdApiResult err(usize code);

    bool isOk() const;
    bool isErr() const;

    usize code() const;

private:
    ZstdApiResult(bool ok, usize code);

    bool ok_;
    usize code_;
};

class ZstdApi {
public:
    ZstdApiResult compress(Vec<u8>& dest, const Vec<u8>& src, int level) const;
    ZstdApiResult decompress(Vec<u8>& dest, const Vec<u8>& src) const;
};