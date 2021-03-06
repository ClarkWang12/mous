#ifndef STATUSVIEW_H
#define STATUSVIEW_H

#include <mutex>

#include <util/MediaItem.h>
using namespace mous;

#include "IView.h"
#include "ClientPlayerHandler.h"

class StatusView: public IView
{
public:
    StatusView();
    ~StatusView();

    void Refresh();
    bool NeedRefresh() const;

    void MoveTo(int x, int y);
    void Resize(int w, int h);

    bool InjectKey(int key);

    void Show(bool show);
    bool IsShown() const;

public:
    int MinHeight() const;

    void SetPlayerHandler(ClientPlayerHandler* handler);

private:
    void SlotPause();
    void SlotSeek();
    void SlotVolume(int);
    void SlotPlayNext(bool);
    void SlotPlayMode(const std::string&);
    void SlotStatus(const ClientPlayerHandler::PlayerStatus&);

private:
    Window d;

    ClientPlayerHandler* m_PlayerHandler = nullptr;

    mutable std::mutex m_RefreshMutex;
    bool m_WaitReply = false;
    int m_NeedRefresh = 0;
    int m_Volume = 0;
    std::string m_PlayMode;
    ClientPlayerHandler::PlayerStatus m_PlayerStatus;
};

#endif
