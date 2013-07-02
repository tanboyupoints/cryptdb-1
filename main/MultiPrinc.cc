/*
 * MultiPrincipal.cpp
 *
 */

#include <main/MultiPrinc.hh>
#include <crypto/BasicCrypto.hh>
#include <util/cryptdb_log.hh>

using namespace std;

MultiPrinc::MultiPrinc(Connect * connarg)
{
    conn = connarg;
    accMan = new KeyAccess(conn);

    //cerr << "creating multi princ ";
    //cerr << "size of mkm.encForMap " << mkm.encForMap.size() << "\n";

}

MultiPrinc::~MultiPrinc()
{

    for (auto predIt = mkm.condAccess.begin(); predIt != mkm.condAccess.end(); predIt++) {
        assert_s(conn->execute("DROP FUNCTION " + predIt->second->name + ";"), "Failed to drop predicate");
    }

    delete accMan;

}

const bool VERBOSE = true;

list<string>
MultiPrinc::processAnnotation(Annotation &annot, bool &encryptfield,
                              SchemaInfo * schema) {
    list<string> query_list;

    switch (annot.type) {
        //MultiPrinc shouldn't get the single princ annotations
    case SINGLE_ENC:
        assert_s(false, "mp received SINGLE type annotation");


    case PRINCTYPE:
        assert_s(accMan->startPrinc(annot.getPrimitive()) >= 0, "access manager could not start principal " + annot.getPrimitive());
        encryptfield = false;
        break;


    case PRINCTYPE_EXTERNAL:
        assert_s(accMan->startPrinc(annot.getPrimitive()) >= 0, "access manager could not start principal " + annot.getPrimitive());
        assert_s(accMan->addGives(annot.getPrimitive()) >= 0, "access manager could not make principal external " + annot.getPrimitive());
        encryptfield = false;
        break;


    case SPEAKSFOR: {
        assert_s(setSensitive(schema, annot.getLeftTableName(), annot.getLeftFieldName()), "could not set left speaksfor table as sensitive");
        assert_s(setSensitive(schema, annot.getRightTableName(), annot.getRightFieldName()), "could not set right speaksfor table as sensitive");

        assert_s(accMan->addToPrinc(annot.getLeft().column, annot.getLeft().princtype) >= 0, "access manager could not add to princ " + annot.getLeftStr());
        assert_s(accMan->addToPrinc(annot.getRight().column, annot.getRight().princtype) >= 0, "access manager could not add to princ " + annot.getRightStr());

        assert_s(accMan->addSpeaksFor(annot.getLeft().princtype, annot.getRight().princtype) >= 0, "access manager could not add " + annot.getLeftStr() + " speaks for " + annot.getRightStr());
        encryptfield = false;
        
        Predicate *pred = annot.getPredicate();
        if(pred) {
            mkm.condAccess[AccessRelation(annot.getLeft().column, annot.getRight().column)] = pred;
        }
        break;
    }


    case ENCFOR:
        string right_col = annot.getRight().column;
        string right_type = annot.getRight().princtype;
        string right_table = annot.getRightTableName();
        string right_field = annot.getRightFieldName();
        string prim_table = annot.getPrimitiveTableName();
        string prim_field = annot.getPrimitiveFieldName();

        assert_s(accMan->addToPrinc(right_col, right_type) >= 0, "access manager could not add to princ " + annot.getRightStr());
        mkm.reverseEncFor[right_col] = true;
        encryptfield = true;
        assert_s(setSensitive(schema, prim_table, prim_field), "could not set primitive encfor table as sensitive");
        assert_s(setSensitive(schema, right_table, right_field), "could not set right encfor table as sensitive");

        FieldMeta *fm = schema->tableMetaMap[prim_table]->fieldMetaMap[prim_field];
        assert_s(fm, "ENCFOR received primitive that does not exist; please put CREATE TABLE query before ENCFOR annotation\n");
        mkm.encForMap[fullName(fm->fname, prim_table)] = right_col;

        string prev_onion = "";

        for (auto pr : fm->onions) {
            onion o = pr.first;
            SECLEVEL level = annot.hasOnion(o);
            cerr << o << " " << levelnames[(int) level] << endl;
            //if level not specified, it will be SECLEVEL::INVALID
            if (level == SECLEVEL::INVALID) {
                fm->removeOnion(o);
                continue;
            }

            if (level != SECLEVEL::RND) {
                assert_s(fm->setOnionLevel(o, level), "cannot set onion to requested level");
            }

            string onionname = fm->onions[o]->onionname;
            Create_field * cf = fm->onions[o]->layers.back()->newCreateField(onionname);

            stringstream query;
            query << "ALTER TABLE " + prim_table;
        
            if (prev_onion == "") {
                query << " CHANGE " << fm->fname << " " << *cf << ";";
            } else {
                query << " ADD " << *cf << " AFTER " << prev_onion << ";";
            }

            prev_onion = fm->onions[o]->onionname;
            query_list.push_back(query.str());
        }
     }

    return query_list;
}

