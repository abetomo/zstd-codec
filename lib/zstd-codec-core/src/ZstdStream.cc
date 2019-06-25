#include "zstd.h"
#include "zstd_codec_core/ZstdStream.h"

///////////////////////////////////////////////////////////////////////////////
//
// ZstdCompressStream
//
///////////////////////////////////////////////////////////////////////////////

class ZstdCompressStream::Impl {
public:
    using CStreamInitializer = std::function<size_t(ZSTD_CStream *)>;

    Impl() : stream_(nullptr, ZSTD_freeCStream), next_read_size_(), src_bytes_(), dest_bytes_() {
    }

    bool hasStream() const {
        return stream_ != nullptr;
    }

    bool begin(const CStreamInitializer& initializer) {
        if (hasStream()) return false;

        CStreamPtr stream(ZSTD_createCStream(), ZSTD_freeCStream);
        if (stream == nullptr) return false;

        const auto init_rc = initializer(stream.get());
        if (ZSTD_isError(init_rc)) return false;

        stream_ = std::move(stream);
        src_bytes_.reserve(ZSTD_CStreamInSize());
        dest_bytes_.resize(ZSTD_CStreamOutSize());  // resize
        next_read_size_ = src_bytes_.capacity();

        return true;
    }

    bool transform(const Vec<u8>& chunk, const StreamCallback& callback)
    {
        if (!hasStream()) return false;

        auto chunk_offset = 0u;
        while (chunk_offset < chunk.size()) {
            const auto src_available = src_bytes_.capacity() - src_bytes_.size();
            const auto chunk_remains = chunk.size() - chunk_offset;
            const auto copy_size = std::min(src_available, chunk_remains);

            const auto copy_begin = std::begin(chunk) + chunk_offset;
            const auto copy_end = copy_begin + copy_size;

            chunk_offset += copy_size;

            // append src bytes
            std::copy(copy_begin, copy_end, std::back_inserter(src_bytes_));

            // compress if enough bytes ready
            if (src_bytes_.size() >= next_read_size_ || src_available == 0u) {
                const auto success = compress(callback);
                if (!success) return false;
            }
        }

        return true;
    }

    bool end(const StreamCallback& callback ) {
        if (!hasStream()) return true;

        const auto success = src_bytes_.empty() || compress(callback);
        if (success) {
            dest_bytes_.resize(dest_bytes_.capacity());
            ZSTD_outBuffer output { &dest_bytes_[0], dest_bytes_.size(), 0 };
            const auto remaining = ZSTD_endStream(stream_.get(), &output);
            if (remaining > 0u) return false;

            dest_bytes_.resize(output.pos);
            callback(dest_bytes_);
        }

        stream_.reset();
        return success;
    }

    bool compress(const StreamCallback &callback) {
        if (src_bytes_.empty()) return true;

        ZSTD_inBuffer input{&src_bytes_[0], src_bytes_.size(), 0};
        while (input.pos < input.size) {
            dest_bytes_.resize(dest_bytes_.capacity());
            ZSTD_outBuffer output{&dest_bytes_[0], dest_bytes_.size(), 0};
            next_read_size_ = ZSTD_compressStream(stream_.get(), &output, &input);
            if (ZSTD_isError(next_read_size_)) return false;

            dest_bytes_.resize(output.pos);
            callback(dest_bytes_);
        }

        src_bytes_.clear();
        return true;
    }

private:
    using CStreamPtr = std::unique_ptr<ZSTD_CStream, decltype(&ZSTD_freeCStream)>;

    CStreamPtr stream_;
    size_t next_read_size_;
    Vec<u8> src_bytes_;
    Vec<u8> dest_bytes_;
};

ZstdCompressStream::ZstdCompressStream() : pimpl_(new Impl()) {
}

ZstdCompressStream::~ZstdCompressStream() = default;

bool ZstdCompressStream::begin(int compression_level) {
    return pimpl_->begin([compression_level](ZSTD_CStream* cstream) {
        return ZSTD_initCStream(cstream, compression_level);
    });
}

bool ZstdCompressStream::begin(const ZstdCompressionDict &cdict) {
    /*
    return pimpl_->begin([&cdict](ZSTD_CStream* cstream) {
        return ZSTD_initCStream_usingCDict(cstream, cdict.get());
    });
     */
    return false;
}

bool ZstdCompressStream::transform(const Vec<u8> &chunk, const StreamCallback& callback) {
    return pimpl_->transform(chunk, callback);
}

