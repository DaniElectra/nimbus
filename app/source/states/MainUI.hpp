#pragma once

#include "../common.hpp"

namespace MainUI
{
    Result switchAccounts(MainStruct* mainStruct, u8 friend_account_id);
    Result createAccount(MainStruct* mainStruct, u8 friend_account_id, NascEnvironment environmentId);
    bool drawUI(MainStruct *mainStruct, C3D_RenderTarget* top_screen, C3D_RenderTarget* bottom_screen, u32 kDown, u32 kHeld, touchPosition touch);
}
