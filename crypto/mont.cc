#include <algorithm>
#include <assert.h>
#include <crypto/mont.hh>

using namespace std;
using namespace NTL;

ZZ
montgomery::to_mont(const ZZ &a)
{
    return MulMod(a, _r, _m);
}

ZZ
montgomery::from_mont(const ZZ &a)
{
    return MulMod(a, _rinv, _m);
}

ZZ
montgomery::mmul(const ZZ &a, const ZZ &b)
{
    ZZ abr = a * b;

    for (uint i = 0; i < _mbits; i += sizeof(long)) {
        uint thisbits = std::min((uint) sizeof(long), _mbits - i);
        long l = trunc_long(abr >> i, thisbits);  // bits to cancel out
        long c = _minusm_inv_modr * l & (~(long)0 >> (sizeof(long)-thisbits));
        abr += (_m * c) << i;
    }

    // assert(abr % _r == 0);
    ZZ ab = abr >> _mbits;
    while (ab >= _m)
        ab = ab - _m;
    return ab;
}
