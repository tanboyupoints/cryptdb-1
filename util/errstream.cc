#include <sstream>

#include <main/enum_text.hh>
#include <main/rewrite_ds.hh>
#include <util/errstream.hh>
#include <util/onions.hh>

std::ostream &operator<<(std::ostream &out,
                         const AbstractCryptDBError &abstract_error)
{
    out << abstract_error.to_string();
    return out;
}

std::string AbstractCryptDBError::to_string() const
{
    return "FILE: " + file_name + "\n"
           "LINE: " + std::to_string(line_number) + "\n";
}

// FIXME: Format the output.
std::string BadItemArgumentCount::to_string() const
{
    return "Item has bad argument count\n" +
           AbstractCryptDBError::to_string() +
           // FIXME: Use TypeText.
           "ITEM TYPE: " + std::to_string(type) + "\n" +
           "EXPECTED COUNT: " + std::to_string(expected) + "\n" +
           "ACTUAL COUNT: " + std::to_string(actual) + "\n";
}

// FIXME: Format the output.
std::string UnexpectedSecurityLevel::to_string() const
{
    return "Unexpected security level for onion\n" +
           AbstractCryptDBError::to_string() +
           "ONION TYPE: " + TypeText<onion>::toText(o) + "\n" +
           "EXPECTED LEVEL: " + TypeText<SECLEVEL>::toText(expected)+"\n" +
           "ACTUAL LEVEL: " + TypeText<SECLEVEL>::toText(actual) + "\n";
}

// FIXME: Format the output.
// FIXME: Children EncSet would be nice.
std::string NoAvailableEncSet::to_string() const
{
    std::stringstream s;
    s <<
        "Current crypto schemes do not support this query" << std::endl
        << AbstractCryptDBError::to_string()
        // << "ITEM TYPE " + TypeText<>::toText();
        << "ITEM TYPE: " << std::to_string(type) << std::endl
        << "OPERATION: " << why << std::endl
        << "REQUIRED ENCSET: " << req_enc_set << std::endl
        << "***** CHILDREN REASONS *****" << std::endl;
    for (unsigned int i = 0; i < child_count; ++i) {
        s << "[" << std::to_string(i) << "] "
          << childr_rp[i]->getReason() << std::endl;
    }

    return s.str();
}
