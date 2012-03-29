/**
 *  This is the crypto manager.
 */

#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <istream>

#include <crypto-old/CryptoManager.hh>
#include <util/ctr.hh>
#include <util/cryptdb_log.hh>
#include <util/util.hh>


using namespace std;
using namespace NTL;

// TODO: simplify CryptoManager using a function taking from level to level
// for a type of data using union for answers or inputs
// TODO: optimizations for CryptAPP, HOM especially; for each user, for each
// field, keep the keys in the right format already and just load them rather
// than recompute

static ZZ __attribute__((unused))
Paillier_L(const ZZ & u, const ZZ & n)
{
    return (u - 1) / n;
}

static ZZ
Paillier_Lfast(const ZZ & u, const ZZ &ninv, const ZZ &two_n, const ZZ &n)
{
    return (((u - 1) * ninv) % two_n) % n;
}

static ZZ
LCM(const ZZ & a, const ZZ & b)
{
    return (a * b) / GCD(a, b);
}

AES_KEY *
CryptoManager::getmkey()
{
    return masterKey;
}

CryptoManager::CryptoManager(const string &masterKeyArg)
{
    VERBOSE = VERBOSE_G;
    masterKey = 0;
    setMasterKey(masterKeyArg);

    useEncTables = false;

    //setup Paillier encryption
    Paillier_fast = true;

    if (Paillier_fast) {
        uint abits = Paillier_len_bits/8;   /* ~256 bits for a 2048-bit n^2 */
        do {
            Paillier_a = RandomPrime_ZZ(abits);

            ZZ cp = RandomLen_ZZ(Paillier_len_bits/4-abits);
            ZZ cq = RandomLen_ZZ(Paillier_len_bits/4-abits);

            Paillier_p = Paillier_a * cp + 1;
            while (!ProbPrime(Paillier_p))
                Paillier_p += Paillier_a;

            Paillier_q = Paillier_a * cq + 1;
            while (!ProbPrime(Paillier_q))
                Paillier_q += Paillier_a;

            Paillier_n = Paillier_p * Paillier_q;
        } while ((NumBits(Paillier_n) != Paillier_len_bits / 2) ||
                 Paillier_p == Paillier_q);
    } else {
        Paillier_p = RandomPrime_ZZ(Paillier_len_bits/4);
        Paillier_q = RandomPrime_ZZ(Paillier_len_bits/4);
        Paillier_n = Paillier_p * Paillier_q;
    }

    if (Paillier_p > Paillier_q)
        swap(Paillier_p, Paillier_q);

    Paillier_p2 = Paillier_p * Paillier_p;
    Paillier_q2 = Paillier_q * Paillier_q;
    Paillier_n2 = Paillier_n * Paillier_n;

    Paillier_lambda = LCM(Paillier_p-1, Paillier_q-1);

    if (Paillier_fast) {
        Paillier_g = PowerMod(to_ZZ(2),
                              Paillier_lambda / Paillier_a,
                              Paillier_n);
    } else {
        Paillier_g = 1;
        do {
            Paillier_g++;
        } while (GCD(Paillier_L(PowerMod(Paillier_g,
                                         Paillier_lambda,
                                         Paillier_n2),
                                Paillier_n), Paillier_n) != to_ZZ(1));

        Paillier_dec_denom =
            InvMod(Paillier_L(PowerMod(Paillier_g, Paillier_lambda, Paillier_n2),
                              Paillier_n),
                   Paillier_n);
    }

    Paillier_2n = power(to_ZZ(2), NumBits(Paillier_n));
    Paillier_2p = power(to_ZZ(2), NumBits(Paillier_p));
    Paillier_2q = power(to_ZZ(2), NumBits(Paillier_q));

    Paillier_ninv = InvMod(Paillier_n, Paillier_2n);
    Paillier_pinv = InvMod(Paillier_p, Paillier_2p);
    Paillier_qinv = InvMod(Paillier_q, Paillier_2q);

    Paillier_hp =
        InvMod(Paillier_Lfast(PowerMod(Paillier_g % Paillier_p2,
                                       Paillier_fast ? Paillier_a : (Paillier_p-1),
                                       Paillier_p2),
                              Paillier_pinv, Paillier_2p, Paillier_p),
               Paillier_p);
    Paillier_hq =
        InvMod(Paillier_Lfast(PowerMod(Paillier_g % Paillier_q2,
                                       Paillier_fast ? Paillier_a : (Paillier_q-1),
                                       Paillier_q2),
                              Paillier_qinv, Paillier_2q, Paillier_q),
               Paillier_q);


}

