#pragma once

#include <string>
#include <crypto/blowfish.hh>
#include <util/static_assert.hh>

template<class EncT>
struct tree_node;

class ope_lookup_failure {};

template<class EncT>
class ope_server {
 public:
    EncT lookup(uint64_t v, uint64_t nbits) const;
    void insert(uint64_t v, uint64_t nbits, const EncT &encval);
    ~ope_server();

 private:
    tree_node<EncT> *root;
};

template<class V, class BlockCipher>
class ope_client {
 public:
    ope_client(BlockCipher *bc, ope_server<V> *server) : b(bc), s(server) {
        _static_assert(BlockCipher::blocksize == sizeof(V));
    }

    V decrypt(uint64_t ct) const {
        uint64_t nbits = 64 - ffsl(ct);
        return block_decrypt(s->lookup(ct>>(64-nbits), nbits));
    }

    uint64_t encrypt(V pt) const {
        uint64_t v = 0;
        uint64_t nbits = 0;
        try {
            for (;;) {
                V xct = s->lookup(v, nbits);
                V xpt = block_decrypt(xct);
                if (pt == xpt)
                    break;
                if (pt < xpt)
                    v = (v<<1) | 0;
                else
                    v = (v<<1) | 1;
                nbits++;
            }
        } catch (ope_lookup_failure&) {
            s->insert(v, nbits, block_encrypt(pt));
        }

        assert(nbits <= 63);
        return (v<<(64-nbits)) | (1ULL<<(63-nbits));
    }

 private:
    V block_decrypt(V ct) const {
        V pt;
        b->block_decrypt((const uint8_t *) &ct, (uint8_t *) &pt);
        return pt;
    }

    V block_encrypt(V pt) const {
        V ct;
        b->block_encrypt((const uint8_t *) &pt, (uint8_t *) &ct);
        return ct;
    }

    BlockCipher *b;
    ope_server<V> *s;
};
