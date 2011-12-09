#pragma once

#include <vector>
#include <stdint.h>

template<class BlockCipher>
class cbcmac {
 public:
    cbcmac(const BlockCipher *cx) {
        c = cx;
        memset(v, 0, BlockCipher::blocksize);
    }

    void update(const void *data, size_t len) {
        assert(len % BlockCipher::blocksize == 0);
        const uint8_t *d = static_cast<const uint8_t *> (data);

        for (size_t off = 0; off < len; off += BlockCipher::blocksize) {
            uint8_t x[BlockCipher::blocksize];
            for (size_t i = 0; i < BlockCipher::blocksize; i++)
                x[i] = v[i] ^ d[off + i];
            c->block_encrypt(x, v);
        }
    }

    void update(const std::vector<uint8_t> &v) {
        update(&v[0], v.size());
    }

    void final(uint8_t *buf) {
        memcpy(buf, v, BlockCipher::blocksize);
    }

    std::vector<uint8_t> final() {
        std::vector<uint8_t> f(BlockCipher::blocksize);
        final(&f[0]);
        return f;
    }

 private:
    uint8_t v[BlockCipher::blocksize];
    const BlockCipher *c;
};
