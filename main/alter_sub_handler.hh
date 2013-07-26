#pragma once

#include <map>

#include <main/Analysis.hh>
#include <main/sql_handler.hh>

#include <sql_lex.h>

class AlterSubHandler : public SQLHandler {
public:
    virtual LEX *transformLex(Analysis &a, LEX *lex,
                              const ProxyState &ps) const;
    virtual ~AlterSubHandler() {;}

private:
    virtual LEX *rewriteAndUpdate(Analysis &a, LEX *lex,
                                  const ProxyState &ps) const = 0;

protected:
    AlterSubHandler() {;}
};

class AlterDispatcher;
AlterDispatcher *buildAlterSubDispatcher();
