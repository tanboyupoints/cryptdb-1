#pragma once

/*
 * Based on "The FFX Mode of Operation for Format-Preserving Encryption"
 * by Bellare, Rogaway, and Spies.
 * http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/ffx/ffx-spec.pdf
 *
 * This implementation follows FFX-A2, and is hard-coded to use:
 *   radix:    2 (binary)
 *   addition: 0 (character-wise addition, i.e., XOR)
 *   method:   2 (alternating Feistel)
 *   F:        CBC-MAC using AES
 */

#include <sys/types.h>
#include <crypto/aes.hh>
#include <crypto/cbcmac.hh>

class ffx_a2 {
 public:
    ffx_a2(const AES *key) {
        k = key;
    }

    /*
     * For non-multiple-of-8-bit values, the bits come from MSB.
     */
    void encrypt(const uint8_t *pt, uint8_t *ct, uint nbits,
                 const std::vector<uint8_t> &t) const;
    void decrypt(const uint8_t *ct, uint8_t *pt, uint nbits,
                 const std::vector<uint8_t> &t) const;

 private:
    const AES *k;
};

template<uint nbits>
class ffx_a2_block_cipher {
 public:
    ffx_a2_block_cipher(const ffx_a2 *farg, const std::vector<uint8_t> &targ)
        : f(farg), t(targ) {}

    void block_encrypt(const uint8_t *ptext, uint8_t *ctext) const {
        f->encrypt(ptext, ctext, nbits, t);
    }

    void block_decrypt(const uint8_t *ctext, uint8_t *ptext) const {
        f->decrypt(ctext, ptext, nbits, t);
    }

    static const size_t blocksize = (nbits + 7) / 8;

 private:
    const ffx_a2 *f;
    const std::vector<uint8_t> &t;
};
