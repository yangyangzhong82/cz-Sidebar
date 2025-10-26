#include "Entry/Entry.h"

#include "ll/api/mod/RegisterHelper.h"
#include "Config/ConfigManager.h"
#include "DatabaseManager.h" 
#include "logger.h"          
#include "sidebar/SidebarManager.h" // 引入 SidebarManager
#include "sidebar/command.h"
#include <filesystem> // 引入 filesystem 头文件
namespace Sidebar {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() {
    getSelf().getLogger().debug("Loading...");
    // Code for loading the mod goes here.
    auto configPath = getSelf().getConfigDir();
    if (!std::filesystem::exists(configPath)) {
        std::filesystem::create_directories(configPath);
    }
    configPath /= "config.json";
    configPath.make_preferred();

    if (!ConfigManager::getInstance().load(configPath.string())) {
        getSelf().getLogger().error("Failed to load config file!");
        return false;
    }
    return true;
}

bool Entry::enable() {
    getSelf().getLogger().debug("Enabling...");
    // Code for enabling the mod goes here.
    auto Path = getSelf().getDataDir();
    // 初始化数据库
    auto& dbManager = Sidebar::DatabaseManager::getInstance();
    if (!std::filesystem::exists(Path)) {
        std::filesystem::create_directories(Path);
    }
    std::string dbPath = (Path / "sidebar.db").string();
    if (!dbManager.openDatabase(dbPath)) {
        logger.error("无法初始化数据库，侧边栏功能可能无法正常工作。");
        return false;
    }
    if (!dbManager.createTable()) {
        logger.error("无法创建数据库表，侧边栏功能可能无法正常工作。");
        return false;
    }
    RegisterSidebarCommand();
        // 启动侧边栏管理器
        Sidebar::SidebarManager::getInstance()
            .start();

    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.

    // 停止侧边栏管理器
    Sidebar::SidebarManager::getInstance().stop();

    // 关闭数据库
    Sidebar::DatabaseManager::getInstance().closeDatabase();

    return true;
}


} // namespace Sidebar

LL_REGISTER_MOD(Sidebar::Entry, Sidebar::Entry::getInstance());
