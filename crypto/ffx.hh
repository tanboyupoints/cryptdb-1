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

struct ffx_a2_mac_header {
    uint16_t ver;
    uint8_t method;
    uint8_t addition;
    uint8_t radix;
    uint8_t n;
    uint8_t s;
    uint8_t rounds;
    uint64_t tlen;

    ffx_a2_mac_header(uint64_t narg, const std::vector<uint8_t> &t)
        : ver(1), method(2), addition(0), radix(2), n(narg),
          s(n/2), tlen(t.size())
    {
        assert(n >= 8 && n <= 128);

        if (n <= 9)
            rounds = 36;
        else if (n <= 13)
            rounds = 30;
        else if (n <= 19)
            rounds = 24;
        else if (n <= 31)
            rounds = 18;
        else
            rounds = 12;
    }
};

class ffx_a2_inited : public ffx_a2_mac_header {
 public:
    ffx_a2_inited(const AES *key, uint nbits, const std::vector<uint8_t> &t);

    /*
     * For non-multiple-of-8-bit values, the bits come from MSB.
     */
    void encrypt(const uint8_t *pt, uint8_t *ct) const;
    void decrypt(const uint8_t *ct, uint8_t *pt) const;

 private:
    uint64_t f(uint8_t i, uint64_t b) const;

    const AES *k;
    cbcmac<AES> mac_base;
    uint tailoff;
};

class ffx_a2 {
 public:
    ffx_a2(const AES *key) {
        k = key;
    }

    ffx_a2_inited init(uint nbits, const std::vector<uint8_t> &t) const {
        ffx_a2_inited fi(k, nbits, t);
        return fi;
    }

    void encrypt(const uint8_t *pt, uint8_t *ct, uint nbits,
                 const std::vector<uint8_t> &t) const {
        auto fi = init(nbits, t);
        fi.encrypt(pt, ct);
    }

    void decrypt(const uint8_t *ct, uint8_t *pt, uint nbits,
                 const std::vector<uint8_t> &t) const {
        auto fi = init(nbits, t);
        fi.decrypt(ct, pt);
    }

 private:
    const AES *k;
};

template<uint nbits>
class ffx_a2_block_cipher {
 public:
    ffx_a2_block_cipher(const ffx_a2 *f, const std::vector<uint8_t> &t)
        : fi(f->init(nbits, t)) {}

    void block_encrypt(const uint8_t *ptext, uint8_t *ctext) const {
        fi.encrypt(ptext, ctext);
    }

    void block_decrypt(const uint8_t *ctext, uint8_t *ptext) const {
        fi.decrypt(ctext, ptext);
    }

    static const size_t blocksize = (nbits + 7) / 8;

 private:
    const ffx_a2_inited fi;
};
