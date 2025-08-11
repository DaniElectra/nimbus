#pragma once

#include <3ds.h>
#include "common.hpp"
#include "PatternManager.hpp"

namespace CTRPluginFramework
{
    enum NexBufferVersion {
        NOTINIT = -1,
        INVALID = -2,
        V0 = 0,
        V1 = 1
    };

    struct NexBufferv0 {
        u32 vtable;
        u32 unk;
        const u8* data;
        u32 size;
    };

    struct NexBufferv1 {
        u32 vtable;
        u32 refCount;
        u32 unk;
        const u8* data;
        u32 size;
    };

    void initHokakuCTR(PatternManager& pm);
    PatternStatus getHokakuCTRStatus();
    void enableHokakuCTR();
    void disableHokakuCTR();
    void finiHokakuCTR();
}
