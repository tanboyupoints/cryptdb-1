#pragma once
using namespace std;


namespace {

/**
 * Import database tool class. 
 */
class Import
{
    public:
        Import(const char *fname) : filename(fname){}
        ~Import(){}

        void executeQueries(Rewriter& r);
        void printOutOnly(void);

    private:
        string filename;
};
};