bool
MultiPrinc::setSensitive(SchemaInfo *schema, string table_name, string field) {
    //check that table exists
    if (schema->tableMetaMap.find(table_name) == schema->tableMetaMap.end()) {
        LOG(warn) << "table " << table_name << " does not exist; please create it before this annotation";
        return false;
    }
    schema->tableMetaMap[table_name]->hasSensitive = true;
    return true;
}

int
MultiPrinc::commitAnnotations()
{
    return accMan->CreateTables();
}


typedef struct equalOp {
    string a, op, b;
} equalOp;


bool
MultiPrinc::hasEncFor(string field)
{
    if (mkm.reverseEncFor.find(field) != mkm.reverseEncFor.end()) {
        return mkm.reverseEncFor[field];
    }
    return false;
}


// fills tmkm.encForReturned and decides if the next field was added by us and
// should not be returned to the user
void
MultiPrinc::processReturnedField(unsigned int index, bool nextIsSalt, string fullname, onion o,
                                 TMKM &tmkm,
                                 bool & ignore)
{
    //ANON_REGION(__func__, &perf_cg);

    ignore = false;

    if (o != oINVALID) {
        //figure out where is the principal we want
        string princ = mkm.encForMap[fullname];
        if (tmkm.principalsSeen.find(princ) == tmkm.principalsSeen.end()) {
            //it must be value after because we inserted it (but skipping salt)
            if (nextIsSalt) {
                tmkm.encForReturned[princ] = index+2;
            } else {
                tmkm.encForReturned[princ] = index+1;
            }
            tmkm.principalsSeen[princ] = true;
            ignore = true;

        }
    }

    tmkm.returnBitMap[index] = true;
    if (nextIsSalt) {
        tmkm.returnBitMap[index+2] = !ignore;
    } else {
        tmkm.returnBitMap[index+1] = !ignore;

    }
}

//returns the name of the table if given an expression of the form
// $(PSSWD_TABLE_PREFIX)__TABLENAME,
// else returns ""
static string
getPsswdTable(string table)
{
    size_t prefix_len = PWD_TABLE_PREFIX.length();

    if (table.substr(0, prefix_len) == PWD_TABLE_PREFIX) {
        return table.substr(prefix_len, table.length()-prefix_len);
    }

    return "";
}

/*
 * checks for
 * INSERT INTO pwdcryptdb__TABLENAME (fieldname, psswd) VALUES (...)
 *   or
 * DELETE FROM pwdcryptdb__TABLENAME WHERE fieldname=psswd;
 */
bool
MultiPrinc::checkPsswd(LEX *lex) {
    //get tablename
    string table = lex->select_lex.table_list.first->table_name;
    string pw_table;

    //if not login, return false
    if ((pw_table = getPsswdTable(table)).length() == 0) {
        return false;
    }

    string type, password, uname;
    int resacc;
    if (lex->sql_command == SQLCOM_INSERT) {
        //fieldname for first (non-password) field
        assert_s(lex->field_list.head(), "login does not have fields named");
        auto it_f = List_iterator<Item>(lex->field_list);
        Item *i = it_f++;
        assert_s(i, "login does not have fields");
        assert(i->type() == Item::FIELD_ITEM);
        Item_field *ifd = static_cast<Item_field*>(i);
        type = fullName(ifd->field_name, pw_table);
        
        //values for fieldname and password
        assert_s(lex->many_values.head(), "login does not have values");
        auto it_l = List_iterator<List_item>(lex->many_values);
        List_item *li = it_l++;
        assert_s(li, "login does not have List_item");
        auto it = List_iterator<Item>(*li);
        //fieldname
        i = it++;
        assert_s(i, "login does not have fieldname value");
        String S_uname;
        String *S0_uname = i->val_str(&S_uname);
        uname = string(S0_uname->ptr(), S0_uname->length());
        //password
        i = it++;
        assert_s(i, "login does not have password value");
        String S_pass;
        String *S0_pass = i->val_str(&S_pass);
        password = string(S0_pass->ptr(), S0_pass->length());

        //insert into accMan
        password.resize(AES_KEY_BYTES);
        resacc = accMan->insertPsswd(Prin(type, uname), password);
        assert_s(resacc >= 0, "access manager insert password failed");
        return true;
    }

    if (lex->sql_command == SQLCOM_DELETE) {
        //HACK!!!
        //TODO (cat_red): make this less dependant on the form Rewriter turns out
        assert_s(lex->select_lex.where, "logout requires that a username be specified");
        stringstream ss;
        ss << *(lex->select_lex.where) << endl;
        string s = ss.str();
        list<string> q = split(s, ' ');
        auto it = q.begin();
        for (; it != q.end(); it++) {
            if (*it == "=") {
                break;
            }
        }
        assert_s(it != q.begin() && it != q.end(), "improperly formatted logout query");
        type = *(--it);
        start_prune(type, '(');
        prune(type, '`');
        type = fullName(type, pw_table);
        it++;
        assert_s(*it == "=", "logic fail in checkPsswd");
        uname = *(++it);
        end_prune(uname, '\n');
        end_prune(uname, ')');
        prune(uname, '\'');

        resacc = accMan->removePsswd(Prin(type, uname));
        assert_s(resacc >= 0, "access manager delete password failed");
        return true;
    }

    assert_s(false, "query should not have gone to mp->checkPsswd");
    return false;
}


