#include "sidebar.h"

#include "mc/network/packet/RemoveObjectivePacket.h"
#include "mc/network/packet/ScorePacketInfo.h"
#include "mc/network/packet/SetDisplayObjectivePacket.h"
#include "mc/network/packet/SetScorePacket.h"
#include "mc/server/ServerPlayer.h"
#include "mc/world/scores/IdentityDefinition.h"
#include "mc/network/packet/RemoveObjectivePacketPayload.h"
SetScorePacket::SetScorePacket()                                     = default;
SetDisplayObjectivePacket::SetDisplayObjectivePacket()               = default;
SetDisplayObjectivePacketPayload::SetDisplayObjectivePacketPayload() = default;


RemoveObjectivePacketPayload::RemoveObjectivePacketPayload() = default;
RemoveObjectivePacketPayload::RemoveObjectivePacketPayload(RemoveObjectivePacketPayload const&) = default;
RemoveObjectivePacket::RemoveObjectivePacket()               = default;

namespace Sidebar {

const std::string FAKE_OBJECTIVE_NAME = "cz-sidebar";

bool setSidebar(
    Player*                                         player,
    const std::string&                              title,
    const std::vector<std::pair<std::string, int>>& data,
    ObjectiveSortOrder                              sortOrder
) {
    if (!player) {
        return false;
    }

    // 移除旧的侧边栏以强制刷新
    removeSidebar(player);

    // 1. 创建并发送数据包，用于在侧边栏显示目标
    SetDisplayObjectivePacket displayPacket;
    displayPacket.mDisplaySlotName      = "sidebar";
    displayPacket.mObjectiveName        = FAKE_OBJECTIVE_NAME;
    displayPacket.mObjectiveDisplayName = title;
    displayPacket.mCriteriaName         = "dummy";
    displayPacket.mSortOrder            = sortOrder;
    player->sendNetworkPacket(displayPacket);

    // 2. 为每行数据创建 ScorePacketInfo
    std::vector<ScorePacketInfo> scoreInfos;
    for (const auto& line : data) {
        ScorePacketInfo info;
        info.mScoreboardId->mRawID = line.second; // 使用分数作为ID
        info.mObjectiveName        = FAKE_OBJECTIVE_NAME;
        info.mScoreValue           = line.second;
        info.mIdentityType         = IdentityDefinition::Type::FakePlayer;
        info.mFakePlayerName       = line.first;
        scoreInfos.emplace_back(info);
    }

    // 3. 创建并发送数据包以设置分数
    if (!scoreInfos.empty()) {
        SetScorePacket setScorePacket;
        setScorePacket.mType      = ScorePacketType::Change;
        setScorePacket.mScoreInfo = scoreInfos;
        player->sendNetworkPacket(setScorePacket);
    }

    return true;
}

bool removeSidebar(Player* player) {
    if (!player) {
        return false;
    }

    // 移除目标以清除侧边栏
    RemoveObjectivePacketPayload removePayload;
    removePayload.mObjectiveName = FAKE_OBJECTIVE_NAME;
    RemoveObjectivePacket removePacket(removePayload);
    player->sendNetworkPacket(removePacket);

    return true;
}

} // namespace Sidebar
