#include <memory>

#include "zstd.h"
#include "zstd_codec_core/ZstdDict.h"

////////////////////////////////////////////////////////////////////////////////
//
// ZstdCompressionDict
//
////////////////////////////////////////////////////////////////////////////////

class ZstdCompressionDict::Impl {
public:
    Impl(const Vec<u8> &dict_bytes, int level) : dict_(
            ZSTD_createCDict(&dict_bytes[0], dict_bytes.size(), level), ZSTD_freeCDict) {
    }

    ZSTD_CDict_s *get() const {
        return dict_.get();
    }

private:
    using CDictPtr = std::unique_ptr<ZSTD_CDict_s, decltype(&ZSTD_freeCDict)>;
    CDictPtr dict_;
};

ZstdCompressionDict::ZstdCompressionDict(const Vec<u8> &dict_bytes, int level)
        : pimpl_(new Impl(dict_bytes, level)) {
}

ZstdCompressionDict::~ZstdCompressionDict() = default;

bool ZstdCompressionDict::fail() const {
    return pimpl_->get() == nullptr;
}

ZSTD_CDict_s *ZstdCompressionDict::get() const {
    return pimpl_->get();
}

////////////////////////////////////////////////////////////////////////////////
//
// ZstdDecompressionDict
//
////////////////////////////////////////////////////////////////////////////////

class ZstdDecompressionDict::Impl {
public:
    Impl(const Vec<u8> &dict_bytes) : dict_(
            ZSTD_createDDict(&dict_bytes[0], dict_bytes.size()), ZSTD_freeDDict) {
    }

    ZSTD_DDict_s *get() const {
        return dict_.get();
    }

private:
    using DDictPtr = std::unique_ptr<ZSTD_DDict_s, decltype(&ZSTD_freeDDict)>;
    DDictPtr dict_;
};

ZstdDecompressionDict::ZstdDecompressionDict(const Vec<u8> &dict_bytes)
        : pimpl_(new Impl(dict_bytes)) {
}

ZstdDecompressionDict::~ZstdDecompressionDict() = default;

bool ZstdDecompressionDict::fail() const {
    return pimpl_->get() == nullptr;
}

ZSTD_DDict_s *ZstdDecompressionDict::get() const {
    return pimpl_->get();
}
