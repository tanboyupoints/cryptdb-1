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
 */

#include <string.h>
#include <sys/types.h>
#include <crypto/aes.hh>
#include <crypto/cbcmac.hh>

void ffx_mem_to_u64(const uint8_t *p,
                    uint64_t *a, uint64_t *b,
                    uint abits, uint bbits);
void ffx_u64_to_mem(uint64_t a, uint64_t b,
                    uint64_t abits, uint64_t bbits,
                    uint8_t *p);

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

template<class BlockCipher>
class ffx_a2 : public ffx_a2_mac_header {
 public:
    ffx_a2(const BlockCipher *key, uint nbits, const std::vector<uint8_t> &t)
        : ffx_a2_mac_header(nbits, t), mac_base(key)
    {
        _static_assert(BlockCipher::blocksize >= 8);
        auto h = static_cast<const ffx_a2_mac_header *> (this);
        mac_base.update(h, sizeof(*h));
        mac_base.update(&t[0], t.size());
    }

    /*
     * For non-multiple-of-8-bit values, the bits come from MSB.
     */
    void encrypt(const uint8_t *pt, uint8_t *ct) const {
        uint64_t a, b, c;
        ffx_mem_to_u64(pt, &a, &b, s, n - s);

        for (int i = 0; i < rounds; i++) {
            c = a ^ f(i, b);
            a = b;
            b = c;
        }

        ffx_u64_to_mem(a, b, s, n - s, ct);
    }

    void decrypt(const uint8_t *ct, uint8_t *pt) const {
        uint64_t a, b, c;
        ffx_mem_to_u64(ct, &a, &b, s, n - s);

        for (int i = rounds - 1; i >= 0; i--) {
            c = b;
            b = a;
            a = c ^ f(i, b);
        }

        ffx_u64_to_mem(a, b, s, n - s, pt);
    }

 private:
    uint64_t f(uint8_t i, uint64_t b) const {
        auto mac = mac_base;

        struct {
            uint8_t alignpad[7];
            uint8_t i;
            uint64_t b;
        } tail = { { 0 }, i, b };
        mac.update(&tail.i, sizeof(tail) - sizeof(tail.alignpad));

        union {
            uint8_t u8[mac.blocksize];
            uint64_t u64;
        } out;
        mac.final(out.u8);

        uint m = (i % 2) ? (n -  s) : s;
        return out.u64 >> (64 - m);
    }

    cbcmac<BlockCipher> mac_base;
};

template<class BlockCipher, uint nbits>
class ffx_a2_block_cipher {
 public:
    ffx_a2_block_cipher(const BlockCipher *key, const std::vector<uint8_t> &t)
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
    const ffx_a2<BlockCipher> fi;
};