bool ZstdCompressStream::flush(const StreamCallback& callback) {
    return pimpl_->compress(callback);
}

bool ZstdCompressStream::end(const StreamCallback& callback) {
    return pimpl_->end(callback);
}

///////////////////////////////////////////////////////////////////////////////
//
// ZstdDecompressStream
//
///////////////////////////////////////////////////////////////////////////////

class ZstdDecompressStream::Impl {
public:
    Impl() : stream_(nullptr, ZSTD_freeDStream), next_read_size_(), src_bytes_(), dest_bytes_()  {
    }

    using DStreamInitializer = std::function<size_t(ZSTD_DStream*)>;

    bool hasStream() const {
        return stream_ != nullptr;
    }

    bool begin(const DStreamInitializer& initializer){
        if (hasStream()) return true;

        DStreamPtr stream(ZSTD_createDStream(), ZSTD_freeDStream);
        if (stream == nullptr) return false;

        const auto init_rc = initializer(stream.get());
        if (ZSTD_isError(init_rc)) return false;

        stream_ = std::move(stream);
        src_bytes_.reserve(ZSTD_DStreamInSize());
        dest_bytes_.resize(ZSTD_DStreamOutSize());  // resize
        next_read_size_ = init_rc;

        return true;
    }

    bool transform(const Vec<u8>& chunk, const StreamCallback& callback)
    {
        if (!hasStream()) return false;

        auto chunk_offset = 0u;
        while (chunk_offset < chunk.size()) {
            const auto src_available = src_bytes_.capacity() - src_bytes_.size();
            const auto chunk_remains = chunk.size() - chunk_offset;
            const auto copy_size = std::min(src_available, chunk_remains);

            const auto copy_begin = std::begin(chunk) + chunk_offset;
            const auto copy_end = copy_begin + copy_size;

            chunk_offset += copy_size;

            // append src bytes
            std::copy(copy_begin, copy_end, std::back_inserter(src_bytes_));

            // compress if enough bytes ready
            if (src_bytes_.size() >= next_read_size_ || src_available == 0u) {
                const auto success = decompress(callback);
                if (!success) return false;
            }
        }

        return true;
    }

    bool end(const StreamCallback& callback) {
        if (!hasStream()) return true;

        const auto success = src_bytes_.empty() || decompress(callback);

        stream_.reset();
        return success;
    }

    bool decompress(const StreamCallback& callback) {
        if (src_bytes_.empty()) return true;

        ZSTD_inBuffer input { &src_bytes_[0], src_bytes_.size(), 0 };
        while (input.pos < input.size) {
            dest_bytes_.resize(dest_bytes_.capacity());
            ZSTD_outBuffer output { &dest_bytes_[0], dest_bytes_.size(), 0};
            next_read_size_ = ZSTD_decompressStream(stream_.get(), &output, &input);
            if (ZSTD_isError(next_read_size_)) return false;

            dest_bytes_.resize(output.pos);
            callback(dest_bytes_);
        }

        src_bytes_.clear();
        return true;
    }

private:
    using DStreamPtr = std::unique_ptr<ZSTD_DStream, decltype(&ZSTD_freeDStream)>;

    DStreamPtr  stream_;
    size_t      next_read_size_;
    Vec<u8>     src_bytes_;
    Vec<u8>     dest_bytes_;
};

ZstdDecompressStream::ZstdDecompressStream() : pimpl_(new ZstdDecompressStream::Impl()) {
}

ZstdDecompressStream::~ZstdDecompressStream() = default;

bool ZstdDecompressStream::begin() {
    return pimpl_->begin([](ZSTD_DStream* dstream) {
        return ZSTD_initDStream(dstream);
    });
}

bool ZstdDecompressStream::begin(const ZstdDecompressionDict &ddict) {
    /*
    return pimpl_->begin([&ddict](ZSTD_DStream* dstream) {
        return ZSTD_initDStream_usingDDict(dstream, ddict.get());
    });
     */
    return false;
}

bool ZstdDecompressStream::transform(const Vec<u8> &chunk, const StreamCallback &callback) {
    return pimpl_->transform(chunk, callback);
}

bool ZstdDecompressStream::flush(const StreamCallback &callback) {
    return pimpl_->decompress(callback);
}

bool ZstdDecompressStream::end(const StreamCallback &callback) {
    return pimpl_->end(callback);
}
