#pragma once

#include "../common.hpp"

Result frdAInit();
void frdAExit();
Handle *frdAGetSessionHandle();
Result FRDA_CreateLocalAccount(u8 localAccountId, NascEnvironment nascEnvironment, u8 serverTypeField1, u8 serverTypeField2);
Result FRDA_IsOnline(bool *state);
Result FRDA_Logout();
Result FRDA_GetMyLocalAccountId(u8 *localAccountId);
Result FRDA_LoadLocalAccount(u8 localAccountId);
Result FRDA_UnloadLocalAccount();
Result FRDA_SetClientSdkVersion(u32 sdkVer);
Result FRDA_GetServerTypes(u32 *out);
