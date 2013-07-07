#pragma once

#include <stdio.h>
#include <iostream>
#include <rewrite_main.hh>

using namespace std;

// Anonymous namespace
namespace {

typedef enum {
    MODE_FILE,
    MODE_FROM_SCRATCH,
    MODE_INVALID
}mode_e;

//TODO: dunno if I'll keep using this..
typedef struct _learn_mode_t
{
    mode_e mode;
    const char *envname;
} learn_mode_t;
static learn_mode_t __attribute__((unused))modes[]=
{
    {MODE_FILE, "TRUE"},
    {MODE_FROM_SCRATCH, "FALSE"},
    {MODE_INVALID, "FALSE"},
};

class Learn 
{
    public:

        Learn(mode_e mode, Rewriter& r, const string& filename) 
            : m_totalnum(0), m_errnum(0), 
            m_mode(mode), m_r(r), m_filename(filename)
        {
            //TODO: setenv is just a sugestion took from test, but need to be
            //better than this.
            //We must somehow inform CryptDB that we are in training mode for security
            //and performance matters.
            setenv("TRAIN_QUERY", modes[m_mode].envname, 1);
        }

        ~Learn(){
            unsetenv("TRAIN_QUERY");
            cout << "Executed queries: " << m_totalnum << endl;
            cout << "Errors: " << m_errnum << endl;
        }

        void trainFromFile(void);
        void trainFromScratch(void);


    private:
        int m_totalnum;
        int m_errnum;
        mode_e m_mode;
        Rewriter& m_r;
        string m_filename;

};

};
