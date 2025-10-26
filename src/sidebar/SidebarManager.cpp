#include "SidebarManager.h"
#include "Config/ConfigManager.h"
#include "DatabaseManager.h" // 引入数据库管理器
#include "Entry/Entry.h"
#include "PA/PlaceholderAPI.h"
#include "Sidebar.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/thread/ServerThreadExecutor.h"
#include "logger.h"
#include "mc/network/NetworkPeer.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/level/Level.h"

namespace Sidebar {

using namespace ll::chrono_literals;

SidebarManager::SidebarManager()
: mRunning(false),
  mLastLineGroupSwitchTime(std::chrono::steady_clock::now()),
  mThreadPool("SidebarThreadPool", 2) {} // 初始化线程池，设置2个线程

SidebarManager::~SidebarManager() { stop(); }

SidebarManager& SidebarManager::getInstance() {
    static SidebarManager instance;
    return instance;
}

ll::coro::CoroTask<> updateSidebarTask(std::atomic<bool>& running) {
    while (running) {
        auto& config = ConfigManager::getInstance().get();
        if (!config.enablePlugin) {
            co_await std::chrono::milliseconds(config.update_interval);
            continue;
        }

        auto level = ll::service::getLevel();
        if (level) {
            level->forEachPlayer([&](Player& player) {
                if (player.isSimulated()) {
                    return true; // 不向模拟玩家设置侧边栏
                }
                auto uuid = player.getUuid();
                if (!DatabaseManager::getInstance().getPlayerSidebarStatus(uuid)) {

                } else {
                    // 捕获 player 的引用，以便在异步任务中使用
                    auto& playerRef = player;
                    SidebarManager::getInstance().mThreadPool.execute([&playerRef, &config]() {
                        // 获取侧边栏配置
                        auto& sidebarConfig = config.sidebar;

                        // 检查是否需要切换行组
                        auto now = std::chrono::steady_clock::now();
                        if (sidebarConfig.dynamicLines.size() > 1
                            && std::chrono::duration_cast<std::chrono::milliseconds>(
                                   now - SidebarManager::getInstance().mLastLineGroupSwitchTime
                               )
                                       .count()
                                   >= sidebarConfig.lineGroupSwitchInterval) {
                            sidebarConfig.currentLineGroupIndex =
                                (sidebarConfig.currentLineGroupIndex + 1) % sidebarConfig.dynamicLines.size();
                            SidebarManager::getInstance().mLastLineGroupSwitchTime = now;
                        }

                        // 获取当前显示的行组
                        const std::vector<std::string>* currentLines = nullptr;
                        if (!sidebarConfig.dynamicLines.empty()) {
                            currentLines = &sidebarConfig.dynamicLines[sidebarConfig.currentLineGroupIndex];
                        }

                        // 创建玩家上下文
                        PA::PlayerContext playerCtx;
                        playerCtx.player = &playerRef;

                        // 获取占位符服务
                        auto paService = PA::PA_GetPlaceholderService();

                        // 处理标题占位符
                        std::string processedTitle;
                        if (!sidebarConfig.dynamicTitles.empty()) {
                            processedTitle = paService->replace(
                                sidebarConfig.dynamicTitles[sidebarConfig.currentLineGroupIndex],
                                &playerCtx
                            );
                        } else {
                            processedTitle = "Sidebar"; // 默认标题
                        }

                        // 处理每行占位符
                        std::vector<std::pair<std::string, int>> sidebarData;
                        if (currentLines) {
                            int score = currentLines->size(); // 从行数开始递减作为分数
                            for (const auto& line : *currentLines) {
                                std::string processedLine = paService->replace(line, &playerCtx);
                                sidebarData.emplace_back(processedLine, score--);
                            }
                        }

                        // 将更新侧边栏的任务提交回主线程
                        ll::thread::ServerThreadExecutor::getDefault().execute(
                            [&playerRef, processedTitle, sidebarData]() {
                                Sidebar::setSidebar(
                                    &playerRef,
                                    processedTitle,
                                    sidebarData,
                                    ObjectiveSortOrder::Descending
                                );
                            }
                        );
                    });
                }
                return true;
            });
        }
        co_await std::chrono::milliseconds(config.update_interval);
    }
    co_return;
}

void SidebarManager::start() {
    mRunning = true;
    logger.info("Starting sidebar update task...");
    ll::coro::keepThis(updateSidebarTask, std::ref(mRunning)).launch(ll::thread::ServerThreadExecutor::getDefault());
}

void SidebarManager::stop() {
    if (mRunning) {
        mRunning = false;
        mThreadPool.destroy(); // 销毁线程池
    }
}

} // namespace Sidebar
