#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

template <typename type>
class EnumText {
public:
    EnumText(std::vector<type> enums, std::vector<std::string> texts);
    ~EnumText() {;}

    // Instance.
    std::string getText(type e);
    type getEnum(std::string t);

    // Static.
    static void addSet(std::vector<type> enums,
                       std::vector<std::string> text);
    static std::vector<std::string *> allText();
    static std::vector<type *> allEnum();
    static std::string toText(type e);
    static type toEnum(std::string t);

protected:
    // Instance.
    std::vector<std::string *> theTexts;
    std::vector<type *> theEnums;
    static EnumText *instance;
};

template<typename _type> EnumText<_type>* EnumText<_type>::instance = NULL;

