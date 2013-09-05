#pragma once

#include <sstream>
#include <string>

#include <util/onions.hh>

class EncSet;
class RewritePlan;

class AbstractCryptDBError {
public:
    AbstractCryptDBError(const std::string &file_name,
                         unsigned int line_number)
        : file_name(file_name), line_number(line_number) {}
    virtual ~AbstractCryptDBError() {}

    virtual std::string to_string() const = 0;
    std::string getFileName() const {return file_name;}
    unsigned int lineNumber() const {return line_number;}

private:
    const std::string file_name;
    const unsigned int line_number;
};

std::ostream &operator<<(std::ostream &out,
                         const AbstractCryptDBError &abstract_error);

class BadItemArgumentCount : public AbstractCryptDBError {
public:
    BadItemArgumentCount(const std::string &file_name, int line_number,
                         unsigned int type, int expected, int actual)
        : AbstractCryptDBError(file_name, line_number), type(type),
          expected(expected), actual(actual) {}
    ~BadItemArgumentCount() {}

    // final requires g++ 4.7
    // std::string to_string() const final;
    std::string to_string() const;

private:
    // const Item::Type type;
    const unsigned int type;
    const int expected;
    const int actual;
};

class UnexpectedSecurityLevel : public AbstractCryptDBError {
public:
    UnexpectedSecurityLevel(const std::string &file_name, int line_number,
                            onion o, SECLEVEL expected, SECLEVEL actual)
        : AbstractCryptDBError(file_name, line_number), o(o),
          expected(expected), actual(actual) {}
    ~UnexpectedSecurityLevel() {}

    // std::string to_string() const final;
    std::string to_string() const;

private:
    const onion o;
    const SECLEVEL expected;
    const SECLEVEL actual;
};

class NoAvailableEncSet : public AbstractCryptDBError {
public:
    NoAvailableEncSet(const std::string &file_name, int line_number,
                      unsigned int type, const EncSet &req_enc_set,
                      const std::string &why,
                      const RewritePlan *const *const childr_rp,
                      unsigned int child_count)
        : AbstractCryptDBError(file_name, line_number), type(type),
          req_enc_set(req_enc_set), why(why), childr_rp(childr_rp),
          child_count(child_count) {}
    ~NoAvailableEncSet() {}

    // std::string to_string() const final;
    std::string to_string() const;

private:
    // const Item *const i;
    const unsigned int type;
    // FIXME: This reference is not safe.
    const EncSet &req_enc_set;
    const std::string why;
    const RewritePlan *const *const childr_rp;
    const unsigned int child_count;
};


