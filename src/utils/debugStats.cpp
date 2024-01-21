#include "debugStats.h"
#include <iostream>

void DebugStats::printStats()
{
    std::cerr
        << "Debug Stats: "
        << "frameskips: " << this->frameSkips
    << std::endl;
}