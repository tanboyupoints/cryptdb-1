#pragma once

#include <list>
#include <vector>
#include <NTL/ZZ.h>

class Paillier {
 public:
    Paillier(const std::vector<NTL::ZZ> &pk);
    std::vector<NTL::ZZ> pubkey() const { return { n, g }; }

    NTL::ZZ encrypt(const NTL::ZZ &plaintext);
    NTL::ZZ add(const NTL::ZZ &c0, const NTL::ZZ &c1) const;
    NTL::ZZ mul(const NTL::ZZ &ciphertext, const NTL::ZZ &constval) const;

    void rand_gen(size_t niter = 100, size_t nmax = 1000);

    template<class PackT>
    uint32_t pack_count() {
        return (nbits + sizeof(PackT)*16) / 2 / (sizeof(PackT)*16);
    }

    template<class PackT>
    NTL::ZZ encrypt_pack(const std::vector<PackT> &items) {
        uint32_t npack = pack_count<PackT>();
        assert(items.size() == npack);

        NTL::ZZ sum = NTL::to_ZZ(0);
        for (uint i = 0; i < npack; i++)
            sum += NTL::to_ZZ(items[i]) << (i*sizeof(PackT)*16);
        return encrypt(sum);
    }

    template<class PackT>
    NTL::ZZ add_pack(const NTL::ZZ &agg, const NTL::ZZ &pack, uint32_t packidx) {
        uint32_t npack = pack_count<PackT>();
        assert(packidx < npack);

        NTL::ZZ s = mul(pack, NTL::to_ZZ(1) << (npack-1-packidx) * (sizeof(PackT)*16));
        return add(agg, s);
    }

 protected:
    /* Public key */
    const NTL::ZZ n, g;

    /* Cached values */
    const uint nbits;
    const NTL::ZZ n2;

    /* Pre-computed randomness */
    std::list<NTL::ZZ> rqueue;
};

class Paillier_priv : public Paillier {
 public:
    Paillier_priv(const std::vector<NTL::ZZ> &sk);
    std::vector<NTL::ZZ> privkey() const { return { p, q, g, a }; }

    NTL::ZZ decrypt(const NTL::ZZ &ciphertext) const;

    static std::vector<NTL::ZZ> keygen(uint nbits = 1024, uint abits = 256);

    template<class PackT>
    PackT decrypt_pack(const NTL::ZZ &pack) {
        uint32_t npack = pack_count<PackT>();
        NTL::ZZ plain = decrypt(pack);
        PackT result;
        NTL::conv(result, plain >> (npack - 1) * sizeof(PackT) * 16);
        return result;
    }

 private:
    /* Private key, including g from public part; n=pq */
    const NTL::ZZ p, q;
    const NTL::ZZ a;      /* non-zero for fast mode */

    /* Cached values */
    const bool fast;
    const NTL::ZZ p2, q2;
    const NTL::ZZ two_p, two_q;
    const NTL::ZZ pinv, qinv;
    const NTL::ZZ hp, hq;
};
