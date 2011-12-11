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

ffx_a2::ffx_a2(const AES *key, uint n, const vector<uint8_t> &t)
    : ffx_a2_mac_header(n, t), k(key), mac_base(k)
{
    auto h = static_cast<const ffx_a2_mac_header *> (this);
    mac_base.update(h, sizeof(*h));
    mac_base.update(&t[0], t.size());
}

uint64_t
ffx_a2::f(uint8_t i, uint64_t b) const
{
    cbcmac<AES> mac = mac_base;

    struct {
        uint8_t alignpad[7];
        uint8_t i;
        uint64_t b;
    } tail = { { 0 }, i, b };
    mac.update(&tail.i, sizeof(tail) - sizeof(tail.alignpad));

    uint8_t out[16];
    mac.final(out);

    uint m = (i % 2) ? (n -  s) : s;
    uint64_t r, dummy;
    mem_to_u64(out, &r, &dummy, m, 0);
    return r;
}

void
ffx_a2::encrypt(const uint8_t *pt, uint8_t *ct) const
{
    uint64_t a, b, c;
    mem_to_u64(pt, &a, &b, s, n - s);

    for (int i = 0; i < rounds; i++) {
        c = a ^ f(i, b);
        a = b;
        b = c;
    }

    u64_to_mem(a, b, s, n - s, ct);
}

void
ffx_a2::decrypt(const uint8_t *ct, uint8_t *pt) const
{
    uint64_t a, b, c;
    mem_to_u64(ct, &a, &b, s, n - s);

    for (int i = rounds - 1; i >= 0; i--) {
        c = b;
        b = a;
        a = c ^ f(i, b);
    }

    u64_to_mem(a, b, s, n - s, pt);
}
