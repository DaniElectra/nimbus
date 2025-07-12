#pragma once

#include "../common.hpp"

Result ACT_GetAccountCount(u32 *out);
Result ACT_GetFriendLocalAccountId(u8 *out, u32 index);
Result ACT_GetPersistentId(u32 *out, u32 index);
Result ACT_GetAccountIndexOfFriendAccountId(u32 *index, u8 friend_account_id);
