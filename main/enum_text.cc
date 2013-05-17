#include "enum_text.h"

template<typename _type> EnumText<_type>* EnumText<_type>::instance = NULL;

template <typename _type> void
EnumText<_type>::addSet(std::vector<_type> enums,
                        std::vector<std::string> texts)
{
    if (enums.size() != texts.size()) {
        throw "enums and text must be the same length!";
    }
    EnumText<_type>::instance = new EnumText<_type>(enums, texts);

    return;
}

template <typename _type> std::vector<std::string>
EnumText<_type>::allText()
{
    return EnumText<_type>::instance->allText();
}

template <typename _type> std::vector<_type>
EnumText<_type>::allEnum()
{
    return EnumText<_type>::instance->allEnum();
}

template <typename _type> std::string
EnumText<_type>::toText(_type e)
{
    return EnumText<_type>::instance->getText(e);
}

template <typename _type> _type
EnumText<_type>::toEnum(std::string t)
{
    return EnumText<_type>::instance->getEnum(t);
}

// FIXME: I'm assuming getText or getEnum is broken.
template <typename _type>
std::string EnumText<_type>::getText(_type e)
{
   typename std::vector<_type>::iterator iter =
       std::find(theEnums.begin(), theEnums.end(), e);
   if (theEnums.end() == iter) {
       throw "enum does not exist!"; 
   }

   int pos = iter - theEnums.begin();
   return theTexts[pos];
}

template <typename _type>
_type EnumText<_type>::getEnum(std::string t)
{
    std::vector<std::string>::iterator iter =
        std::find(theTexts.begin(), theTexts.end(), t);
    if (theTexts.end() == iter) {
        throw "text does not exist!"; 
    }

    int pos = iter - theTexts.begin();
    return theEnums[pos];
}

