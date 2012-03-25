#pragma once

#include <vector>
#include <stdint.h>
#include <openssl/blowfish.h>

class blowfish {
 public:
    blowfish(const std::vector<uint8_t> &key) {
        BF_set_key(&k, key.size(), &key[0]);
    }

    void block_encrypt(const uint8_t *ptext, uint8_t *ctext) const {
        BF_ecb_encrypt(ptext, ctext, &k, BF_ENCRYPT);
    }

    void block_decrypt(const uint8_t *ctext, uint8_t *ptext) const {
        BF_ecb_encrypt(ctext, ptext, &k, BF_DECRYPT);
    }

    uint64_t encrypt(uint64_t pt) const {
        uint64_t ct;
        block_encrypt((const uint8_t*) &pt, (uint8_t*) &ct);
        return ct;
    }

    uint64_t decrypt(uint64_t ct) const {
        uint64_t pt;
        block_decrypt((const uint8_t*) &ct, (uint8_t*) &pt);
        return pt;
    }

    static const size_t blocksize = 8;

 private:
    BF_KEY k;
};
