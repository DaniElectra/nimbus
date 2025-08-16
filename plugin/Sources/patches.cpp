/*  Copyright (C) 2025 Pretendo Network contributors <pretendo.network>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include <CTRPluginFramework.hpp>
#include "common.hpp"
#include "patches.hpp"
#include "PatternManager.hpp"
#include "rt.h"

namespace CTRPluginFramework
{
    // Animal Crossing: New Leaf
    bool acnlPatches = false;
    RT_HOOK acnlPacket0x81HandlerHook = { 0 };
    u32 acnlPacket0x81HandlerAddr = 0;
    Mutex acnlPacket0x81HandlerMutex;
    std::vector<u32> acnlGameServerIDAddrs;
    Mutex acnlGameServerIDAddrMutex;

    bool checkACNLTitleID(u64 candidateTitleID) {
        return
            candidateTitleID == ACNL_TITLE_ID_USA ||
            candidateTitleID == ACNL_TITLE_ID_WA_USA ||
            candidateTitleID == ACNL_TITLE_ID_EUR ||
            candidateTitleID == ACNL_TITLE_ID_WA_EUR ||
            candidateTitleID == ACNL_TITLE_ID_JPN ||
            candidateTitleID == ACNL_TITLE_ID_WA_JPN ||
            candidateTitleID == ACNL_TITLE_ID_KOR ||
            candidateTitleID == ACNL_TITLE_ID_WA_KOR ||
            candidateTitleID == ACNL_TITLE_ID_WL_EUR;
    }

    void acnlPacket0x81Handler(u8 arg0, void* buffer, u32 size, u32 arg3, u32 arg4) {
        if (size > 0x1AC) {
            OSD::Notify("Detected attempt of ACNL exploit");
            size = 0x1AC;
        }

        return ((void(*)(u8, void*, u32, u32, u32))acnlPacket0x81HandlerHook.callCode)(arg0, buffer, size, arg3, arg4);
    }

    bool installACNLPacket0x81Handler(u32 addr) {
        Lock lock(acnlPacket0x81HandlerMutex);
        if (acnlPacket0x81HandlerAddr) return true;
        u32 funcStart = addr; // (u32)findNearestSTMFDptr((u32*)addr); -- The pattern already points to the beginning of the function
        if (!funcStart) return false;
        rtInitHook(&acnlPacket0x81HandlerHook, funcStart, (u32)acnlPacket0x81Handler);
        acnlPacket0x81HandlerAddr = funcStart;
        return true;
    }

    bool installACNLGameServerIDAddr(u32 addr) {
        Lock lock(acnlGameServerIDAddrMutex);
        acnlGameServerIDAddrs.push_back(addr);
        return false;
    }

    PatternStatus getACNLPatchesStatus() {
        if (!acnlPacket0x81HandlerAddr) {
            return PatternStatus::NotFound;
        }

        if (!acnlPacket0x81HandlerHook.isEnabled) {
            return PatternStatus::NotActive;
        }

        return PatternStatus::Active;
    }

    void initPatches(PatternManager& pm, u64 titleID) {
        acnlPatches = checkACNLTitleID(titleID);

        if (acnlPatches) {
            const u8 acnlPacket0x81HandlerPat[] = { 0xF0, 0x40, 0x2D, 0xE9, 0x6B, 0xDF, 0x4D, 0xE2 };
            pm.Add(acnlPacket0x81HandlerPat, sizeof(acnlPacket0x81HandlerPat), installACNLPacket0x81Handler);

            const u8 acnlGameServerIDPat[] = { 0x01, 0x62, 0x08, 0x00 };
            pm.Add(acnlGameServerIDPat, sizeof(acnlGameServerIDPat), installACNLGameServerIDAddr);
        }
    }

    void enablePatches() {
        if (acnlPacket0x81HandlerAddr) {
            rtEnableHook(&acnlPacket0x81HandlerHook);
            for (u32 addr : acnlGameServerIDAddrs) {
                *reinterpret_cast<u32*>(addr) = 0x00086202;
            }
        }
    }

    void disablePatches() {
        rtDisableHook(&acnlPacket0x81HandlerHook);
        for (u32 addr : acnlGameServerIDAddrs) {
            *reinterpret_cast<u32*>(addr) = 0x00086201;
        }
    }

    void finiPatches() {
        disablePatches();
        acnlGameServerIDAddrs.clear();
    }
}
