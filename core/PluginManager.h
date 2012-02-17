#ifndef MOUS_PLUGINMANAGER_H
#define MOUS_PLUGINMANAGER_H

#include <inttypes.h>
#include <map>
#include <vector>
#include <string>
#include <mous/ErrorCode.h>
#include <mous/PluginHelper.h>
struct stat;

namespace mous {

struct PluginInfo;
class IPluginAgent;
class IDecoder;
class IRenderer;
class IMediaUnpacker;

class PluginManager
{
public:
    PluginManager();
    ~PluginManager();

    ErrorCode LoadPluginDir(const std::string& dir);
    ErrorCode LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& path);
    void UnloadAllPlugins();

    void GetPluginPath(std::vector<std::string>& list);
    const PluginInfo* GetPluginInfo(const std::string& path);
    const PluginInfo* GetPluginInfo(const void* vp);

    void GetDecoders(std::vector<IDecoder*>& list);
    void GetRenderers(std::vector<IRenderer*>& list);
    void GetMediaUnpackers(std::vector<IMediaUnpacker*>& list);

    void* GetVpPlugin(const std::string& path, PluginType& type);

private:
    static std::vector<std::string>* pFtwFiles;
    static int OnFtw(const char* file, const struct stat* s, int);

private:
    template<typename Super>
    void GetPluginsByType(std::vector<Super*>& list, PluginType);

private:
    std::map<std::string, IPluginAgent*> m_PluginMap;
    typedef std::map<std::string, IPluginAgent*>::iterator PluginMapIter;
};

}

#endif