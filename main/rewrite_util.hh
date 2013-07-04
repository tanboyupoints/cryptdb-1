#pragma once

#include <string>

#include <main/Analysis.hh>

// anonymizes table name based on the information in a.schema
// TODO(burrows): Do we want to handle aliasing here, or up a level?
std::string
anonymize_table_name(const std::string &tname,
                     Analysis & a);
