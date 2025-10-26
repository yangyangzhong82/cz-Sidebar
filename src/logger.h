#include "Entry/Entry.h"
#include "ll/api/io/Logger.h"


inline ll::io::Logger& logger =
    Sidebar::Entry::getInstance().getSelf().getLogger(); // logger.Trace Debug Info Warn Error Fatal
