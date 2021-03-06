#ifndef MOUS_MEDIAITEM_H
#define MOUS_MEDIAITEM_H

#include <util/MediaTag.h>

namespace mous {

struct MediaItem
{
    std::string url;
    int32_t duration = -1;

    // for wav/ape/flac/... with cue
    bool hasRange = false;
    uint64_t msBeg = 0;
    uint64_t msEnd = 0;

    MediaTag tag;

    void* userData = nullptr;

    template<typename buf_t> void operator>>(buf_t& buf) const
    {
        buf << url << duration << hasRange << msBeg << msEnd;
        tag >> buf;
    }

    template<typename buf_t> void operator<<(buf_t& buf)
    {
        buf >> url >> duration >> hasRange >> msBeg >> msEnd;
        tag << buf;
    }
};

}
#endif
