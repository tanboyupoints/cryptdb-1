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
    ZZ l = trunc_ZZ(abr, _mbits);   // low bits that need to be shot down
    ZZ c = trunc_ZZ(l * _minusm_inv_modr, _mbits);

    // assert((abr + c * _m) % _r == 0);

    ZZ ab = (abr + c * _m) >> _mbits;
    if (ab >= _m)
        return ab - _m;
    else
        return ab;
}
