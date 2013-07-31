#pragma once

#include <string>

#include <main/Connect.hh>

namespace MetaDataTables {
    namespace Name {
        std::string delta();
        std::string query();
        std::string dmlCompletion();
        std::string metaObject();
        std::string bleedingMetaObject();
    };

    void initialize(Connect *conn, Connect *e_conn);
};

