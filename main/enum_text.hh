#pragma once

#include <vector>
#include <string>
#include <iostream>

// TODO: Use functor + find_if instead of for loops.
template <typename _type>
class TypeText {
public:
    TypeText(std::vector<_type> enums, std::vector<std::string> texts) {
        theEnums = new std::vector<_type>(enums);
        theTexts = new std::vector<std::string>(texts);
    }

    ~TypeText() {
        delete theEnums;
        delete theTexts;
    }

    // Instance.
    std::string getText(_type e) {
        for (unsigned int i = 0; i < theEnums->size(); ++i) {
            if ((*theEnums)[i] == e) {
                return (*theTexts)[i];
            }
        }

        throw "enum does not exist!";
    }

    _type getEnum(std::string t) {
        for (unsigned int i = 0; i < theTexts->size(); ++i) {
            if ((*theTexts)[i] == t) {
                return (*theEnums)[i];
            }
        }

        throw "text does not exist!";
    }

    // Static.
    static void addSet(std::vector<_type> enums,
                       std::vector<std::string> texts) {
        if (enums.size() != texts.size()) {
            throw "enums and text must be the same length!";
        }

        TypeText<_type>::instance = new TypeText<_type>(enums, texts);

        return;
    }

    static std::vector<std::string> allText() {
        return TypeText<_type>::instance->allText();
    }

    static std::vector<_type> allEnum() {
        return TypeText<_type>::instance->allEnum();
    }

    static std::string toText(_type e) {
        return TypeText<_type>::instance->getText(e);
    }

    // TODO(burrows): Add case insensitive lookup.
    static _type toType(std::string t) {
        return TypeText<_type>::instance->getEnum(t);
    }

    static std::string parenList() {
        std::vector<std::string> texts =
            *TypeText<_type>::instance->theTexts;
        std::stringstream s;
        s << "(";
        for (unsigned int i = 0; i < texts.size(); ++i) {
            s << "'" << texts[i] << "'";
            if (i != texts.size() - 1) {
                s << ", ";
            }
        }
        s << ")";

        return s.str();
    }

protected:
    // Instance.
    std::vector<std::string> *theTexts;
    std::vector<_type> *theEnums;
    static TypeText *instance;
};

template<typename _type> TypeText<_type>* TypeText<_type>::instance = NULL;

