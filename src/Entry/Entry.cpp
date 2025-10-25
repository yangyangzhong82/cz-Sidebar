#include "Entry/Entry.h"

#include "ll/api/mod/RegisterHelper.h"
#include "Config/ConfigManager.h"
#include "DatabaseManager.h" 
#include "logger.h"          
namespace my_mod {

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

    // 初始化数据库
    auto& dbManager = Sidebar::DatabaseManager::getInstance();
    // 假设数据库文件位于插件的data目录下
    // TODO: 实际路径可能需要从配置或插件管理器获取
    std::string dbPath = "plugins/Sidebar/data/sidebar.db";
    if (!dbManager.openDatabase(dbPath)) {
        logger.error("无法初始化数据库，侧边栏功能可能无法正常工作。");
        return false;
    }
    if (!dbManager.createTable()) {
        logger.error("无法创建数据库表，侧边栏功能可能无法正常工作。");
        return false;
    }

    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");
    // Code for disabling the mod goes here.
    return true;
}

} // namespace my_mod

LL_REGISTER_MOD(my_mod::Entry, my_mod::Entry::getInstance());
