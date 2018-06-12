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

U32 SteamAPI::getUserId()
{
   U32 steamId = 0;

   if ( mIsSteamRunning )
   {
      CSteamID sID = SteamUser()->GetSteamID();
      if ( sID.GetEAccountType() != k_EAccountTypeInvalid )
         steamId = sID.GetAccountID();
   }

   return steamId;
}

DefineEngineStaticMethod(SteamAPI, getUserId, const char *, (), ,
   "@brief Returns the steam ID for the currently logged in steam user.\n\n"
   "@ingroup Steam")
{
   if ( !gSteamAPI )
      return NULL;

   U32 userId = gSteamAPI->getUserId();
   char *ret = Con::getReturnBuffer(16);
   dSprintf(ret, 16, "%u", userId);
   return ret;
}
