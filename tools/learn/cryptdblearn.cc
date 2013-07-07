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
#include <Connect.hh>

static bool 
ignore_line(const string& line)
{
    static const string begin_match("--");

    return(line.compare(0,2,begin_match) == 0); 
}

void
Learn::trainFromFile(void)
{
    std::string line;
    string s("");
    ifstream input(this->m_filename);

    assert(input.is_open() == true); 

    while(std::getline(input, line )){
        if(ignore_line(line))
            continue;

        if (!line.empty()){
            char lastChar = *line.rbegin();
            if(lastChar == ';'){
                s += line;
                
                /*
                 * During rewrite EncLayers are computed
                 * and intersect is then invoked(). After that adjustOnion()
                 * may call removeOnionLayer() if needed. 
                 *
                 * The point is that we should have the tables present 
                 * beforehand otherwise how could we ajustOnions() if onions
                 * modify columns encryption schema?
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
     * OK, here we have no queries at all and we should 
     * train using as most secure onions layout as possible, 
     * but the question about tables to be already present 
     * remains.
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

    string username("");
    string password("");
    string filename("");

    while(1)
    {
        c = getopt_long(argc, argv, "hf:u:p:", long_options, &optind);
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
            case '?':
                break;
            default:
                break;
        }
    }

    assert(username != "");
    assert(password != "");
    
    ConnectionInfo ci("localhost", username, password);
    Rewriter r(ci, "/var/lib/shadow-mysql", "cryptdbtest", false, true);

    // Onion layer keys are derived from master key.
    // here using the same as cdb_test.
    r.setMasterKey("2392834");

    Learn *learn; 
    
    if(filename != "")
    {
        learn = new Learn(MODE_FILE, r, filename);
        learn->trainFromFile();
    }else{
        learn = new Learn(MODE_FILE, r, "");
        learn->trainFromScratch();
    }
   
    delete learn;

    return 0;
}
