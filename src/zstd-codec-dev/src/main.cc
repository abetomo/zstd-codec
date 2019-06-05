#include "zstd_codec_core/ZstdApi.h"
#include "zstd_codec_core/types.h"

int main() {
    Vec<u8> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Vec<u8> buff;

    ZstdApi().compress(buff, data, 5);
    return 0;
}