void CryptoManager::generateRandomPool(unsigned int randomPoolSize, string filename) {
    ofstream file(filename, ios::app);

    /*
     * Pre-generate a bunch of r^n mod n^2 values..
     */

    file << "randomPool" << " " << StringFromVal(randomPoolSize) << "\n";

    HOMRandCache.clear();
    for (unsigned int i = 0; i < randomPoolSize; i++) {
        ZZ r = RandomLen_ZZ(Paillier_len_bits/2) % Paillier_n;
        ZZ rn = PowerMod(Paillier_g, Paillier_n*r, Paillier_n2);

        file << rn << "\n";
    }

    file.close();
}


SECLEVEL
highestEq(SECLEVEL sl)
{
    if (sl == SECLEVEL::DET) {
        return SECLEVEL::DET;
    } else {
        return sl;
    }
}

void CryptoManager::loadEncTables(const string &filename) {
    ifstream file(filename);

    useEncTables = true;

    LOG(crypto) << "loading enc tables\n";

    assert_s(file.is_open(), "could not open file " + filename);

    if (file.eof()) {
        return;
    }
    string fieldname;
    file >> fieldname;
    unsigned int count;
    file >> count;
    LOG(crypto_v) << "loading for " << fieldname << " count " << count << "\n";

    while (!file.eof() && fieldname != "HOM") {
        cerr << "loading for " << fieldname << " count " << count << "\n";
        map<unsigned int, uint64_t> * opemap = new map<unsigned int,uint64_t>();
        for (unsigned int i = 0; i < count; i++) {
            unsigned int v;
            file >> v;
            uint64_t enc;
            file >> enc;
            opemap->insert(pair<unsigned int, uint64_t>(v, enc));
        }
        OPEEncTable[fieldname] = opemap;

        if (!file.eof()) {
            file >> fieldname;
            file >> count;
            LOG(crypto_v) << "loading for " << fieldname << " count " << count << "\n";
        }
    }

    cerr << "loading for " << fieldname << " count " << count << "\n";
    if (!file.eof()) {
        //hom case
        for (unsigned int i = 0; i < count; i++) {
            unsigned v;
            file >> v;
            string enc;
            file >> enc;
            HOMEncTable[v] = unmarshallBinary(enc);

        }
    }

    if (!file.eof()) {
        file >> fieldname;
        file >> count;
         cerr << "loading for " << fieldname << " count " << count << "\n";

        for (unsigned int i = 0; i < count; i++) {
            ZZ val;
            file >> val;
            HOMRandCache.push_back(val);
        }

    }

    file.close();
}


string assembleWords(list<string> * words);
list<string> * getWords(string text);

string
CryptoManager::getKey(const string &uniqueFieldName, SECLEVEL sec)
{
    return getKey(masterKey, uniqueFieldName, sec);
}

string
CryptoManager::getKey(AES_KEY * masterKeyArg, const string &uniqueFieldName,
                      SECLEVEL sec)
{
    string id = uniqueFieldName + strFromVal((unsigned int) sec);

    unsigned char shaDigest[SHA_DIGEST_LENGTH];
    SHA1((const uint8_t *) &id[0], id.length(), shaDigest);

    string result;
    result.resize(AES_BLOCK_BYTES);
    AES_encrypt(shaDigest, (uint8_t *) &result[0], masterKeyArg);
    return result;
}