/*bool
MultiPrinc::checkPsswd(command comm, list<string> & words)
{
    //ANON_REGION(__func__, &perf_cg);
    
    list<string>::iterator wordsIt = words.begin();
    string table;

    if (comm == cmd::INSERT) {
        roll<string>(wordsIt, 2);
        table = *wordsIt;
        string pw_table;
        if ((pw_table = getPsswdTable(table)).length() > 0) {
            wordsIt++;
            assert_s(wordsIt->compare(
                         "(") == 0,
                     "expected ( fields names list ) before VALUES ");
            roll<string>(wordsIt, 1);
            string type = fullName(*wordsIt, pw_table);
            roll<string>(wordsIt, 6);
            string uname = removeApostrophe(*wordsIt);
            wordsIt++;
            string p = removeApostrophe(*wordsIt);

             * XXX
             * we should hash this password!
            string passwd = p;
            passwd.resize(AES_KEY_BYTES);

            int resacc = accMan->insertPsswd(Prin(type, uname), passwd);
            assert_s(resacc >=0, "access manager insert psswd failed");
            return true;
        }

        return false;
    }

    if (comm == cmd::DELETE) {
        roll<string>(wordsIt, 2);         //now points to table name
        table = *wordsIt;
        string pw_table;
        if (VERBOSE_G) { LOG(mp) << "table in DELETE " << table <<"\n"; }
        if ((pw_table = getPsswdTable(table)).length() > 0) {
            roll<string>(wordsIt, 2);             // now points to givespsswd
                                                  // fieldname
            string type = fullName(*wordsIt, pw_table);
            roll<string>(wordsIt,2);             //now points to value
            string uname = *wordsIt;
            accMan->removePsswd(Prin(type, removeApostrophe(uname)));
            return true;
        }

        return false;
    }

    assert_s(false, "checkpasswd should be called only for insert and delete");
    return false;
}*/

bool
MultiPrinc::checkPredicate(const AccessRelation & accRel, map<string, string> & vals)
{
    //ANON_REGION(__func__, &perf_cg);
    if (mkm.condAccess.find(accRel) != mkm.condAccess.end()) {
        Predicate * pred = mkm.condAccess[accRel];

        //need to check correctness of this predicate
        string query = "SELECT " +  pred->name + "( ";
        for (list<string>::const_iterator it = pred->fields.begin();
             it != pred->fields.end(); it++) {
            query += " " + vals[*it] + ",";
        }
        query[query.length()-1]=' ';
        query += ");";
        DBResult * dbres;
        LOG(mp) << "Check predicate: " << query << "\n";
        assert_s(conn->execute(
                     query,
                     dbres), "failure while executing query " + query);
        ResType result = dbres->unpack();
        delete dbres;
        //TODO: not sure how to deal with new ResType
        return false;
        /*if (result.rows[0][0] == "1") {
            LOG(mp) << "pred OK\n";
            return true;
        } else {
            LOG(mp) << "pred NO\n";
            return false;
            }*/
    }

    //no predicate
    LOG(mp) << "no predicate to check";
    return true;
}

