#pragma once

#include "mc/world/actor/player/Player.h"
#include <string>
#include <vector>

struct sqlite3;

namespace Sidebar {

class DatabaseManager {
public:
    static DatabaseManager& getInstance();
    bool openDatabase(const std::string& dbPath);
    void closeDatabase();
    bool createTable();
    bool setPlayerSidebarStatus(const mce::UUID& playerUuid, bool enabled);
    bool getPlayerSidebarStatus(const mce::UUID& playerUuid);

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    sqlite3* db;
    std::string dbFilePath;
};

} // namespace Sidebar
