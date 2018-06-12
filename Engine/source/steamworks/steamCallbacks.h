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

#ifndef _STEAMCALLBACKS_H_
#define _STEAMCALLBACKS_H_

#include "steam_gameserver.h"

class SteamCallbacks
{
public:
   SteamCallbacks();

private:
   STEAM_CALLBACK(SteamCallbacks, OnGameOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated);
   STEAM_CALLBACK(SteamCallbacks, OnUserStatsReceived, UserStatsReceived_t, m_CallbackUserStatsReceived);
   STEAM_CALLBACK(SteamCallbacks, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored);
   STEAM_CALLBACK(SteamCallbacks, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
};

#endif // _STEAMCALLBACKS_H_
