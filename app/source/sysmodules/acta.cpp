#include "acta.hpp"

Result ACT_GetFriendLocalAccountId(u8 *out, u32 index) {
	return ACT_GetAccountInfo(out, sizeof(u32), index, INFO_TYPE_FP_LOCAL_ACCOUNT_ID);
}

Result ACT_GetPersistentId(u32 *out, u32 index) {
	return ACT_GetAccountInfo(out, sizeof(u32), index, INFO_TYPE_PERSISTENT_ID);
}

Result ACT_GetAccountCount(u32 *out) {
	return ACT_GetCommonInfo(out, sizeof(u32), INFO_TYPE_COMMON_NUM_ACCOUNTS);
}

Result ACT_GetAccountIndexOfFriendAccountId(u32 *index, u8 friend_account_id) {
	Result ret = 0;
	u32 account_count = 0;

	if (R_FAILED(ret = ACT_GetAccountCount(&account_count))) {
		return ret;
	}

	for (u32 account_index = 1; account_index < account_count; account_index++) {
		u8 found_friend_account_id = 0;

		if (R_FAILED(ret = ACT_GetFriendLocalAccountId(&found_friend_account_id, account_index))) {
			return ret;
		}

		if (friend_account_id == found_friend_account_id) {
			*index = account_index;
			return 0;
		}
	}

	// Found nothing
	*index = 0;
	return 0;
}
