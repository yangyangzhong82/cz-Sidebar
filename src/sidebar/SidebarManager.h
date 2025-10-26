#pragma once

#include <atomic>
#include <chrono> // 引入 chrono
#include <memory>
#include "ll/api/thread/ThreadPoolExecutor.h" // 引入 ThreadPoolExecutor

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

public: // 将 mThreadPool 声明为 public
    ll::thread::ThreadPoolExecutor mThreadPool; // 线程池成员
    std::chrono::steady_clock::time_point mLastLineGroupSwitchTime; 
};

} // namespace Sidebar