string
CryptoManager::marshallKey(const string &key)
{
    // we will be sending key as two big nums
    string res = "";

    for (unsigned int i = 0; i < AES_KEY_SIZE/bitsPerByte; i++) {
        res = res + strFromVal((unsigned int)(key[i])) + ",";
    }

    //remove last comma
    res.resize(res.length() - 1);
    return res;
}

AES_KEY *
CryptoManager::get_key_SEM(const string &key)
{
    ANON_REGION(__func__, &perf_cg);
    return get_AES_KEY(key);
}

string
CryptoManager::encrypt_SEM(const string &ptext, AES_KEY * enckey, uint64_t salt)
{
   return encrypt_AES_CBC(ptext, enckey, BytesFromInt(salt, SALT_LEN_BYTES), false);
}

string
CryptoManager::decrypt_SEM(const string &ctext, AES_KEY * deckey, uint64_t salt)
{
    return decrypt_AES_CBC(ctext, deckey, BytesFromInt(salt, SALT_LEN_BYTES), false);
}


/*
uint64_t
CryptoManager::encrypt_DET(uint64_t plaintext, BF_KEY * key)
{

    return encrypt_BF(plaintext, key);

}

uint64_t
CryptoManager::decrypt_DET(uint64_t ciphertext, BF_KEY * key)
{
    return decrypt_BF(ciphertext, key);

}

string
CryptoManager::encrypt_DET(const string & ptext, AES_KEY * enckey)
{

   return encrypt_AES_CMC(ptext, enckey);
}

string
CryptoManager::decrypt_DET(const string & ctext, AES_KEY * deckey)
{
    return decrypt_AES_CMC(ctext, deckey);
}
*/
void
CryptoManager::setMasterKey(const string &masterKeyArg)
{
    if (masterKey)
        delete masterKey;

    masterKey = getKey(masterKeyArg);

    RAND_seed((const uint8_t *) masterKeyArg.c_str(),
              (int) masterKeyArg.size());

    SetSeed(ZZFromStringFast(padForZZ(masterKeyArg)));
}

AES_KEY * CryptoManager::getKey(const string & key) {
    AES_KEY * resKey = new AES_KEY();
    string mkey = key;

    // PAD KEY to be AES_KEY_SIZE bits long
    if (mkey.size() < AES_KEY_BYTES) {
      char buf[AES_KEY_BYTES];
      memset(buf, 0, sizeof(buf));
      memcpy(buf, mkey.data(), mkey.size());
      mkey = string(buf, sizeof(buf));
    }

    AES_set_encrypt_key(
            (const uint8_t *) mkey.data(), AES_KEY_SIZE, resKey);

    return resKey;

}

//returns the concatenation of all words in the given list
string
assembleWords(list<string> * words)
{
    string res = "";

    for (list<string>::iterator it = words->begin(); it != words->end();
         it++) {
        res = res + *it;
    }

    return res;
}

string
CryptoManager::encrypt_Paillier(uint64_t val)
{
    if (useEncTables) {
        auto it = HOMEncTable.find(val);
        if (it != HOMEncTable.end()) {
            LOG(crypto_v) << "HOM hit for " << val;
            return it->second;
        }

        LOG(crypto_v) << "HOM miss for " << val;
    
    
        auto it2 = HOMRandCache.begin();
        if (it2 != HOMRandCache.end()) {
            ZZ rn = *it2;
            HOMRandCache.pop_front();

            ZZ c = (PowerMod(Paillier_g, to_ZZ((uint) val), Paillier_n2) * rn) % Paillier_n2;
            return StringFromZZ(c);
        }

        cerr << "HOM and RAND Pool miss for " << val << "\n";
    }
    
    ZZ r = RandomLen_ZZ(Paillier_len_bits/2) % Paillier_n;
    ZZ c = PowerMod(Paillier_g, to_ZZ((uint) val) + Paillier_n*r, Paillier_n2);
    return StringFromZZ(c);
    
}

