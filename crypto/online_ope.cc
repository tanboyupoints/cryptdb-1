#include <assert.h>
#include <string.h>
#include <crypto/online_ope.hh>

template<class EncT>
struct tree_node {
    EncT enc_val;
    tree_node *left;
    tree_node *right;

    tree_node(const EncT &ev) : enc_val(ev), left(0), right(0) {}
    ~tree_node() {
        if (left)
            delete left;
        if (right)
            delete right;
    }

    uint64_t height() {
        uint64_t h = 0;
        if (left)
            h = max(h, 1 + left->height());
        if (right)
            h = max(h, 1 + right->height());
        return h;
    }

    uint64_t count() {
        uint64_t n = 1;
        if (left)
            n += left->count();
        if (right)
            n += right->count();
        return n;
    }
};

template<class EncT>
static tree_node<EncT> *
tree_lookup(tree_node<EncT> *root, uint64_t v, uint64_t nbits)
{
    if (nbits == 0)
        return root;

    if (!root)
        return 0;

    return tree_lookup((v&(1ULL<<(nbits-1))) ? root->right : root->left, v, nbits-1);
}

template<class EncT>
static void
tree_insert(tree_node<EncT> **np, uint64_t v,
            const EncT &encval, uint64_t nbits)
{
    if (nbits == 0) {
        assert(*np == 0);
        *np = new tree_node<EncT>(encval);
    } else {
        assert(*np);
        tree_insert((v&(1ULL<<(nbits-1))) ? &(*np)->right : &(*np)->left,
                    v, encval, nbits-1);
    }
}

template<class EncT>
EncT
ope_server<EncT>::lookup(uint64_t v, uint64_t nbits) const
{
    auto n = tree_lookup(root, v, nbits);
    if (!n)
        throw ope_lookup_failure();

    return n->enc_val;
}

template<class EncT>
void
ope_server<EncT>::insert(uint64_t v, uint64_t nbits, const EncT &encval)
{
    tree_insert(&root, v, encval, nbits);
}

template<class EncT>
ope_server<EncT>::~ope_server()
{
    if (root)
        delete root;
}

/*
 * Explicitly instantiate the ope_server template for various ciphertext types.
 */
template class ope_server<uint64_t>;
template class ope_server<uint32_t>;
template class ope_server<uint16_t>;
