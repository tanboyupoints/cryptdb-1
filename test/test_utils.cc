/*
 * test_utils.cc
 * -- useful functions for testing
 *
 *
 */

#include <test/test_utils.hh>


using namespace std;

void
PrintRes(const ResType &res)
{
    for (auto i = res.names.begin(); i != res.names.end(); i++)
        cerr << *i << " | ";
    cerr << endl;
    for (auto outer = res.rows.begin(); outer != res.rows.end(); outer++) {
        for (auto inner = outer->begin(); inner != outer->end(); inner++)
            cerr << *inner << " | ";
        cerr << endl;
    }
    cerr << endl;
}

void 
displayLoading(bool mode)
{
    static const char *loading[] = {"|", "/", "-", "\\", "|", "/", "-", NULL};
    static unsigned int i = 0;
    static bool init = false;
    if(!init)
    {
        init=true;
        fprintf(stdout, "Wait.. ");
    }

    switch(mode)
    {
        case 1:
            loading[i] == NULL ? i = 0 : 0;
            fprintf(stdout, "%s\b", loading[i++]);
            fflush(stdout);
            break;

        case 0:
        default:
            i = 0;
            init = false;
            break;
    }
}

/*ResType
myExecute(EDBProxy * cl, string query)
{
    if (PLAIN)
        return cl->plain_execute(query);
    else
        return cl->execute(query);
        }*/

 /*ResType
myCreate(EDBProxy *cl, string annotated_query, string plain_query)
{
    if (PLAIN)
        return cl->plain_execute(plain_query);
    else
        return cl->execute(annotated_query);
}
 */

