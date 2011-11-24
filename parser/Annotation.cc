#include "Annotation.hh"

using namespace std;

//removes character c from the end of s, if it's the last character
// if c isn't the last character, returns without doing anything
static void
end_prune(string &str, char c) {
    if (*str.rbegin() == c) {
        str.erase(str.length() - 1);
    }
}

//splits string s into a list of words deliminated by c
static list<string>
split(string &str, char c) {
    string s = str;
    list<string> result;
    size_t delim;
    while((delim = s.find(c)) != string::npos) {
        string word = s.substr(0,delim);
        result.push_back(word);
        s = s.substr(delim,string::npos);
    }
    return result;
}

Annotation::Annotation(const string &q) {
    query = q;
    parse();
}

PrincType
Annotation::getLeft() {
    if (type == SPEAKSFOR) {
        return left;
    }
    //LOG(error) << "Annotation asked for left when not SPEAKSFOR";
    PrincType empty;
    empty.princtype = "";
    empty.column = "";
    return empty;
}

PrincType
Annotation::getRight() {
    if (type == SPEAKSFOR || type == ENCFOR) {
        return right;
    }
    //LOG(error) << "Annotation asked for right when not SPEAKSFOR or ENCFOR";
    PrincType empty;
    empty.princtype = "";
    empty.column = "";
    return empty;
}

string
Annotation::getPrimitive() {
    if (type != SPEAKSFOR) {
        return primitive;
    }
    //LOG(error) << "Annotation asked for primitive when SPEAKSFOR";
    return "";
}

string
Annotation::getLeftTableName() {
    if (type != SPEAKSFOR) {
        //LOG(error) << "Annotation asked for left when not SPEAKSFOR";
        return "";
    }
    list<string> left_words = split(left.column, '.');
    assert_s(left_words.size() == 2, "table or column name has a . in it");
    return *left_words.begin();
}

string
Annotation::getRightTableName() {
    if (type != SPEAKSFOR && type != ENCFOR) {
        //LOG(error) << "Annotation asked for right when not SPEAKSFOR or ENCFOR";
        return "";
    }
    list<string> right_words = split(right.column, '.');
    assert_s(right_words.size() == 2, "table or column name has a . in it");
    return *right_words.begin();
}

string
Annotation::getPrimitiveTableName() {
    if (type != ENCFOR) {
        //LOG(error) << "Annotation asked for primitive when not ENCFOR";
        return "";
    }
    list<string> prim_words = split(primitive, '.');
    assert_s(prim_words.size() == 2, "table or column name has a . in it");
    return *prim_words.begin();
}

//XXX
//enclevel encoding stuff

Predicate *
Annotation::getPredicate() {
    if (type != SPEAKSFOR) {
        //LOG(error) << "Annotation asked for predicate when not SPEAKSFOR";
        return NULL;
    }
    return pred;
}

string
Annotation::getLeftStr() {
    if (type == SPEAKSFOR) {
        return left.princtype + "=" + left.column;
    }
    //LOG(error) << "Annotation asked for left when not SPEAKSFOR";
    return "";
}

string
Annotation::getRightStr() {
    if (type == SPEAKSFOR || ENCFOR) {
        return right.princtype + "=" + right.column;
    }
    //LOG(error) << "Annotation asked for left when not SPEAKSFOR";
    return "";
}    

void
Annotation::parse() {
    //remove trailing semicolon
    end_prune(query, ';');
    list<string> query_list = split(query, ' ');
    
    //type
    if (toLowerCase(query).find("princtype",0) != string::npos) {
        if (toLowerCase(query).find("external",0) != string::npos) {
            type = PRINCTYPE_EXTERNAL;
        } else {
            type = PRINCTYPE;
        }
    } else if (toLowerCase(query).find("encfor",0) != string::npos) {
        type = ENCFOR;
    } else if (toLowerCase(query).find("speaksfor",0) != string::npos) {
        type = SPEAKSFOR;
    } else {
        assert_s(false, "annotation is not PRINCTYPE, PRINCTYPE EXTERNAL, SPEAKSFOR, or ENCFOR");
    }

    auto word = query_list.begin();
    if (toLowerCase(*word) == "cryptdb") {
        word++;
    }

    //first word/pair
    switch (type) {
    case PRINCTYPE:
    case PRINCTYPE_EXTERNAL:
        assert_s(toLowerCase(*word) == "princtype", "PRINCTYPE annotation does not begin with PRINCTYPE");
        break;
    case ENCFOR:
        primitive = *word;
        break;
    case SPEAKSFOR:
        left.princtype = *word;
        word++;
        left.column = *word;
        break;
    }

    //middle word
    word++;
    switch(type) {
    case PRINCTYPE:
    case PRINCTYPE_EXTERNAL:
        primitive = *word;
        break;
    case ENCFOR:
        assert_s(toLowerCase(*word) == "encfor", "middle of ENCFOR annotation is not ENCFOR");
        break;
    case SPEAKSFOR:
        assert_s(toLowerCase(*word) == "speaksfor", "middle of SPEAKSFOR annotation is not SPEAKSFOR");
        break;
    }

    //last word/pair
    word++;
    switch(type) {
    case PRINCTYPE:
        assert_s(word == query_list.end(), "PRINCTYPE annotation has too many words");
        return;
    case PRINCTYPE_EXTERNAL:
        assert_s(toLowerCase(*word) == "external", "PRINCTYPE EXERNAL annotation does not have EXTERNAL after primitive");
    case ENCFOR:
    case SPEAKSFOR:
        right.princtype = *word;
        word++;
        right.column = *word;
    }

    //encryption level or predicate, if exists
    word++;
    switch(type) {
    case PRINCTYPE:
    case PRINCTYPE_EXTERNAL:
        //this should be the end
        assert_s(word == query_list.end(), "annotation has too many words");
        break;
    case ENCFOR:
        if (word == query_list.end()) {
            break;
        }
        if (equalsIgnoreCase(levelnames[(int) SECLEVEL::DET], *word)) {
            DETenclevel = SECLEVEL::DET;
            break;
        }
        if (equalsIgnoreCase(levelnames[(int) SECLEVEL::DETJOIN], *word)) {
            DETenclevel = SECLEVEL::DETJOIN;
            break;
        }
        if (equalsIgnoreCase(levelnames[(int) SECLEVEL::OPE], *word)) {
            DETenclevel = SECLEVEL::DET;
            OPEenclevel = SECLEVEL::OPE;
            break;
        }
        if (equalsIgnoreCase(levelnames[(int) SECLEVEL::SEMANTIC_AGG], *word)) {
            AGGenclevel = true;
            break;
        }
    case SPEAKSFOR:
        assert_s(toLowerCase(*word) == "if", "SPEAKSFOR annotation predicate does not start with if");
        list<string> pred_split = split(*word, '(');
        assert_s(pred_split.size() == 2, "predicate has too many (");
        pred->name = *pred_split.begin();
        end_prune(*pred_split.end(),')');
        list<string> pred_fields = split(*pred_split.end(),',');
        for(auto field = pred_fields.begin(); field != pred_fields.end(); field++) {
            pred->fields.push_back(*word);
        }
    }
}