void
MultiPrinc::insertLex(LEX *lex, SchemaInfo * schema, TMKM &tmkm) {
    assert_s(lex->sql_command == SQLCOM_INSERT, "insertLex should only get insert commands");
    string table = lex->select_lex.table_list.first->table_name;
    if (!lex->many_values.head()) {
        cerr << "not values head in insertLex" << endl;
        return;
    }

    list<string> field_names;
    if (lex->field_list.head()) {
        auto it_fname = List_iterator<Item>(lex->field_list);
        Item *f;
        for (;;) {
            f = it_fname++;
            if(!f) {
                break;
            }
            assert(f->type() == Item::FIELD_ITEM);
            Item_field *ifd = static_cast<Item_field*>(f);
            field_names.push_back(ifd->field_name);
        }
    } else {
        auto it = schema->tableMetaMap.find(table);
        assert(it != schema->tableMetaMap.end());
        // FIXME: Clean.
        // field_names = it->second->fieldNames;
    }

    //construct map from fieldname to values
    map<string, string> vals;
    auto it_l = List_iterator<List_item>(lex->many_values);
    List_item *li = it_l++;
    assert_s(li, "insertLex does not have values");
    auto it_v = List_iterator<Item>(*li);
    Item *v;
    for (auto it_f = field_names.begin(); it_f != field_names.end(); it_f++) {
        v = it_v++;
        if(!v) {
            break;
        }
        String s;
        String *s_ptr = v->val_str(&s);
        vals[*it_f] = string(s_ptr->ptr(), s_ptr->length());
    }
    
    for(auto it = vals.begin(); it != vals.end(); it++) {        
        string speaks = fullName(it->first, table);
        //cerr << "speaks " << speaks << endl;
        //if current field has an ENC_FOR, note that it needs to be encrypted
        if (mkm.encForMap.find(speaks) != mkm.encForMap.end()) {
            string encForField = fullName(mkm.encForMap[speaks], table);
            tmkm.encForVal[encForField] = vals[getField(encForField)];
        }
        //examine cur_fields keychain, and add this value if necessary
        if (accMan->isType(speaks)) {
            std::set<string> speaks_for = accMan->getTypesHasAccessTo(speaks);
            for (auto accIt = speaks_for.begin(); accIt != speaks_for.end(); accIt++) {
                string spoken = *accIt; 
                //cerr << " spoken " << spoken << endl;
                if (getTable(spoken) != table) {
                    //this link in the chain is not in this table, so won't touch this insert
                    continue;
                }
                if (checkPredicate(AccessRelation(speaks, spoken), vals)) {
                    int resacc = accMan->insert(Prin(speaks, vals[it->first]),
                                                Prin(spoken, vals[getField(spoken)]));
                    assert_s(resacc >= 0, "access manager insert failed");
                }
            }
        }
    }
    vals.clear();
}

//wordsIt points to the first value
//bool is whether value is NULL or not
//TODO pass values as pointers, rather than copying them
void
MultiPrinc::insertRelations(const list<pair<string, bool> > & values, string table,
                            list<string> fields, TMKM &tmkm) 
{
    //ANON_REGION(__func__, &perf_cg);

    //first collect all values in a list
    map<string, string> vals;
    list<string>::iterator fieldIt = fields.begin();
    auto valIt = values.begin();

    // if (VERBOSE_G) { LOG(mp) << "fields are " << toString(fields, id_op); }
    // if (VERBOSE_G) { LOG(mp) << "values are " << toString(fields, id_op); }

    while (fieldIt != fields.end()) {
        vals[*fieldIt] = removeApostrophe(valIt->first);
        fieldIt++; valIt++;
    }

    assert_s(
        valIt == values.end(),
        "values and fields should have the same length");

    // We have a mapping, vals, of field name to values in this insert
    // we need to figure out which has access to values to insert
    // TODO: this is restricted to values in the same table only

    LOG(mp) << "in insert relations";
    for (fieldIt = fields.begin(); fieldIt != fields.end(); fieldIt++) {
        string fullField = fullName(*fieldIt, table);
        if (mkm.encForMap.find(fullField) != mkm.encForMap.end()) {
            string encForField = mkm.encForMap[fullField];
            tmkm.encForVal[encForField] = vals[getField(encForField)];
        }
        string hasaccess = fullField;

        if (accMan->isType(hasaccess)) {
            std::set<string> accessto_lst = accMan->getTypesHasAccessTo(
                hasaccess);
            LOG(mp) << hasaccess << " has access to  " <<
                             " <" << toString(accessto_lst, id_op);

            for (std::set<string>::iterator accIt = accessto_lst.begin();
                 accIt != accessto_lst.end(); accIt++) {
                string accessto = *accIt;
                if (getTable(accessto) != table) {
                    //this access to is not in this table and hence in this
                    // insert
                    continue;
                }
                LOG(mp) << "before predicate \n";
                //TODO: checkPredicate should also take accessto
                if (checkPredicate(AccessRelation(hasaccess, accessto), vals)) {
                    //need to insert
                   int resacc =
                        accMan->insert(Prin(hasaccess,
                                            vals[*fieldIt]),
                                       Prin(*accIt, vals[getField(accessto)]));
                    assert_s(resacc >=0, "access manager insert failed");
                    if (VERBOSE_G) { LOG(mp) << "after insert\n"; }
                }
            }
        }

    }

    vals.clear();
}