int
CryptoManager::decrypt_Paillier(const string &ciphertext)
{
    ZZ c = ZZFromStringFast(padForZZ(ciphertext));

    ZZ mp =
        (Paillier_Lfast(PowerMod(c % Paillier_p2, Paillier_fast ? Paillier_a : (Paillier_p-1), Paillier_p2),
                        Paillier_pinv, Paillier_2p, Paillier_p)
         * Paillier_hp) % Paillier_p;
    ZZ mq =
        (Paillier_Lfast(PowerMod(c % Paillier_q2, Paillier_fast ? Paillier_a : (Paillier_q-1), Paillier_q2),
                        Paillier_qinv, Paillier_2q, Paillier_q)
         * Paillier_hq) % Paillier_q;

    ZZ m, pq;
    pq = 1;
    CRT(m, pq, mp, Paillier_p);
    CRT(m, pq, mq, Paillier_q);

    return to_int(m);
}

string
CryptoManager::getPKInfo()
{
    return StringFromZZ(Paillier_n2);
}
/*
void
CryptoManager::createEncryptionTables(int noOPEarg, int noHOMarg,
                                      list<string>  fieldsWithOPE)
{

    int encryptionsOfOne = 100;
    int noEncryptions = 5;

    noOPE = noOPEarg;
    noHOM = noHOMarg;

    OPEEncTable.clear();
    HOMEncTable.clear();

    struct timeval starttime, endtime;
    //OPE

    gettimeofday(&starttime, NULL);

    for (list<string>::iterator it = fieldsWithOPE.begin();
         it != fieldsWithOPE.end(); it++) {
        string anonName = *it;
        OPEEncTable[anonName] = map<int, uint64_t>();
        OPE * currentKey = get_key_OPE(getKey(anonName, SECLEVEL::OPE));
        for (int i = 0; i < noOPE; i++) {
            OPEEncTable[anonName][i] = encrypt_OPE(i, currentKey);
        }

    }
    gettimeofday(&endtime, NULL);
    LOG(crypto) << "time per OPE "
                << timeInSec(starttime, endtime) * 1000.0 / noOPE;

    gettimeofday(&starttime, NULL);
    // HOM
    for (int i = 0; i < encryptionsOfOne; i++) {
        HOMEncTable[1] = list<string>();
        HOMEncTable[1].push_back(encrypt_Paillier(1));
    }

    for (int i = 0; i < noHOM; i++) {
        if (i != 1) {
            HOMEncTable[i] = list<string>();
            for (int j = 0; j < noEncryptions; j++) {
                HOMEncTable[i].push_back(encrypt_Paillier(i));
            }
        }
    }

    gettimeofday(&endtime, NULL);
    LOG(crypto) << "per HOM "
                << timeInSec(starttime, endtime)*1000.0 /
    (encryptionsOfOne + noHOM * noEncryptions);

    LOG(crypto) << "entries in OPE table are:";
    for (map<string, map<int, uint64_t> >::iterator it = OPEEncTable.begin();
         it != OPEEncTable.end(); it++) {
        LOG(crypto) << it->first;
    }

    LOG(crypto) << "entries for HOM are:";
    for (auto it = HOMEncTable.begin(); it != HOMEncTable.end(); it++) {
        LOG(crypto) << it->first;
    }

    useEncTables = true;
}

void
CryptoManager::replenishEncryptionTables()
{
    assert_s(false, "unimplemented replenish");
}
*/

//**************** Public Key Cryptosystem (PKCS)
// ****************************************/

//marshall key
static string
DER_encode_RSA_public(RSA *rsa)
{
    string s;
    s.resize(i2d_RSAPublicKey(rsa, 0));

    uint8_t *next = (uint8_t *) &s[0];
    i2d_RSAPublicKey(rsa, &next);
    return s;
}

static RSA *
DER_decode_RSA_public(const string &s)
{
    const uint8_t *buf = (const uint8_t*) s.data();
    return d2i_RSAPublicKey(0, &buf, s.length());
}

