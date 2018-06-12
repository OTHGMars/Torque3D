//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "steamworksAPI.h"
#include "steamCallbacks.h"
#include "steamConfig.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
extern SteamAPI *gSteamAPI;

IMPLEMENT_GLOBAL_CALLBACK(onSteamGameOverlayActivated, void, (S32 isActive), (isActive),
   "Callback posted when the steam game overlay activates or deactivates.\n\n"
   "@param isActive The new state of the overlay. True- Active, False - Inactive.\n"
   "@ingroup Steam");

IMPLEMENT_GLOBAL_CALLBACK(onSteamUserStatsReceived, void, (bool statsLoaded), (statsLoaded),
   "Callback posted when the steam game overlay activates or deactivates.\n\n"
   "@param statsLoaded True if the stats have been loaded. False if there was an "
   "error and no stats are available.\n"
   "@ingroup Steam");

IMPLEMENT_GLOBAL_CALLBACK(onSteamUserStatsStored, void, (bool success), (success),
   "Callback posted to indicate the result of a StoreStats call.\n\n"
   "@param success True if the stats have been uploaded to steam servers. False if "
   "there was an error and no stats were sent.\n"
   "@ingroup Steam");

IMPLEMENT_GLOBAL_CALLBACK(onSteamAchievementStored, void,
(bool isUnlocked, const char* apiName, U32 curProgress, U32 maxProgress),
(isUnlocked, apiName, curProgress, maxProgress),
   "Callback posted when a steam achievement status change has been stored.\n\n"
   "@param isUnlocked True if the achievement is fully unlocked. If the achievement "
   "is unlocked, both curProgress and maxProgress will be 0.\n"
   "@param apiName The 'apiName' set for the achievement in the steam App Admin panel.\n"
   "@param curProgress Current progress towards the achievement.\n"
   "@param maxProgress The total amount of progress required to unlock.\n"
   "@ingroup Steam");

//------------------------------------------------------------------------------
SteamCallbacks::SteamCallbacks() :
   m_CallbackGameOverlayActivated(this, &SteamCallbacks::OnGameOverlayActivated),
   m_CallbackUserStatsReceived(this, &SteamCallbacks::OnUserStatsReceived),
   m_CallbackUserStatsStored(this, &SteamCallbacks::OnUserStatsStored),
   m_CallbackAchievementStored(this, &SteamCallbacks::OnAchievementStored)
{

}

void SteamCallbacks::OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
{
   onSteamGameOverlayActivated_callback(pCallback->m_bActive);
}

void SteamCallbacks::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
   if ((pCallback->m_nGameID == TORQUE_STEAMWORKS_APPID) && (pCallback->m_steamIDUser == SteamUser()->GetSteamID()))
   {
      if (gSteamAPI && (pCallback->m_eResult == k_EResultOK))
         gSteamAPI->initUserStats();

      onSteamUserStatsReceived_callback(pCallback->m_eResult == k_EResultOK);
   }
}

void SteamCallbacks::OnUserStatsStored(UserStatsStored_t* pCallback)
{
   if (pCallback->m_nGameID == TORQUE_STEAMWORKS_APPID)
   {
      onSteamUserStatsStored_callback(pCallback->m_eResult == k_EResultOK);
   }
}

void SteamCallbacks::OnAchievementStored(UserAchievementStored_t* pCallback)
{
   if (pCallback->m_nGameID == TORQUE_STEAMWORKS_APPID)
   {
      bool isUnlocked = (pCallback->m_nCurProgress == 0) && (pCallback->m_nMaxProgress == 0);
      onSteamAchievementStored_callback(isUnlocked, pCallback->m_rgchAchievementName,
         pCallback->m_nCurProgress, pCallback->m_nMaxProgress);
   }
}