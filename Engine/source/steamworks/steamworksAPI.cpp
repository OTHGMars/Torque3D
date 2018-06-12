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

#include "platform/platform.h"
#include "core/module.h"
#include "core/util/journal/process.h"
#include "steamworksAPI.h"
#include "steamCallbacks.h"
#include "steamConfig.h"

IMPLEMENT_STATIC_CLASS( SteamAPI,, "Module exposing the Steamworks API to Torque Script." );
SteamAPI *gSteamAPI;

MODULE_BEGIN( SteamAPI )
   MODULE_INIT_AFTER (ProcessList)
   MODULE_SHUTDOWN_BEFORE(ProcessList)

   MODULE_INIT
   {
      gSteamAPI = new SteamAPI;
      gSteamAPI->init();
   }
   MODULE_SHUTDOWN
   {
      gSteamAPI->shutdown();
      delete gSteamAPI;
   }
MODULE_END;

SteamAPI::SteamAPI():
mCallbacks(NULL),
mIsSteamRunning(false),
mProcessList(NULL),
mTics(0),
mStatsLoaded(false)
{
}

SteamAPI::~SteamAPI()
{
}

bool SteamAPI::init()
{
#ifdef TORQUE_STEAMWORKS_REQUIRED
   if ( SteamAPI_RestartAppIfNecessary(TORQUE_STEAMWORKS_APPID) )
   {
      // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
      // local Steam client and also launches this game again.
		
      Platform::postQuitMessage(EXIT_FAILURE);
      return false;
   }
#endif // TORQUE_STEAMWORKS_REQUIRED

	mIsSteamRunning = SteamAPI_Init();

   // If steam is running, we need ticked to process events and callbacks
   if ( mIsSteamRunning )
   {
      mProcessList = ServerProcessList::get();
      mProcessList->preTickSignal().notify(this, &SteamAPI::process);

      mCallbacks = new SteamCallbacks;
   }

   return mIsSteamRunning;
}

void SteamAPI::shutdown()
{
   if (mCallbacks)
      delete mCallbacks;

   // Shutdown the SteamAPI
   if ( mIsSteamRunning )
   {
      mProcessList->preTickSignal().remove(this, &SteamAPI::process);
      mProcessList = NULL;
      SteamAPI_Shutdown();
   }
}

void SteamAPI::process()
{
   //
   if ( mIsSteamRunning )
   {
      SteamAPI_RunCallbacks();
      mTics++;
   }
}

DefineEngineStaticMethod(SteamAPI, isSteamRunning, bool, (), ,
   "@brief Returns true if Steam is running and the Steamworks API has been initialized.\n\n"
   "@ingroup Steamworks")
{
   return gSteamAPI->isSteamRunning();
}