//marshall key
static string
DER_encode_RSA_private(RSA *rsa)
{
    string s;
    s.resize(i2d_RSAPrivateKey(rsa, 0));

    uint8_t *next = (uint8_t *) &s[0];
    i2d_RSAPrivateKey(rsa, &next);
    return s;
}

static RSA *
DER_decode_RSA_private(const string &s)
{
    const uint8_t *buf = (const uint8_t*) s.data();
    return d2i_RSAPrivateKey(0, &buf, s.length());
}

static void
remove_private_key(RSA *r)
{
    r->d = r->p = r->q = r->dmp1 = r->dmq1 = r->iqmp = 0;
}

//Credits: the above five functions are from "secure programming cookbook for
// C++"

void
CryptoManager::generateKeys(PKCS * & pk, PKCS * & sk)
{
    LOG(crypto) << "pkcs generate";
    PKCS * key =  RSA_generate_key(PKCS_bytes_size*8, 3, NULL, NULL);

    sk = RSAPrivateKey_dup(key);

    pk = key;
    remove_private_key(pk);

}

string
CryptoManager::marshallKey(PKCS * mkey, bool ispk)
{
    LOG(crypto) << "pkcs encrypt";
    string key;
    if (!ispk) {
        key = DER_encode_RSA_private(mkey);
    } else {
        key = DER_encode_RSA_public(mkey);
    }
    assert_s(key.length() >= 1, "issue with RSA pk \n");
    return key;
}

PKCS *
CryptoManager::unmarshallKey(const string &key, bool ispk)
{
    LOG(crypto) << "pkcs decrypt";
    //cerr << "before \n";
    if (ispk) {
        return DER_decode_RSA_public(key);
    } else {
        return DER_decode_RSA_private(key);
    }
}

string
CryptoManager::encrypt(PKCS * key, const string &s)
{
    string tocipher;
    tocipher.resize(RSA_size(key));

    RSA_public_encrypt((int) s.length(),
                       (const uint8_t*) s.data(), (uint8_t*) &tocipher[0],
                       key,
                       RSA_PKCS1_OAEP_PADDING);

    return tocipher;
}

string
CryptoManager::decrypt(PKCS * key, const string &s)
{
    assert_s(s.length() == (uint)RSA_size(key), "fromlen is not RSA_size");
    string toplain;
    toplain.resize(RSA_size(key));

    uint len =
        RSA_private_decrypt((int) s.length(),
                            (const uint8_t*) s.data(),
                            (uint8_t*) &toplain[0], key,
                            RSA_PKCS1_OAEP_PADDING);
    toplain.resize(len);

    return toplain;
}

void
CryptoManager::freeKey(PKCS * key)
{
    RSA_free(key);
}

//***************************************************************************************/

CryptoManager::~CryptoManager()
{
    if (masterKey)
        delete masterKey;

    for (auto it = OPEEncTable.begin(); it != OPEEncTable.end(); it++)
        delete it->second;
}

Binary
CryptoManager::encryptSWP(const Binary & key, const list<Binary> & words)
{
    auto l = SWP::encrypt(key, words);
    Binary r(*l);
    delete l;
    return r;
}

list<Binary> *
CryptoManager::decryptSWP(const Binary & key, const Binary & overall_ciph)
{
    auto l = overall_ciph.split(SWPCiphSize);
    auto r = SWP::decrypt(key, *l);
    delete l;
    return r;
}

Token
CryptoManager::token(const Binary & key, const Binary & word)
{
    return SWP::token(key, word);
}

bool
CryptoManager::searchExists(const Token & token, const Binary & overall_ciph)
{
    auto l = overall_ciph.split(SWPCiphSize);
    bool r = SWP::searchExists(token, *l);
    delete l;
    return r;
}

list<unsigned int> *
CryptoManager::searchSWP(const Token & token, const Binary & overall_ciph)
{
    return SWP::search(token, *(overall_ciph.split(SWPCiphSize)));
}