bool
MultiPrinc::isPrincipal(string princ) {
    return accMan->isType(princ);
}

/*bool
MultiPrinc::isActiveUsers(const string &query)
{
    list<string> words = getSQLWords(query);
    list<string>::iterator wordsIt = words.begin();
    string firstw = *wordsIt;
    string thirdw;
    if (words.size() > 2) {
        roll<string>(wordsIt, 2);
        thirdw = *wordsIt;
    } else {
        thirdw = "";
    }
    if ((equalsIgnoreCase(firstw,
                          "INSERT") ||
         equalsIgnoreCase(firstw,
                          "DELETE")) &&
        equalsIgnoreCase(psswdtable, thirdw)) {
        return true;
    }
    return false;

}*/

string
MultiPrinc::get_key(string fieldName, TempMKM & tmkm)
{
    //ANON_REGION(__func__, &perf_cg);

    assert_s(mkm.encForMap.find(
                 fieldName) != mkm.encForMap.end(),
             "cryptappgetkey gets unencrypted field <"+fieldName+">");
    string encForField = mkm.encForMap[fieldName];
    cerr << "looking for encForField " << encForField << " in size " << tmkm.encForVal.size() << endl;
    if (tmkm.encForVal.find(encForField) != tmkm.encForVal.end()) {
        if (VERBOSE_G) {LOG(mp) << "asking get key for " << encForField <<
                " <" << tmkm.encForVal[encForField] << "> \n"; }
        string key =
            accMan->getKey(Prin(encForField,
                                removeApostrophe(tmkm.encForVal[encForField])));
        LOG(mp) << "-- key from accman is " << marshallKey(key) << "\n";
        assert_s(
            key.length() > 0, "access manager does not have needed key!!");
        return key;
    }

    assert_s(
        false,
        "cryptdb does not have keys to encrypt query for requested users \n");
    return NULL;
}

string
MultiPrinc::get_key(string fieldName, TMKM & tmkm, const vector<Item *> &res)
{
    //ANON_REGION(__func__, &perf_cg);

    assert_s(mkm.encForMap.find(
                 fieldName) != mkm.encForMap.end(),
             "cryptappgetkey gets unencrypted field <"+fieldName+">");
    string encForField = mkm.encForMap[fieldName];
    cerr << "looking for encForField " << encForField << " in size " << tmkm.encForVal.size() << endl;
    if (tmkm.encForVal.find(encForField) != tmkm.encForVal.end()) {
        string key =
            accMan->getKey(Prin(encForField,
                                removeApostrophe(tmkm.encForVal[encForField])));
        if (VERBOSE_G) {LOG(mp) << "using encforval; encForField " <<
                        encForField << " val " <<
                        tmkm.encForVal[encForField] <<
                        " encforreturned index " <<
                        tmkm.encForReturned[encForField] <<
                        "\n"; }
        LOG(mp) << "-- key from accman is " <<
	    marshallKey(key) << "\n";
        assert_s(key.length() > 0, "access manager does not have key\n");
        return key;
    }
    cerr << "not encForVal; use encForReturned size " << tmkm.encForReturned.size() << endl;
    if (tmkm.encForReturned.find(encForField) != tmkm.encForReturned.end()) {
        string val = ItemToString(res[tmkm.encForReturned[encForField]]);
        string key = accMan->getKey(Prin(encForField, removeApostrophe(val)));
        LOG(mp) << "-- key from accman is " <<
	    marshallKey(key) << "\n";
        if (VERBOSE_G) {LOG(mp) << "using encforreturned: get key " <<
                        encForField << " val " << val <<
                        " encforreturned index " <<
                        tmkm.encForReturned[encForField] << "\n"; }
        assert_s(
            key.length() > 0, "access manager does not have needed key\n");
        return key;
    }

    assert_s(false, "cryptdb does not have keys to decrypt query result \n");
    return NULL;
}
