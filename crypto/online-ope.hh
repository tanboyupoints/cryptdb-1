#pragma once

#include <string>
#include <vector>
#include <crypto/blowfish.hh>

struct tree_node;

class ope_server {
 public:
    uint64_t lookup(const std::vector<bool> &v) const;
    void insert(const std::vector<bool> &v, uint64_t encval);

 private:
    struct tree_node *root;
};

class ope_client {
 public:
    ope_client(blowfish *bf, ope_server *server) : b(bf), s(server) {}

    uint64_t decrypt(const std::vector<bool> &ct) const;
    std::vector<bool> encrypt(uint64_t pt) const;

 private:
    uint64_t local_decrypt(uint64_t ct) const;
    uint64_t local_encrypt(uint64_t pt) const;

    blowfish *b;
    ope_server *s;
};
