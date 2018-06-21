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

#ifndef _STEAMWORKS_H_
#define _STEAMWORKS_H_
#include "steam_gameserver.h"

#if defined( WIN32 ) && defined( NDEBUG )
#include "platformWin32/platformWin32.h"
#endif

#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif
#ifndef _GAMEPROCESS_H_
   #include "T3D/gameBase/gameProcess.h"
#endif

class SteamCallbacks;
class SteamAPI
{
private:
   SteamCallbacks* mCallbacks;
   bool mIsSteamRunning;
   ProcessList *mProcessList;
   U32 mTics;

   bool mStatsLoaded;

public:
   DECLARE_STATIC_CLASS( SteamAPI );

   SteamAPI();
   virtual ~SteamAPI();

   bool init();
   void shutdown();
   void process();

   bool isSteamRunning() { return mIsSteamRunning; }
   bool areStatsLoaded() { return mStatsLoaded; }
   void initUserStats();

   // ISteamUser interface
   U32 getUserId();

   // ISteamFriends interface
   const char* getPersonaName();

   // ISteamUserStats interface
   bool requestCurrentStats();
   bool setAchievement(const char* apiName);
   bool clearAchievement(const char* apiName);
   bool storeStats();
   U32 getNumAchievements();
   const char* getAchievementName(U32 idx);
   bool getAchievement(const char* apiName);
   const char* getAchievementDisplayAttribute(const char* apiName, const char* attributeKey);
   bool resetAllStats(bool achievementsToo);

   // ISteamUtil interface
   bool isOverlayEnabled();
   bool isSteamInBigPictureMode();
   bool isSteamRunningInVR();
   void setOverlayNotificationPosition(ENotificationPosition newPos);
   void setOverlayNotificationInset(int nHorizontalInset, int nVerticalInset);
   bool showGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode,
      const char* pchDescription, uint32 unCharMax, const char* pchExistingText);
};

#endif
