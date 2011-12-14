#include <assert.h>
#include <string.h>
#include <crypto/online-ope.hh>

struct tree_node {
    uint64_t enc_val;
    struct tree_node *left;
    struct tree_node *right;

    tree_node(uint64_t ev) : enc_val(ev), left(0), right(0) {}
    ~tree_node() {
        if (left)
            delete left;
        if (right)
            delete right;
    }
};

static struct tree_node *
tree_lookup(struct tree_node *root, uint64_t v, uint64_t nbits)
{
    if (nbits == 0)
        return root;

    if (!root)
        return 0;

    return tree_lookup((v&1) ? root->right : root->left, v>>1, nbits-1);
}

static void
tree_insert(struct tree_node **np, uint64_t v,
            uint64_t encval, uint64_t nbits)
{
    if (nbits == 0) {
        assert(*np == 0);
        struct tree_node *n = new tree_node(encval);
        *np = n;
    } else {
        assert(*np);
        tree_insert((v&1) ? &(*np)->right : &(*np)->left,
                    v>>1, encval, nbits-1);
    }
}

class lookup_failure {
};

uint64_t
ope_server::lookup(uint64_t v, uint64_t nbits) const
{
    struct tree_node *n = tree_lookup(root, v, nbits);
    if (!n)
        throw lookup_failure();    /* XXX */

    return n->enc_val;
}

void
ope_server::insert(uint64_t v, uint64_t nbits, uint64_t encval)
{
    tree_insert(&root, v, encval, nbits);
}

ope_server::~ope_server()
{
    if (root)
        delete root;
}

uint64_t
ope_client::local_decrypt(uint64_t ct) const
{
    uint64_t pt;
    b->block_decrypt((const uint8_t *) &ct, (uint8_t *) &pt);
    return pt;
}

uint64_t
ope_client::local_encrypt(uint64_t pt) const
{
    uint64_t ct;
    b->block_encrypt((const uint8_t *) &pt, (uint8_t *) &ct);
    return ct;
}

uint64_t
ope_client::decrypt(uint64_t v) const
{
    uint64_t nbits = 64-ffsl(v);
    return local_decrypt(s->lookup(v>>(64-nbits), nbits));
}

uint64_t
ope_client::encrypt(uint64_t pt) const
{
    uint64_t v = 0;
    uint64_t nbits = 0;
    try {
        for (;;) {
            uint64_t xct = s->lookup(v, nbits);
            uint64_t xpt = local_decrypt(xct);
            if (pt == xpt)
                return v;
            if (pt < xpt)
                v |= 0<<nbits;
            else
                v |= 1<<nbits;
            nbits++;
        }
    } catch (lookup_failure&) {
        s->insert(v, nbits, local_encrypt(pt));
    }

    assert(nbits <= 63);
    return (v<<(64-nbits)) | (1ULL<<(63-nbits));
}
