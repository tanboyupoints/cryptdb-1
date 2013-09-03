#pragma once

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <util/onions.hh>

class CryptDBError {
public:
    CryptDBError(const std::string &m) : msg(m) {}
    std::string msg;
};

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

class BadItemArgumentCount : public AbstractCryptDBError {
public:
    BadItemArgumentCount(const std::string &file_name, int line_number,
                         unsigned int type, int expected, int actual)
        : AbstractCryptDBError(file_name, line_number), type(type),
          expected(expected), actual(actual) {}
    ~BadItemArgumentCount() {}

    std::string to_string() const final;

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

    std::string to_string() const final;

private:
    const onion o;
    const SECLEVEL expected;
    const SECLEVEL actual;
};

std::ostream &operator<<(std::ostream &out,
                         const AbstractCryptDBError &abstract_error);

class fatal : public std::stringstream {
 public:
    ~fatal() __attribute__((noreturn)) {
        std::cerr << str() << std::endl;
        exit(-1);
    }
};

class cryptdb_err : public std::stringstream {
 public:
    ~cryptdb_err() /*throw (CryptDBError)*/ {
        std::cerr << str() << std::endl;
        throw CryptDBError(str());
    }
};

class thrower : public std::stringstream {
 public:
    ~thrower() __attribute__((noreturn)) {
        throw std::runtime_error(str());
    }
};

