#ifndef MOUS_PLUGINAGENT_H
#define MOUS_PLUGINAGENT_H

#include <string>
#include <dlfcn.h>
#include <mous/PluginHelper.h>
namespace mous {

template<typename PluginSuperClass>
class PluginAgent
{
    typedef const PluginInfo* (*FnGetPluginInfo)(void);
    typedef PluginSuperClass* (*FnCreatePlugin)(void);
    typedef void (*FnReleasePlugin)(PluginSuperClass*);

public:
    ErrorCode Open(const std::string& path)
    {
	m_pHandle = dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if (m_pHandle == NULL)
	    return MousPluginInvaild;

	m_fnGetInfo = (FnGetPluginInfo)dlsym(m_pHandle, StrGetPluginInfo);
	if (m_fnGetInfo == NULL)
	    return MousPluginInvaild;

	m_fnCreate = (FnCreatePlugin)dlsym(m_pHandle, StrCreatePlugin);
	if (m_fnCreate == NULL)
	    return MousPluginInvaild;

	m_fnRelease = (FnReleasePlugin)dlsym(m_pHandle, StrReleasePlugin);
	if (m_fnCreate == NULL)
	    return MousPluginInvaild;

	m_pPlugin = m_fnCreate();
	if (m_pPlugin == NULL)
	    return MousPluginInvaild;

	return MousOk;
    }

    void Close()
    {
	if (m_fnRelease != NULL)
	    m_fnRelease(m_pPlugin);

	if (m_pHandle != NULL)
	    dlclose(m_pHandle);
    }

    const PluginInfo* GetInfo()
    {
	return (m_fnGetInfo != NULL) ? m_fnGetInfo() : NULL;
    }

    const PluginSuperClass* GetPlugin()
    {
	return m_pPlugin;
    }

private:
    void* m_pHandle;

    FnGetPluginInfo m_fnGetInfo;
    FnCreatePlugin m_fnCreate;
    FnReleasePlugin m_fnRelease;

    PluginSuperClass* m_pPlugin;
};

}
#endif
