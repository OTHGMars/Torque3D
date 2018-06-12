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
extern SteamAPI *gSteamAPI;

void SteamAPI::initUserStats()
{
   mStatsLoaded = true;
}

bool SteamAPI::requestCurrentStats()
{
   if (!mIsSteamRunning)
      return false;

   // Is the user logged on?  If not we can't get stats
   if (!SteamUser()->BLoggedOn())
      return false;

   return SteamUserStats()->RequestCurrentStats();
}

bool SteamAPI::setAchievement(const char* apiName)
{
   if (!mIsSteamRunning || !mStatsLoaded || !apiName)
      return false;

   return SteamUserStats()->SetAchievement(apiName);
}

bool SteamAPI::clearAchievement(const char* apiName)
{
   if (!mIsSteamRunning || !mStatsLoaded || !apiName)
      return false;

   return SteamUserStats()->ClearAchievement(apiName);
}

bool SteamAPI::storeStats()
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return false;

   return SteamUserStats()->StoreStats();
}

U32 SteamAPI::getNumAchievements()
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return 0;

   return SteamUserStats()->GetNumAchievements();
}

const char* SteamAPI::getAchievementName(U32 idx)
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return NULL;

   return SteamUserStats()->GetAchievementName(idx);
}

bool SteamAPI::getAchievement(const char* apiName)
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return false;

   bool achieved;
   if (SteamUserStats()->GetAchievement(apiName, &achieved))
      return achieved;

   return false;
}

const char* SteamAPI::getAchievementDisplayAttribute(const char* apiName, const char* attributeKey)
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return NULL;

   return SteamUserStats()->GetAchievementDisplayAttribute(apiName, attributeKey);
}

bool SteamAPI::resetAllStats(bool achievementsToo)
{
   if (!mIsSteamRunning || !mStatsLoaded)
      return false;

   return SteamUserStats()->ResetAllStats(achievementsToo);
}

//-----------------------------------------------------------------------------
DefineEngineStaticMethod(SteamAPI, areStatsLoaded, bool, (), ,
   "@brief Returns true if a requestCurrentStats command has successfully completed for the "
   "current user.\n\n"
   "@ingroup Steamworks")
{
   return gSteamAPI && gSteamAPI->areStatsLoaded();
}


DefineEngineStaticMethod(SteamAPI, requestCurrentStats, bool, (), ,
   "Requests an asynchronous stats load for the currently logged in steam user.\n\n"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->requestCurrentStats();
}

DefineEngineStaticMethod(SteamAPI, setAchievement, bool, (const char* apiName), ,
   "Unlocks an achievement for the currently logged in steam user. This will save the "
   "stat change locally. Call SteamAPI::StoreStats() to upload to the server.\n\n"
   "@param apiName The 'apiName' for the achievement set in the steam App Admin panel.\n"
   "@return True if the achievement was unlocked. False if there was any error."
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#SetAchievement"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->setAchievement(apiName);
}

DefineEngineStaticMethod(SteamAPI, clearAchievement, bool, (const char* apiName), ,
   "Resets the unlock status of an achievement. This is primarily only ever used for "
   "testing. This will save the stat change locally. Call SteamAPI::StoreStats() to "
   "upload to the server.\n\n"
   "@param apiName The 'apiName' for the achievement set in the steam App Admin panel.\n"
   "@return True if the achievement was reset to locked. False if there was any error."
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#ClearAchievement"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->clearAchievement(apiName);
}

DefineEngineStaticMethod(SteamAPI, storeStats, bool, (), ,
   "Send the changed stats and achievements data to the server for permanent storage.\n\n"
   "@return True if data was sent. False if there was an error.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#StoreStats"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->storeStats();
}

DefineEngineStaticMethod(SteamAPI, getNumAchievements, S32, (), ,
   "Get the number of achievements defined in the App Admin panel of the Steamworks "
   "website. This is used for iterating through all of the achievements with GetAchievementName.\n\n"
   "@return The number of achievements or 0 if there was an error.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#GetNumAchievements"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return 0;

   return (S32) gSteamAPI->getNumAchievements();
}

DefineEngineStaticMethod(SteamAPI, getAchievementName, const char*, (U32 achievementIndex), ,
   "Gets the 'API name' for an achievement index between 0 and getNumAchievements.\n\n"
   "@return The achievement name or NULL if there was an error.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementName"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return NULL;

   return gSteamAPI->getAchievementName(achievementIndex);
}

DefineEngineStaticMethod(SteamAPI, getAchievement, bool, (const char* apiName), ,
   "Gets the unlock status of the achievement.\n\n"
   "@return True if the call was a success and the achievement is unlocked. False otherwise.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievement"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->getAchievement(apiName);
}

DefineEngineStaticMethod(SteamAPI, getAchievementDisplayAttribute, const char*, (const char* apiName, const char* attributeKey), ,
   "Get general attributes for an achievement. Currently provides: Name, Description, and Hidden status. "
   "This receives the value from a dictionary/map keyvalue store, so you must provide one of the following keys\n."
   "\"name\" to retrive the localized achievement name in UTF8.\n"
   "\"desc\" to retrive the localized achievement description in UTF8.\n"
   "\"hidden\" for retrieving if an achievement is hidden. Returns \"0\" when "
   "not hidden, \"1\" when hidden.\n\n"
   "@param apiName The 'apiName' for the achievement set in the steam App Admin panel.\n"
   "@param attributeKey The key text as described above.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#GetAchievementDisplayAttribute"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return NULL;

   return gSteamAPI->getAchievementDisplayAttribute(apiName, attributeKey);
}

DefineEngineStaticMethod(SteamAPI, resetAllStats, bool, (bool achievementsToo), ,
   "Resets the current users stats and, optionally achievements. This automatically "
   "calls StoreStats to persist the changes to the server. This should typically only "
   "be used for testing purposes during development. Ensure that you sync up your "
   "stats with the new default values provided by Steam after calling this by calling "
   "RequestCurrentStats.\n\n"
   "@param achievementsToo True if achievements should all be reset to locked. False "
   "if only stats should be reset.\n"
   "@return True if the stats were reset. False if there was any error."
   "@see https://partner.steamgames.com/doc/api/ISteamUserStats#ResetAllStats"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->resetAllStats(achievementsToo);
}

