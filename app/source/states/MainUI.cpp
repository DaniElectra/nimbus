#include <format>
#include "MainUI.hpp"
#include "../sysmodules/acta.hpp"

constexpr Result ResultFPDLocalAccountNotExists = 0xC880C4ED; // FPD::LocalAccountNotExists

Result MainUI::unloadAccount(MainStruct *mainStruct) {
    Result rc = 0;

    handleResult(ACTA_UnloadConsoleAccount(), mainStruct, "Unload ACT account");
    if (R_FAILED(rc)) {
        return rc;
    }

    // In order to unload the friends account we need to make it go offline. Enter into exclusive state to disconnect from online
    handleResult(NDMU_EnterExclusiveState(NDM_EXCLUSIVE_STATE_LOCAL_COMMUNICATIONS), mainStruct, "Enter exclusive state");
    if (R_FAILED(rc)) {
        return rc;
    }

    bool online;
    // If the console was connected to online, wait until it disconnects.
    // I tried doing this through notification events but it didn't seem to work
    while (true) {
        handleResult(FRD_IsOnline(&online), mainStruct, "Online check");
        if (R_FAILED(rc)) {
            return rc;
        }

        if (!online) break;
    }

    // Now that the console is offline, unload the account
    handleResult(FRDA_UnloadLocalAccount(), mainStruct, "Unload friends account");

    return rc;
}

Result MainUI::switchAccounts(MainStruct *mainStruct, u8 friend_account_id) {
    Result rc = 0;

    handleResult(FRDA_LoadLocalAccount(friend_account_id), mainStruct, "Switch account");
    if (R_FAILED(rc)) {
        return rc;
    }

    u32 act_account_index = 0;
    handleResult(ACT_GetAccountIndexOfFriendAccountId(&act_account_index, friend_account_id), mainStruct, "Get ACT account ID of friend account ID");
    if (R_FAILED(rc)) {
        return rc;
    }

    if (act_account_index == 0) {
        u32 account_count;
        handleResult(ACT_GetAccountCount(&account_count), mainStruct, "Get account count");
        if (R_FAILED(rc)) {
            return rc;
        }

        handleResult(ACTA_CreateConsoleAccount(), mainStruct, "Create ACT account");
        if (R_FAILED(rc)) {
            return rc;
        }

        act_account_index = account_count + 1;

        handleResult(ACTA_CommitConsoleAccount(act_account_index), mainStruct, "Commit ACT account");
        if (R_FAILED(rc)) {
            return rc;
        }
    }

    handleResult(ACTA_SetDefaultAccount(act_account_index), mainStruct, "Set default account");

    return rc;
}

Result MainUI::createAccount(MainStruct *mainStruct, u8 friend_account_id, NascEnvironment environmentId) {
    Result rc = 0;

    // (Re)Create the friend account
    handleResult(FRDA_CreateLocalAccount(friend_account_id, static_cast<u8>(environmentId), 0, 1), mainStruct, "Create account");
    if (R_FAILED(rc)) {
        return rc;
    }

    // Switch to the friend/act accounts
    handleResult(switchAccounts(mainStruct, friend_account_id), mainStruct, "Switch account");
    if (R_FAILED(rc)) {
        return rc;
    }

    // Reset the act account
    handleResult(ACTA_UnbindServerAccount(friend_account_id, true), mainStruct, "Reset account");

    return rc;
}

void MainUI::migrateAccount(MainStruct *mainStruct) {
    Result rc = 0;
    u32 pretendo_account_index = 0;
    // Logs won't override any previous errors
    handleResult(ACT_GetAccountIndexOfFriendAccountId(&pretendo_account_index, 2), mainStruct, "Get PNID for migration");
    if (pretendo_account_index != 0) {
        bool is_commited = false;
        handleResult(ACT_GetAccountInfo(&is_commited, sizeof(bool), pretendo_account_index, INFO_TYPE_IS_COMMITTED), mainStruct, "Get PNID commit status");
        if (!is_commited) {
            handleResult(ACTA_CommitConsoleAccount(pretendo_account_index), mainStruct, "Commit PNID");
            LOG_NIMBUS_ERROR(mainStruct, "PNID has been migrated!");
        } else {
            LOG_NIMBUS_ERROR(mainStruct, "PNID is already migrated!");
        }
    } else {
        LOG_NIMBUS_ERROR(mainStruct, "There is no PNID on this system!");
    }
}

