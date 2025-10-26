#pragma once

#include <string>
#include <vector>

struct SidebarContent {
    std::vector<std::string> dynamicTitles;             // 支持多组动态标题
    std::vector<std::vector<std::string>> dynamicLines; // 支持多组动态行
    int currentLineGroupIndex = 0;                      // 当前显示的行组索引
    int lineGroupSwitchInterval = 5000;                 // 行组切换间隔，单位毫秒
};

struct Config {
    int  version        = 1;
    bool enablePlugin   = true;
    int  update_interval = 1000; // 默认更新间隔为1000毫秒
    SidebarContent sidebar; // 侧边栏配置
};
