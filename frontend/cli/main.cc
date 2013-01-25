#include <assert.h>
#include <unistd.h> // for usleep()
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include <util/MediaItem.h>
#include <util/PluginOption.h>
#include <core/IMediaLoader.h>
#include <core/IPluginManager.h>
#include <core/IConvTask.h>
#include <core/IConvTaskFactory.h>
using namespace mous;

#include <scx/FileInfo.hpp>
using namespace scx;

#include "cmd.h"

namespace Path {
    const char* const PluginRoot = "/lib/mous/";
};

mous_ctx ctx;

struct cmd_action_t
{
    const char* cmd;
    int (*action)(int, char**);
};

static cmd_action_t cmd_actions[] = {
    {   "play",   cmd_play      },
    {   "dec",    cmd_dec       },
    {   "plugin", cmd_plugin    },
    {   "help",   cmd_help      }
};

static const char* cli_name = NULL;

int cmd_help(int, char**)
{
    cout << "Usage: " << cli_name << " <command> <options> <files>\n"
            "play   (default command)\n"
            "       -r(repeat) -s(shuffle)\n"
            "cov    (format conversion)\n"
            "       m4a/mp3/flac/ogg -f(output)\n"
            "img    (dump coverart)\n"
            "       -f(output)\n"
            "info   (display file information)\n"
            "plugin (display plugin information)\n"
            "help   (display help information)\n";

    return 0;
}

int cmd_plugin(int, char**)
{
    vector<string> path_list;
    ctx.mgr->DumpPluginPath(path_list);
    for (size_t i = 0; i < path_list.size(); ++i) {
        cout << "(" << i+1 << ") ";
        cout << path_list[i] << endl;
        const PluginInfo* info = ctx.mgr->QueryPluginInfo(path_list[i]);
        cout << "   " << info->author << endl;
        cout << "   " << info->name << endl;
        cout << "   " << info->desc << endl;
        cout << "   " << info->version << endl;
    }
    cout << endl;

    cout << ">> Decoders:   " << ctx.dec_agents.size() << endl;
    cout << ">> Encoders:   " << ctx.enc_agents.size() << endl;
    cout << ">> Renderers:  " << ctx.red_agents.size() << endl;
    cout << ">> MediaPacks: " << ctx.pack_agents.size() << endl;
    cout << ">> TagParsers: " << ctx.tag_agents.size() << endl;
    cout << endl;

    return 0;
}

int main(int argc, char** argv)
{
    int ret = 0;
    cli_name = argc > 0 ? argv[0] : "mous-cli";

    if (argc < 2) {
        cmd_help(0, NULL);
        exit(-1);
    }

    // check plugin path then load it
    FileInfo dir_info(string(CMAKE_INSTALL_PREFIX) + Path::PluginRoot);
    const string pluginDir(dir_info.AbsFilePath());
    if (!dir_info.Exists() 
        || dir_info.Type() != FileType::Directory
        || pluginDir.empty()) {
        cout << "bad plugin directory!" << endl;
        exit(-1);
    }
    ctx.mgr = IPluginManager::Create();
    ctx.mgr->LoadPluginDir(pluginDir);

    // get plugin agents and check if we have enough
    ctx.mgr->DumpPluginAgent(ctx.dec_agents, PluginType::Decoder);
    ctx.mgr->DumpPluginAgent(ctx.enc_agents, PluginType::Encoder);
    ctx.mgr->DumpPluginAgent(ctx.red_agents, PluginType::Renderer);
    ctx.mgr->DumpPluginAgent(ctx.pack_agents, PluginType::MediaPack);
    ctx.mgr->DumpPluginAgent(ctx.tag_agents, PluginType::TagParser);
    if (ctx.dec_agents.empty() || ctx.red_agents.empty()) {
        cout << "need more plugins!" << endl;
        cmd_plugin(0, NULL);
        exit(-1);
    }

    // setup media loader
    ctx.loader = IMediaLoader::Create();
    ctx.loader->RegisterMediaPackPlugin(ctx.pack_agents);
    ctx.loader->RegisterTagParserPlugin(ctx.tag_agents);

    // match command
    {
        const size_t len = strlen(argv[1]);
        const int count = sizeof(cmd_actions)/sizeof(cmd_action_t);
        int index = 0;
        for (; index < count; ++index) {
            if (strncmp(cmd_actions[index].cmd, argv[1], len) == 0) {
                ret = cmd_actions[index].action(argc-2, argv+2);
                break;
            }
        }

        // "play" is the default cmd
        if (index == count) {
            ret = cmd_play(argc-1, argv+1);
        }
    }

    ctx.loader->UnregisterAll();
    IMediaLoader::Free(ctx.loader);

    ctx.mgr->UnloadAll();
    IPluginManager::Free(ctx.mgr);

    exit(ret);
}

/*
void PrintPluginOption(vector<PluginOption>& list)
{
    for (size_t i = 0; i < list.size(); ++i) {
        PluginOption& opt = list[i];
        cout << ">>>> index:" << i+1 << endl;
        cout << "\tplugin type: " << ToString(opt.pluginType)<< endl;
        for (size_t io = 0; io < opt.options.size(); ++io) {
            cout << "\t\t option type: " << ToString(opt.options[io]->type) << endl;
            cout << "\t\t option desc: " << opt.options[io]->desc << endl;
        }
    }
}
*/

    // test for encoder
    /*
    if (false)
    {
        IConvTaskFactory* factory = IConvTaskFactory::Create();
        factory->RegisterDecoderPlugin(ctx.enc_agents);
        factory->RegisterEncoderPlugin(ctx.dec_agents);

        vector<string> encoders = factory->EncoderNames();
        if (encoders.empty()) {
            cout << "No encoders!" << endl;
            IConvTaskFactory::Free(factory);
            return 0;
        }

        cout << ">> Available encoders:" << endl;
        for (size_t i = 0; i < encoders.size(); ++i) {
            cout << i+1 << ": " << encoders[i] << endl;
        }
        int index;
        cout << ">> Select encoder:";
        cin >> index;
        if (index < 1) {
            IConvTaskFactory::Free(factory);
            return 0;
        }

        MediaItem item = playlist[11];
        cout << item.url << endl;
        IConvTask* task = factory->CreateTask(item, encoders[index-1]);
        task->Run("output.wav");

        while (!task->IsFinished()) {
            double percent =  task->Progress();
            if (percent < 0) {
                cout << "failed!" << endl;
                break;
            }
            cout << "progress:" << (int)(percent*100) << "%" << "\r" << flush;
            usleep(1000);
        }
        cout << "quit!" << endl;

        IConvTask::Free(task);
        IConvTaskFactory::Free(factory);
        return 0;
    }
    */

    // Show player options 
    /*
    {
        vector<PluginOption> list;
        cout << ">> Player decoder plugin options:" << endl;
        player->DecoderPluginOption(list);
        PrintPluginOption(list);
        cout << ">> Player renderer plugin options:" << endl;
        PluginOption opt;
        player->RendererPluginOption(opt);
        list.resize(1);
        list[0] = opt;
        PrintPluginOption(list);
    }
    */


