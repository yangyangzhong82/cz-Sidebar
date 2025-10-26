#include "Sidebar/Sidebar.h"
#include "command.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/server/commands/PlayerCommandOrigin.h"
#include "mc/world/actor/player/Player.h"
#include <string>
#include <utility>
#include <vector>

#include "DatabaseManager.h" // 引入数据库管理器
#include "Config/ConfigManager.h" // 引入配置管理器

namespace Sidebar {

void RegisterSidebarCommand() {
    using ll::command::CommandHandle;
    using ll::command::CommandRegistrar;

    auto& registrar = CommandRegistrar::getInstance();
    auto& command   = registrar.getOrCreateCommand("sidebar", "控制侧边栏的显示", CommandPermissionLevel::Any);

    // Subcommand: /sidebar on
    command.overload<SidebarOn>().text("on").execute(
        [](CommandOrigin const& origin, CommandOutput& output, SidebarOn const&, ::Command const&) {
            auto* player = static_cast<Player*>(static_cast<PlayerCommandOrigin const&>(origin).getEntity());
            if (!player) {
                output.error("该指令只能由玩家执行。");
                return;
            }
            auto uuid = player->getUuid();
            if (DatabaseManager::getInstance().setPlayerSidebarStatus(uuid, true)) {
                output.success("侧边栏已开启。");
            } else {
                output.error("无法开启侧边栏。");
            }
        }
    );

    // Subcommand: /sidebar off
    command.overload<SidebarOff>().text("off").execute(
        [](CommandOrigin const& origin, CommandOutput& output, SidebarOff const&, ::Command const&) {
            auto* player = static_cast<Player*>(static_cast<PlayerCommandOrigin const&>(origin).getEntity());

            if (!player) {
                output.error("该指令只能由玩家执行。");
                return;
            }
            auto uuid = player->getUuid();
            if (DatabaseManager::getInstance().setPlayerSidebarStatus(uuid, false)) {
                output.success("侧边栏已关闭。");
            } else {
                output.error("无法关闭侧边栏。");
            }
        }
    );

    // Subcommand: /sidebar reload
    command.overload<SidebarReload>().text("reload").execute(
        [](CommandOrigin const& origin, CommandOutput& output, SidebarReload const&, ::Command const&) {
            // 只有管理员或控制台可以执行此命令
            if (origin.getPermissionsLevel() < CommandPermissionLevel::GameDirectors) {
                output.error("你没有权限执行此命令。");
                return;
            }

            if (ConfigManager::getInstance().reload()) {
                output.success("配置文件已重新加载。");
            } else {
                output.error("配置文件重新加载失败。");
            }
        }
    );
}

} // namespace Sidebar