bool MainUI::drawUI(MainStruct *mainStruct, C3D_RenderTarget* top_screen, C3D_RenderTarget* bottom_screen, u32 kDown, u32 kHeld, touchPosition touch)
{
    // if start is pressed, exit to hbl/the home menu depending on if the app was launched from cia or 3dsx
    if (kDown & KEY_START) return true;

    C2D_SceneBegin(top_screen);
    DrawVersionString();
    C2D_DrawSprite(&mainStruct->top);

    if (mainStruct->errorString[0] != 0) {
        DrawString(0.5f, 0xFFFFFFFF, std::format("{}{}", mainStruct->errorString, mainStruct->needsReboot ? "\n\nPress START to reboot the system" : ""), 0);
    }

    C2D_SceneBegin(bottom_screen);
    DrawControls();

    if (mainStruct->buttonSelected == NascEnvironment::NASC_ENV_Prod) {
        if (mainStruct->currentAccount == NascEnvironment::NASC_ENV_Prod) {
            C2D_DrawSprite(&mainStruct->nintendo_loaded_selected);
            C2D_DrawSprite(&mainStruct->pretendo_unloaded_deselected);
        }
        else {
            C2D_DrawSprite(&mainStruct->nintendo_unloaded_selected);
            C2D_DrawSprite(&mainStruct->pretendo_loaded_deselected);
        }
    }
    else if (mainStruct->buttonSelected == NascEnvironment::NASC_ENV_Test) {
        if (mainStruct->currentAccount == NascEnvironment::NASC_ENV_Test) {
            C2D_DrawSprite(&mainStruct->nintendo_unloaded_deselected);
            C2D_DrawSprite(&mainStruct->pretendo_loaded_selected);
        }
        else {
            C2D_DrawSprite(&mainStruct->nintendo_loaded_deselected);
            C2D_DrawSprite(&mainStruct->pretendo_unloaded_selected);
        }
    }
    C2D_DrawSprite(&mainStruct->header);

    // Only allow user interaction when the system doesn't need a restart
    if (!mainStruct->needsReboot) {
        // handle touch input
        if (kDown & KEY_TOUCH) {
            if ((touch.px >= 165 && touch.px <= 165 + 104) && (touch.py >= 59 && touch.py <= 59 + 113)) {
                mainStruct->buttonSelected = NascEnvironment::NASC_ENV_Prod;
                mainStruct->buttonWasPressed = true;
            }
            else if ((touch.px >= 49 && touch.px <= 49 + 104) && (touch.py >= 59 && touch.py <= 59 + 113)) {
                mainStruct->buttonSelected = NascEnvironment::NASC_ENV_Test;
                mainStruct->buttonWasPressed = true;
            }
        }
        else if (kDown & KEY_LEFT || kDown & KEY_RIGHT) {
            mainStruct->buttonSelected = mainStruct->buttonSelected == NascEnvironment::NASC_ENV_Test ? NascEnvironment::NASC_ENV_Prod : NascEnvironment::NASC_ENV_Test;
        }

        if (kDown & KEY_A) {
            mainStruct->buttonWasPressed = true;
        }

        if (kDown & KEY_B) {
            migrateAccount(mainStruct);
            mainStruct->buttonWasPressed = false;
            return false;
        }
    }

    if (mainStruct->buttonWasPressed) {
        // Clear any previous logs
        mainStruct->errorString[0] = 0;

        // If the chosen account is the one we are already logged into, exit without rebooting
        if (mainStruct->currentAccount == mainStruct->buttonSelected) return true;

        u8 accountId = (u8)mainStruct->buttonSelected + 1; // by default set accountId to nasc environment + 1

        Result rc = unloadAccount(mainStruct);
        if (R_SUCCEEDED(rc)) {
            rc = switchAccounts(mainStruct, accountId);
            if (rc == ResultFPDLocalAccountNotExists && accountId == 2) {
                // Clear the error to allow createAccount to override it
                memset(mainStruct->errorString, 0, 256);
                rc = createAccount(mainStruct, accountId, NascEnvironment::NASC_ENV_Test);
            }
        }

        if (R_FAILED(rc)) {
            aptSetHomeAllowed(false);
            mainStruct->needsReboot = true;
            mainStruct->buttonWasPressed = false;
            return false;
        }

        mainStruct->needsReboot = true;

        return true;
    }

    return false;
}
