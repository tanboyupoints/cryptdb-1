#include <main/rewrite_util.hh>
#include <main/rewrite_main.hh>
#include <parser/lex_util.hh>

using namespace std;

extern CItemTypesDir itemTypes;

string
anonymize_table_name(const string &tname,
                     Analysis & a)
{
    TableMeta *tm = a.getTableMeta(tname);
    assert(tm);

    return tm->anonTableName;
}


void
optimize(Item **i, Analysis &a) {
   //TODO
/*Item *i0 = itemTypes.do_optimize(*i, a);
    if (i0 != *i) {
        // item i was optimized (replaced) by i0
        if (a.itemRewritePlans.find(*i) != a.itemRewritePlans.end()) {
            a.itemRewritePlans[i0] = a.itemRewritePlans[*i];
            a.itemRewritePlans.erase(*i);
        }
        *i = i0;
    } */
}


// this function should be called at the root of a tree of items
// that should be rewritten
// @context defaults to empty string.
Item *
rewrite(Item *i, const OLK & constr, Analysis &a, string context)
{
    if (context.size()) {
	context = " for " + context;
    }
    RewritePlan * rp = getAssert(a.rewritePlans, i);
    assert(rp);
    if (!rp->es_out.contains(constr)) {
	cerr << "query cannot be supported because " << i << " needs to return " << constr << context << "\n" \
	     << "BUT it can only return " << rp->es_out << " BECAUSE " << rp->r << "\n";
	assert(false);
    }
    return itemTypes.do_rewrite(i, constr, rp, a);
}

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, Analysis &a)
{
    TABLE_LIST * new_t = copy(t);

    // Table name can only be empty when grouping a nested join.
    assert(t->table_name || t->nested_join);
    if (t->table_name) {
        string anon_name = anonymize_table_name(string(t->table_name,
                                                       t->table_name_length), a);
        new_t->table_name = make_thd_string(anon_name, &new_t->table_name_length);
        new_t->alias = make_thd_string(anon_name);
        new_t->next_local = NULL;
    }

    return new_t;
}

// @if_exists: defaults to false.
SQL_I_List<TABLE_LIST>
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a,
                   bool if_exists)
{
    if (!tlist.elements) {
	return SQL_I_List<TABLE_LIST>();
    }

    TABLE_LIST * tl;
    if (if_exists && (false == a.tableMetaExists(tlist.first->table_name))) {
       tl = copy(tlist.first);
    } else {
       tl = rewrite_table_list(tlist.first, a);
    }

    SQL_I_List<TABLE_LIST> * new_tlist = oneElemList<TABLE_LIST>(tl);

    TABLE_LIST * prev = tl;
    for (TABLE_LIST *tbl = tlist.first->next_local;
	 tbl; tbl = tbl->next_local) {

        TABLE_LIST * new_tbl;
        if (if_exists && (false == a.tableMetaExists(tbl->table_name))) {
            new_tbl = copy(tbl);
        } else {
            new_tbl = rewrite_table_list(tbl, a);
        }

	prev->next_local = new_tbl;
	prev = new_tbl;
    }
    prev->next_local = NULL;

    return *new_tlist;
}

List<TABLE_LIST>
rewrite_table_list(List<TABLE_LIST> tll, Analysis & a)
{
    List<TABLE_LIST> * new_tll = new List<TABLE_LIST>();

    List_iterator<TABLE_LIST> join_it(tll);

    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t) {
	    break;
	}

        TABLE_LIST * new_t = rewrite_table_list(t, a);
	new_tll->push_back(new_t);

        if (t->nested_join) {
            new_t->nested_join->join_list = rewrite_table_list(t->nested_join->join_list, a);
            return *new_tll;
        }

        if (t->on_expr) {
            new_t->on_expr = rewrite(t->on_expr, PLAIN_OLK, a);
	}

	/* TODO: derived tables
        if (t->derived) {
            st_select_lex_unit *u = t->derived;
            rewrite_select_lex(u->first_select(), a);
        }
	*/
    }

    return *new_tll;
}

/*
 * Helper functions to look up via directory & invoke method.
 */
RewritePlan *
gather(Item *i, reason &tr, Analysis & a)
{
    return itemTypes.do_gather(i, tr, a);
}

//TODO: need to check somewhere that plain is returned
//TODO: Put in gather helpers file.
void
analyze(Item *i, Analysis & a)
{
    assert(i != NULL);
    LOG(cdb_v) << "calling gather for item " << *i;
    reason r;
    a.rewritePlans[i] = gather(i, r, a);
}

LEX *
begin_transaction_lex(Analysis a) {
    string query = "START TRANSACTION;";
    query_parse *begin_parse = new query_parse(a.ps->conn->getCurDBName(),
                                               query);
    return begin_parse->lex();
}

LEX *
commit_transaction_lex(Analysis a) {
    string query = "COMMIT;";
    query_parse *commit_parse = new query_parse(a.ps->conn->getCurDBName(),
                                                query);
    return commit_parse->lex();
}

