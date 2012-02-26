#include <parser/Analysis.hh>

using namespace std;

string FieldMeta::stringify() {
    string res = " [FieldMeta " + fname + "]";
    return res;
}

string ItemMeta::stringify() {
    stringstream res;
    res << "(ItemMeta: onion " << o << " uptolevel " << (int)uptolevel << " basefield " << stringify_ptr(basefield) << ") ";
    return res.str();
}
