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

#include <crypto/cbcmac.hh>

class ffx {
 public:
    ffx(AES *key) {
        k = key;
    }

    std::vector<uint8_t>
        encrypt(const std::vector<uint8_t> &x,
                const std::vector<uint8_t> &t) const
    {
        assert(x.size() % 2 == 0);
        uint64_t n = x.size() * 8;
        const ffx_mac_header h(n, t);

        uint64_t lbytes = h.s / 8;
        std::vector<uint8_t> a(&x[0], &x[lbytes]),
                             b(&x[lbytes+1], &x[lbytes*2]),
                             c;

        c.resize(lbytes);
        for (int i = 0; i < h.rounds; i++) {
            auto fk = f(n, t, i, b, h);
            for (uint j = 0; j < lbytes; j++)
                c[j] = a[j] ^ fk[j];
            a = b;
            b = c;
        }

        a.insert(a.end(), b.begin(), b.end());
        return a;
    }

    std::vector<uint8_t>
        decrypt(const std::vector<uint8_t> &y,
                const std::vector<uint8_t> &t) const
    {
        assert(y.size() % 2 == 0);
        uint64_t n = y.size() * 8;
        const ffx_mac_header h(n, t);

        uint64_t lbytes = h.s / 8;
        std::vector<uint8_t> a(&y[0], &y[lbytes]),
                             b(&y[lbytes+1], &y[lbytes*2]),
                             c;

        c.resize(lbytes);
        for (int i = h.rounds - 1; i >= 0; i--) {
            c = b;
            b = a;
            auto fk = f(n, t, i, b, h);
            for (uint j = 0; j < lbytes; j++)
                a[j] = c[j] ^ fk[j];
        }

        a.insert(a.end(), b.begin(), b.end());
        return a;
    }

 private:
    struct ffx_mac_header {
        uint16_t ver;
        uint8_t method;
        uint8_t addition;
        uint8_t radix;
        uint8_t n;
        uint8_t s;
        uint8_t rounds;
        uint64_t tlen;

        ffx_mac_header(uint64_t narg, const std::vector<uint8_t> &t)
            : ver(1), method(2), addition(0), radix(2), n(narg),
              s(split(n)), rounds(rnds(n)), tlen(t.size())
            {
                assert(n <= 128);
            }
    };

    static uint64_t split(uint64_t n) {
        return n / 2;
    }

    static uint8_t rnds(uint64_t n) {
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

    std::vector<uint8_t>
        f(uint64_t n, const std::vector<uint8_t> &t,
          uint8_t i, const std::vector<uint8_t> &b,
          const ffx_mac_header &h) const
    {
        assert(n % 8 == 0);

        cbcmac<AES> m(k);
        m.update(&h, sizeof(h));
        m.update(&t[0], t.size());

        /*
         * XXX difference from FFX: no zero padding around tweak & i
         */
        m.update(&i, 1);
        m.update(&b[0], b.size());

        std::vector<uint8_t> y = m.final();

        /*
         * XXX difference from FFX: take the first m bits of CBC-MAC output,
         * rather than the last m bits.
         */
        y.resize(h.s / 8);
        return y;
    }

    AES *k;
};
