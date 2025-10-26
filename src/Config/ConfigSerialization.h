#pragma once

#include "JsonMacros.h"
#include "config.h"
#include <nlohmann/json.hpp>


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SidebarContent,
    dynamicTitles,
    dynamicLines,
    currentLineGroupIndex,
    lineGroupSwitchInterval
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Config,
    version,
    enablePlugin,
    update_interval,
    sidebar
)
