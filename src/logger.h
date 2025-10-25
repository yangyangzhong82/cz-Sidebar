#include "Entry/Entry.h"
#include "ll/api/io/Logger.h"


inline ll::io::Logger& logger = my_mod::Entry::getInstance().getSelf().getLogger(); // logger.Trace Debug Info Warn Error Fatal
