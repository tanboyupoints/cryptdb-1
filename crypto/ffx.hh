#pragma once

#include <util/static_assert.hh>

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

struct ffx_a2_mac_header {
    const uint16_t ver;
    const uint8_t method;
    const uint8_t addition;
    const uint8_t radix;
    const uint8_t n;
    const uint8_t s;
    const uint8_t rounds;
    const uint64_t tlen;

    ffx_a2_mac_header(uint64_t narg, const std::vector<uint8_t> &t)
        : ver(1), method(2), addition(0), radix(2), n(narg),
          s(n/2), rounds(rnds(narg)), tlen(t.size()) {}

 private:
    static uint8_t rnds(uint8_t n) {
        assert(n >= 8 && n <= 128);

        if (n <= 9)
            return 36;
        if (n <= 13)
            return 30;
        if (n <= 19)
            return 24;
        if (n <= 31)
            return 18;
        return 12;
    }
};

class ffx_a2 : public ffx_a2_mac_header {
 public:
    ffx_a2(const AES *key, uint nbits, const std::vector<uint8_t> &t);

    /*
     * For non-multiple-of-8-bit values, the bits come from MSB.
     */
    void encrypt(const uint8_t *pt, uint8_t *ct) const;
    void decrypt(const uint8_t *ct, uint8_t *pt) const;

 private:
    uint64_t f(uint8_t i, uint64_t b) const;

    cbcmac<AES> mac_base;
};

template<uint nbits>
class ffx_a2_block_cipher {
 public:
    ffx_a2_block_cipher(const AES *key, const std::vector<uint8_t> &t)
        : fi(key, nbits, t)
    {
        _static_assert(nbits % 8 == 0);
    }

    void block_encrypt(const uint8_t *ptext, uint8_t *ctext) const {
        fi.encrypt(ptext, ctext);
    }

    void block_decrypt(const uint8_t *ctext, uint8_t *ptext) const {
        fi.decrypt(ctext, ptext);
    }

    static const size_t blocksize = (nbits + 7) / 8;

 private:
    const ffx_a2 fi;
};
