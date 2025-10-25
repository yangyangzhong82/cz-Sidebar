#include "SidebarManager.h"
#include "Entry/Entry.h"
#include "Sidebar.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "logger.h"
#include "mc/network/NetworkPeer.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/level/Level.h"
#include "Config/ConfigManager.h"

namespace Sidebar {

using namespace ll::chrono_literals;

SidebarManager::SidebarManager() : mRunning(false) {}

SidebarManager::~SidebarManager() { stop(); }

SidebarManager& SidebarManager::getInstance() {
    static SidebarManager instance;
    return instance;
}

ll::coro::CoroTask<> updateSidebarTask(std::atomic<bool>& running) {
    while (running) {
        auto& config = ConfigManager::getInstance().get();
        if (!config.enablePlugin) {
            co_await std::chrono::milliseconds(config.update_interval); // 使用新的配置项
            continue;
        }

        auto level = ll::service::getLevel();
        if (level) {
            level->forEachPlayer([&](Player& player) {

                return true;
            });
        }
        co_await std::chrono::milliseconds(config.update_interval); // 使用新的配置项
    }
    co_return;
}

void SidebarManager::start() {
        mRunning = true;
        logger.info("Starting sidebar update task...");
        ll::coro::keepThis(updateSidebarTask, std::ref(mRunning))
            .launch(ll::thread::ServerThreadExecutor::getDefault());
    
}

void SidebarManager::stop() {
    if (mRunning) {
        mRunning = false;
        // Potentially wait for the task to finish if needed
    }
}

} // namespace Sidebar
