#include "zstd.h"
#include "zstd_codec_core/ZstdApi.h"

////////////////////////////////////////////////////////////////////////////////
//
// ZstdApiResult
//
////////////////////////////////////////////////////////////////////////////////

ZstdApiResult ZstdApiResult::ok(usize code) {
    return {true, code};
}

ZstdApiResult ZstdApiResult::err(usize code) {
    return {false, code};
}

bool ZstdApiResult::isOk() const {
    return ok_;
}

bool ZstdApiResult::isErr() const {
    return !ok_;
}

usize ZstdApiResult::code() const {
    return code_;
}

ZstdApiResult::ZstdApiResult(bool ok, usize code)
        : ok_(ok), code_(code) {
}

////////////////////////////////////////////////////////////////////////////////
//
// ZstdApi
//
////////////////////////////////////////////////////////////////////////////////

ZstdApiResult ZstdApi::compress(Vec<u8> &dest, const Vec<u8> &src, int level) const {
    const auto max_size = ZSTD_compressBound(src.size());
    dest.resize(max_size);

    const auto rc = ZSTD_compress(dest.data(), dest.size(), src.data(), src.size(), level);
    if (ZSTD_isError(rc)) {
        return ZstdApiResult::err(rc);
    }

    dest.resize(rc);
    return ZstdApiResult::ok(rc);
}

ZstdApiResult ZstdApi::decompress(Vec<u8> &dest, const Vec<u8> &src) const {
    const auto content_size = ZSTD_getFrameContentSize(src.data(), src.size());
    if (content_size == ZSTD_CONTENTSIZE_UNKNOWN || content_size == ZSTD_CONTENTSIZE_ERROR) {
        return ZstdApiResult::err(content_size);
    }

    dest.resize(content_size);
    const auto rc = ZSTD_decompress(dest.data(), dest.size(), src.data(), src.size());
    if (ZSTD_isError(rc)) {
        return ZstdApiResult::err(rc);
    }

    return ZstdApiResult::ok(rc);
}
