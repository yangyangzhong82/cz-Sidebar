#pragma once

#include <atomic>
#include <memory>

namespace Sidebar {

class SidebarManager {
public:
    SidebarManager(const SidebarManager&)            = delete;
    SidebarManager& operator=(const SidebarManager&) = delete;

    static SidebarManager& getInstance();

    void start();
    void stop();

private:
    SidebarManager();
    ~SidebarManager();

    std::atomic<bool> mRunning;
};

} // namespace Sidebar
