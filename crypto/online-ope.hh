#pragma once

#include <string>
#include <crypto/blowfish.hh>

struct tree_node;

class ope_server {
 public:
    uint64_t lookup(uint64_t v, uint64_t nbits) const;
    void insert(uint64_t v, uint64_t nbits, uint64_t encval);
    ~ope_server();

 private:
    struct tree_node *root;
};

class ope_client {
 public:
    ope_client(blowfish *bf, ope_server *server) : b(bf), s(server) {}

    uint64_t decrypt(uint64_t ct) const;
    uint64_t encrypt(uint64_t pt) const;

 private:
    uint64_t local_decrypt(uint64_t ct) const;
    uint64_t local_encrypt(uint64_t pt) const;

    blowfish *b;
    ope_server *s;
};
