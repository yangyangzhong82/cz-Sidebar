#pragma once


namespace Sidebar {

void RegisterSidebarCommand();

// Empty structs for command overloads
struct SidebarOn {};
struct SidebarOff {};
struct SidebarReload {};

} // namespace Sidebar
