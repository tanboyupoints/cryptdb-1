#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

template <typename type>
class TypeText {
public:
    TypeText(std::vector<type> enums, std::vector<std::string> texts);
    ~TypeText();

    // Instance.
    std::string getText(type e);
    type getEnum(std::string t);

    // Static.
    static void addSet(std::vector<type> enums,
                       std::vector<std::string> text);
    static std::vector<std::string> allText();
    static std::vector<type> allEnum();
    static std::string toText(type e);
    // TODO(burrows): Add case insensitive lookup.
    static type toType(std::string t);
    static std::string parenList();

protected:
    // Instance.
    std::vector<std::string> *theTexts;
    std::vector<type> *theEnums;
    static TypeText *instance;
};

template<typename _type> TypeText<_type>* TypeText<_type>::instance = NULL;

