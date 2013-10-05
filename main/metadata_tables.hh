#pragma once

#include <string>
#include <memory>

#include <main/Connect.hh>

namespace MetaDataTables {
    bool initialize(const std::unique_ptr<Connect> &conn,
                    const std::unique_ptr<Connect> &e_conn,
                    const std::string &prefix);

    namespace Name {
        std::string delta();
        std::string query();
        std::string dmlCompletion();
        std::string metaObject();
        std::string bleedingMetaObject();

        std::string purgatoryDB();
    };

    namespace Internal {
        const std::string &embeddedDB();
        const std::string &remoteDB();
        void initPrefix(const std::string &prefix);
        const std::string &getPrefix();
        const std::string &lowLevelPrefix(const char *const p);
    };
};

