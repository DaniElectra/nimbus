#pragma once

#include <string>
#include "PatternManager.hpp"
#include "common.hpp"

namespace CTRPluginFramework
{
    static const std::string PiaNatMapping[] = {
        "Unknown", "Endpoint independent mapping", "Endpoint dependent mapping"
    };

    static const std::string PiaNatFiltering[] = {
        "Unknown", "Port independent filtering", "Port dependent filtering"
    };

    bool onPiaPlayersFolderAction(MenuFolder& folder, ActionType type);

    void initPiaLogger(PatternManager& pm);
    PatternStatus getPiaLoggerStatus();
}
