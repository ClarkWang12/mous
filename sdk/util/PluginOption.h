#ifndef MOUS_PLUGINOPTION_H
#define MOUS_PLUGINOPTION_H

#include <vector>
#include <util/Option.h>
#include <util/PluginDef.h>

namespace mous {

struct PluginOption
{
    EmPluginType pluginType;
    const PluginInfo* pluginInfo;
    std::vector<const BaseOption*> options;
};

}

#endif
