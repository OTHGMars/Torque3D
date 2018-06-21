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
#include "steamConfig.h"
extern SteamAPI *gSteamAPI;

typedef ENotificationPosition SteamNotificationPositions;
DefineEnumType(SteamNotificationPositions);
ImplementEnumType(SteamNotificationPositions,
   "An enumeration of overlay notification popup positions. Used with the "
   "SteamAPI::setOverlayNotificationPosition() function call.\n\n"
   "@ingroup Steam")
{ k_EPositionTopLeft, "Top Left" },
{ k_EPositionTopRight, "Top Right" },
{ k_EPositionBottomLeft, "Bottom Left" },
{ k_EPositionBottomRight, "Bottom Right" },
EndImplementEnumType;

typedef EGamepadTextInputMode SteamGamepadInputMode;
DefineEnumType(SteamGamepadInputMode);
ImplementEnumType(SteamGamepadInputMode,
   "Available input modes for the Big Picture text input dialog.\n\n"
   "@ingroup Steam")
{ k_EGamepadTextInputModeNormal, "Normal" },
{ k_EGamepadTextInputModePassword, "Password" },
EndImplementEnumType;

typedef EGamepadTextInputLineMode SteamGamepadInputLineMode;
DefineEnumType(SteamGamepadInputLineMode);
ImplementEnumType(SteamGamepadInputLineMode,
   "Available input modes for the Big Picture text input dialog.\n\n"
   "@ingroup Steam")
{ k_EGamepadTextInputLineModeSingleLine, "Single" },
{ k_EGamepadTextInputLineModeMultipleLines, "MultiLine" },
EndImplementEnumType;

bool SteamAPI::isOverlayEnabled()
{
   if (mIsSteamRunning)
      return SteamUtils()->IsOverlayEnabled();
   return false;
}

bool SteamAPI::isSteamInBigPictureMode()
{
   if (mIsSteamRunning)
      return SteamUtils()->IsSteamInBigPictureMode();
   return false;
}

bool SteamAPI::isSteamRunningInVR()
{
   if (mIsSteamRunning)
      return SteamUtils()->IsSteamRunningInVR();
   return false;
}

void SteamAPI::setOverlayNotificationPosition(ENotificationPosition newPos)
{
   if ( mIsSteamRunning )
   {
      SteamUtils()->SetOverlayNotificationPosition(newPos);
   }
}

void SteamAPI::setOverlayNotificationInset(int nHorizontalInset, int nVerticalInset)
{
   if (mIsSteamRunning)
   {
      SteamUtils()->SetOverlayNotificationInset(nHorizontalInset, nVerticalInset);
   }
}

bool SteamAPI::showGamepadTextInput(EGamepadTextInputMode eInputMode, EGamepadTextInputLineMode eLineInputMode,
   const char* pchDescription, uint32 unCharMax, const char* pchExistingText)
{
   if (mIsSteamRunning)
   {
      if (unCharMax > (TORQUE_STEAMWORKS_MAX_TEXT_INPUT - 1))
         unCharMax = TORQUE_STEAMWORKS_MAX_TEXT_INPUT - 1;
      return SteamUtils()->ShowGamepadTextInput(eInputMode, eLineInputMode, pchDescription, unCharMax, pchExistingText);
   }
   return false;
}

//-----------------------------------------------------------------------------

DefineEngineStaticMethod(SteamAPI, isOverlayEnabled, bool, ( ), ,
   "Checks if the Steam Overlay is running and the user can access it. The overlay "
   "process could take a few seconds to start and hook the game process, so this function "
   "will initially return false while the overlay is loading.\n\n"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->isOverlayEnabled();
}

DefineEngineStaticMethod(SteamAPI, isSteamInBigPictureMode, bool, (), ,
   "Checks if Steam & the Steam Overlay are running in Big Picture mode.\n\n"
   "@return true if the Big Picture overlay is available; otherwise, false."
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->isSteamInBigPictureMode();
}

DefineEngineStaticMethod(SteamAPI, isSteamRunningInVR, bool, (), ,
   "Checks if Steam is running in VR mode.\n\n"
   "@return true if Steam itself is running in VR mode; otherwise, false."
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->isSteamRunningInVR();
}

DefineEngineStaticMethod(SteamAPI, setOverlayNotificationPosition, void, (SteamNotificationPositions position), ,
   "@brief Sets which corner the Steam overlay notification popup should display itself in.\n\n"
   "@param position The corner to place the popups at, 'Top Left', 'Top Right', "
   "'Bottom Left' or 'Bottom Right'.\n"
   "@ingroup Steam")
{
   if ( !gSteamAPI )
      return;

   gSteamAPI->setOverlayNotificationPosition(position);
}

DefineEngineStaticMethod(SteamAPI, setOverlayNotificationInset, void, (S32 xOffset, S32 yOffset), (0, 0),
   "@brief Sets the inset of the overlay notification from the corner specified by SetOverlayNotificationPosition.\n\n"
   "@param xOffset The horizontal (left-right) distance in pixels from the corner.\n"
   "@param yOffset The vertical (up-down) distance in pixels from the corner.\n"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return;

   gSteamAPI->setOverlayNotificationInset(xOffset, yOffset);
}

DefineEngineStaticMethod(SteamAPI, showGamepadTextInput, bool,
   (SteamGamepadInputMode inputMode, SteamGamepadInputLineMode lineMode, const char* description,
   U32 maxLength, const char* existingText), ,
   "Activates the Big Picture text input dialog which only supports gamepad input.\n\n"
   "@param inputMode Selects the input mode to use, either 'Normal' or 'Password' (hidden text).\n"
   "@param lineMode Controls whether to use 'Single' or 'MultiLine' input.\n"
   "@param description Sets the description that should inform the user what the input dialog is for.\n"
   "@param maxLength The maximum number of characters that the user can input.\n"
   "@param existingText Sets the preexisting text which the user can edit.\n"
   "@return true if the big picture overlay is running; otherwise, false.\n"
   "@ingroup Steam")
{
   if (!gSteamAPI)
      return false;

   return gSteamAPI->showGamepadTextInput(inputMode, lineMode, description, maxLength, existingText);
}
