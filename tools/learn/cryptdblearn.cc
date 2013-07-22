/*
 * prototype
 */
#include <algorithm>
#include <cryptdblearn.hh>
#include <iostream>
#include <sstream>
#include <fstream>
#include <errstream.hh>
#include <getopt.h>
#include <assert.h>
#include <onions.hh> //layout
#include <Analysis.hh> //for intersect()
#include <rewrite_main.hh>

static bool 
ignore_line(const std::string& line)
{
    static const std::string begin_match("--");

    return(line.compare(0,2,begin_match) == 0); 
}


void
Learn::trainFromFile(void)
{
    std::string line;
    std::string s("");
    std::ifstream input(this->m_filename);

    assert(input.is_open() == true); 

    while(std::getline(input, line )){
        if(ignore_line(line))
            continue;

        if (!line.empty()){
            char lastChar = *line.rbegin();
            if(lastChar == ';'){
                s += line;

                /*
                 * Indeed rewrite is the cleaner solution 
                 * Note that refactory removing
                 * query execution from rewrite is ongoing 
                 * in CryptDB.
                 */
                QueryRewrite qr = this->m_r.rewrite(s);
                if(qr.queries.size() == 0)
                    this->m_errnum++;
                this->m_totalnum++;

                s.clear();
                continue;
            }
            s += line;
        }
    }
}
        
void 
Learn::trainFromScratch(void)
{
    //TODO: implement this
    /*
     *
     * OK, here we have no queries tracing file at all and we should 
     * train using as most secure onions layout as possible.
     */
}

int main(int argc, char **argv)
{
    int c, optind = 0;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"file", required_argument, 0, 'f'},
        {NULL, 0, 0, 0},
    };

    std::string username("");
    std::string password("");
    std::string dbname("");
    std::string filename("");

    while(1)
    {
        c = getopt_long(argc, argv, "hf:u:p:d:", long_options, &optind);
        if(c == -1)
            break;

        switch(c)
        {
            case 'h':
                break;
            case 'f':
                filename = optarg;
                break;
            case 'p':
                password = optarg;
                break;
            case 'u':
                username = optarg;
                break;
            case 'd':
                dbname = optarg;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    assert(username != "");
    assert(password != "");
    assert(dbname != "");
    
    ConnectionInfo ci("localhost", username, password);
    Rewriter r(ci, "/var/lib/shadow-mysql", dbname, true);

    // Onion layer keys are derived from master key.
    // here using the same as cdb_test.
    r.setMasterKey("2392834");

    Learn *learn; 
    
    if(filename != "")
    {
        learn = new Learn(MODE_FILE, r, dbname, filename);
        learn->trainFromFile();
    }else{
        learn = new Learn(MODE_FROM_SCRATCH, r, dbname, "");
        learn->trainFromScratch();
    }
   
    delete learn;

    return 0;
}
