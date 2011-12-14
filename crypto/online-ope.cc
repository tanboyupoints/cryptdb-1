#include <assert.h>
#include <crypto/online-ope.hh>

struct tree_node {
    uint64_t enc_val;
    struct tree_node *left;
    struct tree_node *right;

    tree_node(uint64_t ev) : enc_val(ev), left(0), right(0) {}
};

static struct tree_node *
tree_lookup(struct tree_node *root,
            const std::vector<bool> &v,
            uint vidx)
{
    if (v.size() == vidx)
        return root;

    if (!root)
        return 0;

    return tree_lookup(v[vidx] ? root->right : root->left, v, vidx+1);
}

static void
tree_insert(struct tree_node **np,
            const std::vector<bool> &v,
            uint64_t encval,
            uint vidx)
{
    if (v.size() == vidx) {
        assert(*np == 0);
        struct tree_node *n = new tree_node(encval);
        *np = n;
    } else {
        assert(*np);
        tree_insert(v[vidx] ? &(*np)->right : &(*np)->left,
                    v, encval, vidx+1);
    }
}

class lookup_failure {
};

uint64_t
ope_server::lookup(const std::vector<bool> &v) const
{
    struct tree_node *n = tree_lookup(root, v, 0);
    if (!n)
        throw lookup_failure();    /* XXX */

    return n->enc_val;
}

void
ope_server::insert(const std::vector<bool> &v, uint64_t encval)
{
    tree_insert(&root, v, encval, 0);
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
ope_client::decrypt(const std::vector<bool> &v) const
{
    return local_decrypt(s->lookup(v));
}

std::vector<bool>
ope_client::encrypt(uint64_t pt) const
{
    std::vector<bool> v;
    try {
        for (;;) {
            uint64_t xct = s->lookup(v);
            uint64_t xpt = local_decrypt(xct);
            if (pt == xpt)
                return v;
            if (pt < xpt)
                v.push_back(false);
            else
                v.push_back(true);
        }
    } catch (lookup_failure&) {
        s->insert(v, local_encrypt(pt));
    }

    return v;
}
