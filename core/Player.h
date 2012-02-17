#ifndef MOUS_PLAYER_H
#define MOUS_PLAYER_H

#include <inttypes.h>
#include <string>
#include <vector>
#include <map>
#include <mous/ErrorCode.h>
#include <scx/Thread.hpp>
#include <scx/SemVar.hpp>
#include <scx/PVBuffer.hpp>
#include <scx/Signal.hpp>
#include "AudioMode.h"

namespace mous {

enum PlayerStatus
{
    MousPlaying,
    MousStopped,
    MousPaused
};

class IDecoder;
class IRenderer;

class Player
{
public:
    Player();
    ~Player();

public:
    PlayerStatus GetStatus() const;

    void AddDecoder(IDecoder* pDecoder);
    void RemoveDecoder(IDecoder* pDecoder);
    void RemoveAllDecoders();
    void SpecifyDecoder(const string& suffix, IDecoder* pDecoder);

    void SetRenderer(IRenderer* pRenderer);
    void UnsetRenderer();

    ErrorCode Open(const string& path);
    void Close();

    void Play();
    void Play(uint64_t msBegin, uint64_t msEnd);
    void Pause();
    void Resume();
    void Stop();
    void Seek(uint64_t msPos);

    int32_t GetBitRate() const;
    int32_t GetSampleRate() const;
    uint64_t GetDuration() const;
    uint64_t GetCurrentMs() const;
    AudioMode GetAudioMode() const;

public:
    scx::Signal<void (void)> SigFinished;
    scx::Signal<void (void)> SigStopped;
    scx::Signal<void (void)> SigPaused;
    scx::Signal<void (void)> SigResumed;

private:
    inline void PlayRange(uint64_t beg, uint64_t end);
    inline void WorkForDecoder();
    inline void WorkForRenderer();

private:
    struct UnitBuffer
    {
	char* data;
	uint32_t used;
	uint32_t max;

	uint32_t unitCount;

	UnitBuffer(): 
	    data(NULL),
	    used(0),
	    max(0),
	    unitCount(0)
	{
	}

	~UnitBuffer()
	{
	    if (data != NULL)
		delete[] data;
	    data = NULL;
	    used = 0;
	    max = 0;
	    unitCount = 0;
	}
    };

private:
    PlayerStatus m_Status;

    bool m_StopDecoder;
    bool m_SuspendDecoder;
    IDecoder* m_pDecoder;
    scx::Thread m_ThreadForDecoder;
    scx::SemVar m_SemWakeDecoder;
    scx::SemVar m_SemDecoderSuspended;

    bool m_StopRenderer;
    bool m_SuspendRenderer;
    IRenderer* m_pRenderer;
    scx::Thread m_ThreadForRenderer;
    scx::SemVar m_SemWakeRenderer;
    scx::SemVar m_SemRendererSuspended;

    scx::PVBuffer<UnitBuffer> m_UnitBuffers;

    uint64_t m_UnitBeg;
    uint64_t m_UnitEnd;

    uint64_t m_DecoderIndex;
    uint64_t m_RendererIndex;

    double m_UnitPerMs;

    std::map<std::string, std::vector<IDecoder*>*> m_DecoderMap;
    typedef std::map<std::string, std::vector<IDecoder*>*>::iterator DecoderMapIter;
    typedef std::pair<std::string, std::vector<IDecoder*>*> DecoderMapPair;
};

}

#endif