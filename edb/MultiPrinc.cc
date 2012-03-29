/*
 * MultiPrincipal.cpp
 *
 */

#include <edb/MultiPrinc.hh>
//#include <util/ctr.hh>
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
    int accres;
    list<string> query_list;
    switch (annot.type) {
    case SINGLE_ENC:
        assert_s(false, "mp received SINGLE type annotation");
    case PRINCTYPE:
        accres = accMan->startPrinc(annot.getPrimitive());
        assert_s(accres >= 0, "access manager could not start principal " + annot.getPrimitive());
        encryptfield = false;
        break;
    case PRINCTYPE_EXTERNAL:
        accres = accMan->startPrinc(annot.getPrimitive());
        assert_s(accres >= 0, "access manager could not start principal " + annot.getPrimitive());
        accres = accMan->addGives(annot.getPrimitive());
        assert_s(accres >= 0, "access manager could not make principal external " + annot.getPrimitive());
        encryptfield = false;
        break;
    case SPEAKSFOR: {
        assert_s(setSensitive(schema, annot.getLeftTableName(), annot.getLeftFieldName()), "could not set left speaksfor table as sensitive");
        assert_s(setSensitive(schema, annot.getRightTableName(), annot.getRightFieldName()), "could not set right speaksfor table as sensitive");

        accres = accMan->addToPrinc(annot.getLeft().column, annot.getLeft().princtype);
        assert_s(accres >= 0, "access manager could not add to princ " + annot.getLeftStr());
        accres = accMan->addToPrinc(annot.getRight().column, annot.getRight().princtype);
        assert_s(accres >= 0, "access manager could not add to princ " + annot.getRightStr());

        accres = accMan->addSpeaksFor(annot.getLeft().princtype, annot.getRight().princtype);
        assert_s(accres >= 0, "access manager could not add " + annot.getLeftStr() + " speaks for " + annot.getRightStr());
        encryptfield = false;
        Predicate *pred = annot.getPredicate();
        if(pred) {
            mkm.condAccess[AccessRelation(annot.getLeft().column, annot.getRight().column)] = pred;
        }
        break;
    }
    case ENCFOR:
        accres = accMan->addToPrinc(annot.getRight().column, annot.getRight().princtype);
        assert_s(accres >= 0, "access manager could not add to princ " + annot.getRightStr());

        mkm.reverseEncFor[annot.getRight().column] = true;
        encryptfield = true;

        assert_s(setSensitive(schema, annot.getPrimitiveTableName(), annot.getPrimitiveFieldName()), "could not set primitive encfor table as sensitive");
        assert_s(setSensitive(schema, annot.getRightTableName(), annot.getRightFieldName()), "could not set right encfor table as sensitive");
        cerr << "sensitive done" << endl;
        FieldMeta *fm = schema->tableMetaMap[annot.getPrimitiveTableName()]->fieldMetaMap[annot.getPrimitiveFieldName()];
        assert_s(fm, "ENCFOR received primitive that does not exist; please put CREATE TABLE query before ENCFOR annotation\n");
        mkm.encForMap[fullName(fm->fname, annot.getPrimitiveTableName())] = annot.getRight().column;
        cerr << "process annotations mkm " << fullName(fm->fname, annot.getPrimitiveTableName()) << "->" << annot.getRight().column << endl;
        //if level not specified, it will be SECLEVEL::INVALID
        string query = "ALTER TABLE " + annot.getPrimitiveTableName();
        bool first = true;
        //there will always be a DET onion
        if (annot.getDETLevel() != SECLEVEL::INVALID) {
            if (fm->onions.find(oDET) == fm->onions.end()) {
                fm->onions[oDET].onionname = anonymizeFieldName(0, oDET, fm->fname, true);
            }
            if (annot.getDETLevel() != SECLEVEL::INVALID) {
                fm->setOnionLevel(oDET, annot.getDETLevel());
            }
            //mkm.encForMap[fullName(fm->onions[oDET], annot.getPrimitiveTableName())] = annot.getRight().column;
            //cerr << "process annotations mkm " << fullName(fm->onions[oDET], annot.getPrimitiveTableName()) << "->" << annot.getRight().column << endl;
            if (IsMySQLTypeNumeric(fm->sql_field->sql_type)) {
                query_list.push_back(query + " CHANGE " + fm->fname + " " + fm->onions[oDET].onionname + " " + TN_I64 + ";");
            } else {
                query_list.push_back(query + " CHANGE " + fm->fname + " " + fm->onions[oDET].onionname + " " + TN_TEXT + ";");
            }
            fm->setOnionLevel(oDET, annot.getDETLevel());
            first = false;
        }
        if (annot.getOPELevel() != SECLEVEL::INVALID) {
            if (fm->onions.find(oOPE) == fm->onions.end()) {
                fm->onions[oOPE].onionname = anonymizeFieldName(0, oOPE, fm->fname, true);
            }
            fm->setOnionLevel(oOPE, annot.getOPELevel());
            if (!first) {
                //mkm.encForMap[fullName(fm->onions[oOPE], annot.getPrimitiveTableName())] = annot.getRight().column;
                //cerr << "process annotations mkm " << fullName(fm->onions[oOPE], annot.getPrimitiveTableName()) << "->" << annot.getRight().column << endl;
                query_list.push_back(query + " ADD " + fm->onions[oOPE].onionname + " " + TN_I64 + ";");
            } else {
                query_list.push_back(query + " CHANGE " + fm->fname + " " + fm->onions[oOPE].onionname + " " + TN_I64 + ";");
            }
            first = false;
        }
        if (annot.getAGGLevel()) {
            if (fm->onions.find(oAGG) == fm->onions.end()) {
                fm->onions[oAGG].onionname = anonymizeFieldName(0, oAGG, fm->fname, true);
            }
            fm->setOnionLevel(oAGG, SECLEVEL::HOM);
            //mkm.encForMap[fullName(fm->onions[oAGG], annot.getPrimitiveTableName())] = annot.getRight().column;
            query_list.push_back(query + " ADD " + fm->onions[oAGG].onionname + " " + TN_HOM + ";");
        }
        if (annot.getSWPLevel()) {
            if (fm->onions.find(oSWP) == fm->onions.end()) {
                fm->onions[oSWP].onionname = anonymizeFieldName(0, oSWP, fm->fname, true);
            }
            fm->setOnionLevel(oSWP, SECLEVEL::SEARCH);
            //mkm.encForMap[fullName(fm->onion[oSWP], annot.getPrimitiveTableName())] = annot.getRight().column;
            query_list.push_back(query + " ADD " + fm->onions[oSWP].onionname + " " + TN_TEXT + ";");
        }
        break;
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

/*void
MultiPrinc::processAnnotation(list<string>::iterator & wordsIt,
                              list<string> & words, string tablename,
                              string currentField,
                              bool & encryptfield, map<string,
                                                       TableMetadata *> & tm)
{
    //cerr << "in process ann in mp\n";

    if (equalsIgnoreCase(*wordsIt, "encfor")) {
        tm[tablename]->hasSensitive = true;
        LOG(mp) << "encfor";
        wordsIt++;
        //cerr << "b\n";
        string field2 = *wordsIt;
        //cerr << "c\n";
        wordsIt++;
        //cerr << "before assign " << fullName(currentField, tablename) << " " << fullName(field2,
        //        tablename) << "\n";

        //cerr << "size of encfor map " << (mkm.encForMap.size()) << "\n";
        mkm.encForMap[fullName(currentField, tablename)] = fullName(field2,
                                                                    tablename);
        //cerr << "e\n";
        LOG(mp) << "==> "
                << fullName(currentField, tablename) << " "
                << fullName(field2, tablename);
        mkm.reverseEncFor[fullName(field2, tablename)] = true;
        encryptfield = true;

        FieldMetadata * fm = tm[tablename]->fieldMetaMap[currentField];

        // in multi-princ mode, these are false by default, unless explicitly requested
        fm->has_ope = false;
        fm->has_agg = false;

        //check if there is any annotation for security level
        std::set<string> secAnns =
            { levelnames[(int) SECLEVEL::DET],
              levelnames[(int) SECLEVEL::DETJOIN],
              levelnames[(int) SECLEVEL::OPE],
              levelnames[(int) SECLEVEL::SEMANTIC_AGG]
            };
        while ((wordsIt != words.end()) &&
               contains(*wordsIt, secAnns)) {


            if (equalsIgnoreCase(levelnames[(int) SECLEVEL::DET], *wordsIt)) {
                if (VERBOSE_G) { LOG(mp) << "at det"; }
                fm->secLevelDET = SECLEVEL::DET;
                wordsIt++;
                continue;
            }

            if (equalsIgnoreCase(levelnames[(int) SECLEVEL::DETJOIN], *wordsIt)) {
                fm->secLevelDET = SECLEVEL::DETJOIN;
                wordsIt++;
                continue;
            }

            if (equalsIgnoreCase(levelnames[(int) SECLEVEL::OPE], *wordsIt)) {
                if (VERBOSE_G) { LOG(mp) << "at det and opeself"; }
                fm->secLevelOPE = SECLEVEL::OPE;
                fm->secLevelDET = SECLEVEL::DET;
                fm->has_ope = true;
                wordsIt++;
                continue;
            }

            if (equalsIgnoreCase(levelnames[(int) SECLEVEL::SEMANTIC_AGG], *wordsIt)) {
                fm->agg_used = true;
                wordsIt++;
                continue;
            }
            assert_s(false, "invalid control path");
        }

        return;
    }

    if (equalsIgnoreCase(*wordsIt, "givespsswd")) {
        wordsIt++;
        string field2 = *wordsIt;
        wordsIt++;
        string curFieldPrincType = accMan->getPrincType(fullName(currentField, tablename));
        if (curFieldPrincType == "") {
            accMan->startPrinc("gen_" + fullName(currentField, tablename));
            accMan->addToPrinc(fullName(currentField, tablename), "gen_" + fullName(currentField, tablename));
            curFieldPrincType = accMan->getPrincType(fullName(currentField, tablename));
        }
        string field2PrincType = accMan->getPrincType(fullName(field2, tablename));
        if (field2PrincType == "") {
            accMan->startPrinc("gen_" + fullName(field2, tablename));
            accMan->addToPrinc(fullName(field2, tablename), "gen_" + fullName(field2, tablename));
            field2PrincType = accMan->getPrincType(fullName(field2, tablename));
        }
        int resacc = accMan->addGives(curFieldPrincType);
        assert_s(resacc >=0, "access manager gives psswd failed");
        resacc = accMan->addSpeaksFor(curFieldPrincType, field2PrincType);
        assert_s(resacc >=0, "access manager addaccessto failed");
        tm[tablename]->hasSensitive = true;
        encryptfield = false;
        return;
    }

    int countaccessto = 0;
    while (true) {
        if (equalsIgnoreCase(*wordsIt,"speaksfor")) {
            //if (equalsIgnoreCase(*wordsIt,"hasaccessto")) {
            tm[tablename]->hasSensitive = true;
            wordsIt++;
            if (countaccessto > 0) {
                assert_s(
                    false,
                    "multiple speaksfor annotations on same field, need to add this in insert relations");
            }
            countaccessto++;
            string field2 = *wordsIt;
            wordsIt++;
            string hasAccess = fullName(currentField, tablename);
            string accessto = fullName(field2, tablename);
            string genHasAccess;
            if ((genHasAccess = accMan->getPrincType(hasAccess)) == "") {
                genHasAccess = "gen_" + hasAccess;
                accMan->startPrinc(genHasAccess);
                accMan->addToPrinc(hasAccess,genHasAccess);
            }
            string genAccessTo;
            if ((genAccessTo = accMan->getPrincType(accessto)) == "") {
                genAccessTo = "gen_" + accMan->getPrincType(accessto);
                accMan->startPrinc(genAccessTo);
                accMan->addToPrinc(accessto,genAccessTo);
            }
            int resacc = accMan->addSpeaksFor(genHasAccess, genAccessTo);
            assert_s(resacc >=0, "access manager addSpeaksFor failed");
            encryptfield = false;
            if (equalsIgnoreCase(*wordsIt, "if")) {             //predicate
                LOG(mp) << "has predicate";
                wordsIt++;                 // go over "if"
                Predicate * pred = new Predicate();
                pred->name = *wordsIt;
                roll<string>(wordsIt, 2);                 //go over name and (
                while ((wordsIt != words.end()) && (wordsIt->compare(")"))) {
                    pred->fields.push_back(*wordsIt);
                    wordsIt++;
                    checkStr(wordsIt, words, ",", ")");
                }
                wordsIt++;
                mkm.condAccess[AccessRelation(hasAccess, accessto)] = pred;
                LOG(mp) << hasAccess << " speaks for " << accessto << " IF " << pred->name << "\n";
            }
            continue;
        }

        if (equalsIgnoreCase(*wordsIt, "equals")) {
            tm[tablename]->hasSensitive = true;
            wordsIt++;
            string field2 = *wordsIt;
            wordsIt++;
            string gen;
            //neither of these have a gen
            if (((gen = accMan->getPrincType(fullName(currentField,tablename))) == "") && ((gen = accMan->getPrincType(fullName(field2,tablename))) == "")) {
                gen = "gen_" + fullName(currentField,tablename);
                accMan->startPrinc(gen);
            }
            int resacc;
            resacc = accMan->addToPrinc(fullName(currentField,tablename), gen);
            assert_s(resacc >=0, "access manager addToPrinc failed");
            resacc = accMan->addToPrinc(fullName(field2, tablename), gen);
            encryptfield = false;
            continue;
        }
        return;
    }

    encryptfield = false;

}*/

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
    return mkm.reverseEncFor.find(field) != mkm.reverseEncFor.end();
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
        field_names = it->second->fieldNames;
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
        LOG(mp) << "-- key from accman is " <<
        CryptoManager::marshallKey(key) << "\n";
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
MultiPrinc::get_key(string fieldName, TMKM & tmkm, const vector<Item> &res)
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
        CryptoManager::marshallKey(key) << "\n";
        assert_s(key.length() > 0, "access manager does not have key\n");
        return key;
    }
    cerr << "not encForVal; use encForReturned size " << tmkm.encForReturned.size() << endl;
    if (tmkm.encForReturned.find(encForField) != tmkm.encForReturned.end()) {
        String val_str = res[tmkm.encForReturned[encForField]].str_value;
        string val = string(val_str.ptr(), val_str.length());
        string key = accMan->getKey(Prin(encForField, removeApostrophe(val)));
        LOG(mp) << "-- key from accman is " <<
        CryptoManager::marshallKey(key) << "\n";
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
