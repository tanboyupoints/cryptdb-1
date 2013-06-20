#include <map>

typedef void (*QueryAnalyze)(LEX *lex, Analysis &a);

/*
(const std::string &q, LEX *lex,
                             Analysis &analysis, bool enc_by_default,
                             const string &cur_db);
                             */

typedef void (*UpdateMeta)(const string &q, LEX *lex, Analysis &a);
typedef LEX *(*LexRewrite)(LEX *lex, Analysis &analysis);

class SqlHandler {
    bool update_after_rewrite;
    static std::map<enum_sql_command, SqlHandler *> handlers;
    enum_sql_command sql_cmd;

public:
    QueryAnalyze query_analyze;
    UpdateMeta update_meta;
    LexRewrite lex_rewrite;

    SqlHandler(enum_sql_command cmd, QueryAnalyze query_analyze,
               UpdateMeta update_meta, LexRewrite lex_rewrite,
               bool update_after_rewrite=false) :
        update_after_rewrite(update_after_rewrite), sql_cmd(cmd),
        query_analyze(query_analyze), update_meta(update_meta),
        lex_rewrite(lex_rewrite) {;}
    bool updateAfter() {return update_after_rewrite;}
    enum_sql_command getSqlCmd() {return sql_cmd;}

    static SqlHandler *getHandler(enum_sql_command cmd);
    static bool addHandler(SqlHandler *handler);
};

std::map<enum_sql_command, SqlHandler *> SqlHandler::handlers;

