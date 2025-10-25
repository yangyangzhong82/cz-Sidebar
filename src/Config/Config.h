#pragma once

struct Config {
    int  version        = 1;
    bool enablePlugin   = true;
    int  update_interval = 1000; // 默认更新间隔为1000毫秒
};
