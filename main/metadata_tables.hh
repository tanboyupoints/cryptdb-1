#pragma once

#include <string>
#include <memory>

#include <main/Connect.hh>

namespace MetaDataTables {
    namespace Name {
        std::string delta();
        std::string query();
        std::string dmlCompletion();
        std::string metaObject();
        std::string bleedingMetaObject();
        std::string transactionHelper();
    };

    bool initialize(const std::unique_ptr<Connect> &conn,
                    const std::unique_ptr<Connect> &e_conn);
};

