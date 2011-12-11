#include <string.h>
#include <crypto/ffx.hh>

using namespace std;

static void mem_to_u64(const uint8_t *p,
                       uint64_t *a, uint64_t *b,
                       uint abits, uint bbits)
{
    assert(abits <= 64 && bbits <= 64);

    *a = 0;
    *b = 0;

    while (abits >= 8) {
        *a = *a << 8 | *p;
        p++;
        abits -= 8;
    }

    if (abits) {
        *a = *a << abits | *p >> (8 - abits);
        uint8_t pleft = *p & ((1 << (8 - abits)) - 1);
        if (bbits < 8 - abits) {
            *b = pleft >> (8 - bbits);
            bbits = 0;
        } else {
            *b = pleft;
            bbits -= 8 - abits;
        }
        p++;
    }

    while (bbits >= 8) {
        *b = *b << 8 | *p;
        p++;
        bbits -= 8;
    }

    if (bbits)
        *b = *b << bbits | *p >> (8 - bbits);
}

static void u64_to_mem(uint64_t a, uint64_t b,
                       uint64_t abits, uint64_t bbits,
                       uint8_t *p)
{
    assert(abits <= 64 && bbits <= 64);

    while (abits >= 8) {
        *p = a >> (abits - 8);
        p++;
        abits -= 8;
    }

    if (abits) {
        *p = a & ((1 << abits) - 1);
        if (bbits < 8 - abits) {
            *p = (*p << bbits | (b & ((1 << bbits) - 1))) << (8 - abits - bbits);
            bbits = 0;
        } else {
            *p = *p << (8 - abits) | b >> (bbits - (8 - abits));
            bbits -= (8 - abits);
        }
        p++;
    }

    while (bbits >= 8) {
        *p = b >> (bbits - 8);
        p++;
        bbits -= 8;
    }

    if (bbits)
        *p = b << (8 - bbits);
}

struct a2_mac_header {
    uint16_t ver;
    uint8_t method;
    uint8_t addition;
    uint8_t radix;
    uint8_t n;
    uint8_t s;
    uint8_t rounds;
    uint64_t tlen;

    a2_mac_header(uint64_t narg, const vector<uint8_t> &t)
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

static uint64_t f(const a2_mac_header &h, const AES *k,
                  uint8_t i, uint64_t b,
                  const vector<uint8_t> &t)
{
    cbcmac<AES> mac(k);
    mac.update(&h, sizeof(h));
    mac.update(&t[0], t.size());

    struct {
        uint8_t zero[16 + 7];
        uint8_t i;
        uint64_t b;
    } tail;

    uint tailoff = t.size() % 16;
    if (tailoff + 16 <= sizeof(tail.zero))
        tailoff += 16;

    memset(tail.zero, 0, sizeof(tail.zero));
    tail.i = i;
    tail.b = b;
    mac.update(&tail.zero[tailoff], sizeof(tail) - tailoff);

    uint8_t out[16];
    mac.final(out);

    uint m = (i % 2) ? (h.n -  h.s) : h.s;
    uint64_t r, dummy;
    mem_to_u64(out, &r, &dummy, m, 0);
    return r;
}

void ffx_a2::encrypt(const uint8_t *pt, uint8_t *ct, uint nbits,
                     const vector<uint8_t> &t) const
{
    const a2_mac_header h(nbits, t);

    uint64_t a, b, c;
    mem_to_u64(pt, &a, &b, h.s, h.n - h.s);

    for (int i = 0; i < h.rounds; i++) {
        c = a ^ f(h, k, i, b, t);
        a = b;
        b = c;
    }

    u64_to_mem(a, b, h.s, h.n - h.s, ct);
}

void ffx_a2::decrypt(const uint8_t *ct, uint8_t *pt, uint nbits,
                     const vector<uint8_t> &t) const
{
    const a2_mac_header h(nbits, t);

    uint64_t a, b, c;
    mem_to_u64(ct, &a, &b, h.s, h.n - h.s);

    for (int i = h.rounds - 1; i >= 0; i--) {
        c = b;
        b = a;
        a = c ^ f(h, k, i, b, t);
    }

    u64_to_mem(a, b, h.s, h.n - h.s, pt);
}
