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
#include "core/color.h"
#include "core/module.h"
#include "core/util/journal/process.h"
#include "steamInput.h"
#include "steamworks/steamConfig.h"
#include "steamworks/steamworksAPI.h"

extern SteamAPI *gSteamAPI;

IMPLEMENT_STATIC_CLASS(SteamInput,, "Module exposing the ISteamController interface to Torque Script." );
SteamInput *gSteamInput;

MODULE_BEGIN(SteamInput)
   MODULE_INIT_AFTER (SteamAPI)
   MODULE_SHUTDOWN_BEFORE(SteamAPI)

   MODULE_INIT
   {
      gSteamInput = new SteamInput;
      gSteamInput->init();
   }
   MODULE_SHUTDOWN
   {
      gSteamInput->shutdown();
      delete gSteamInput;
   }
MODULE_END;

IMPLEMENT_GLOBAL_CALLBACK(onSteamControllerChange, void, (S32 numControllers), (numControllers),
   "Callback posted when a game controller device has been connected or disconnected.\n\n"
   "@param numControllers The number of controllers currently recognized by Steam Input. "
   "Indexes are automatically adjusted and will be 0 - (numControllers - 1)  when accessing "
   "controller data through the SteamInput API.\n"
   "@ingroup Steam");

typedef ESteamInputType SteamInputDevices;
DefineEnumType(SteamInputDevices);
ImplementEnumType(SteamInputDevices,
   "The device types currently supported by steamworks (1.4.2). These are meant "
   "to be localization tags.\n\n"
   "@ingroup Steam")
{ k_ESteamInputType_Unknown, "unknown" },
{ k_ESteamInputType_SteamController, "vsc" },
{ k_ESteamInputType_XBox360Controller, "xbox360" },
{ k_ESteamInputType_XBoxOneController, "xboxone" },
{ k_ESteamInputType_GenericXInput, "generic" },
{ k_ESteamInputType_PS4Controller, "ps4" },
EndImplementEnumType;

typedef ESteamControllerPad SteamControllerPads;
DefineEnumType(SteamControllerPads);
ImplementEnumType(SteamControllerPads,
   "The haptic touch pad identifiers for a VSC.\n\n"
   "@ingroup Steam")
{ k_ESteamControllerPad_Left, "Left" },
{ k_ESteamControllerPad_Right, "Right" },
EndImplementEnumType;

SteamInput::SteamInput() :
mInputInitialized(false),
mNumControllers(0)
{
   dMemset(mActiveControllers, 0, sizeof(mActiveControllers));
}

SteamInput::~SteamInput()
{
}

bool SteamInput::init()
{
   // If steam is running, we need ticked to process events and callbacks
   if (gSteamAPI && gSteamAPI->isSteamRunning())
   {
      SteamController()->Init();

      Process::notify(SteamInput::process, PROCESS_INPUT_ORDER);
      mInputInitialized = true;
   }
   return true;
}

void SteamInput::shutdown()
{
   if (mInputInitialized)
   {
      Process::remove(SteamInput::process);
      SteamController()->Shutdown();
   }
}

void SteamInput::process()
{
   if (gSteamInput->isInputInitialized())
   {
      SteamController()->RunFrame();
      gSteamInput->pollControllers();
   }
}

void SteamInput::pollControllers()
{
   static const char *argv[4];
   int numControllers = SteamController()->GetConnectedControllers(mActiveControllers);

   if (numControllers != mNumControllers)
   {  // A controller has been connected or disconnected
      mNumControllers = numControllers;
      onSteamControllerChange_callback(numControllers);
   }

   if (numControllers > 0)
   {
      int numDigital = mDigitalActions.size();
      int numAnalog = mAnalogActions.size();
      for (int i = 0; i < numControllers; ++i)
      {
         for (int j = 0; j < numDigital; ++j)
         {
            ControllerDigitalActionData_t buttonState = SteamController()->GetDigitalActionData(
                  mActiveControllers[i], mDigitalActions[j].actionHandle);
            if (buttonState.bActive && (buttonState.bState != mDigitalActions[j].lastState[i]))
            {
               mDigitalActions[j].lastState[i] = buttonState.bState;
               argv[0] = mDigitalActions[j].callback;
               argv[1] = Con::getIntArg(i);
               argv[2] = Con::getBoolArg(buttonState.bState);
               Con::execute(3, argv);
            }
         }

         for (int j = 0; j < numAnalog; ++j)
         {
            ControllerAnalogActionData_t axesStates = SteamController()->GetAnalogActionData(
               mActiveControllers[i], mAnalogActions[j].actionHandle);
            if (axesStates.bActive && ((axesStates.x != mAnalogActions[j].lastXAxis[i]) ||
               (axesStates.y != mAnalogActions[j].lastYAxis[i])))
            {
               mAnalogActions[j].lastXAxis[i] = axesStates.x;
               mAnalogActions[j].lastYAxis[i] = axesStates.y;
               argv[0] = mAnalogActions[j].callback;
               argv[1] = Con::getIntArg(i);
               argv[2] = Con::getFloatArg(axesStates.x);
               argv[3] = Con::getFloatArg(axesStates.y);
               Con::execute(4, argv);
            }
         }
      }
   }
}

ControllerHandle_t SteamInput::getControllerHandle(S32 controllerIndex)
{
   if (controllerIndex < mNumControllers)
   {
      return mActiveControllers[controllerIndex];
   }
   return (ControllerHandle_t) 0;
}

S32 SteamInput::addActionSet(const char* setName)
{
   if (setName)
   {
      ControllerActionSetHandle_t setHandle = SteamController()->GetActionSetHandle(setName);
      if (setHandle > 0)
      {
         S32 retIndex = mActionSets.size();
         mActionSets.push_back(ActionSet(setHandle, setName));
         //Con::printf("Action Set %s, Handle: %I64u, Index: %d", setName, setHandle, retIndex);
         return retIndex;
      }
      Con::warnf("SteamInput::addActionSet failed for action set: %s.", setName);
   }
   return -1;
}

S32 SteamInput::addActionSetLayer(const char* layerName)
{
   if (layerName)
   {
      ControllerActionSetHandle_t setHandle = SteamController()->GetActionSetHandle(layerName);
      if (setHandle > 0)
      {
         S32 retIndex = mActionSetLayers.size();
         mActionSetLayers.push_back(ActionSet(setHandle, layerName));
         //Con::printf("Action Set Layer %s, Handle: %I64u, Index: %d", layerName, setHandle, retIndex);
         return retIndex;
      }
      Con::warnf("SteamInput::addActionSetLayer failed for action set layer: %s.", layerName);
   }
   return -1;
}

S32 SteamInput::addAnalogAction(const char* actionName, const char* callbackFunc)
{
   if (actionName && callbackFunc)
   {
      ControllerAnalogActionHandle_t actionHandle = SteamController()->GetAnalogActionHandle(actionName);
      if (actionHandle > 0)
      {
         S32 retIndex = mAnalogActions.size();
         mAnalogActions.push_back(AnalogAction(actionHandle, actionName, callbackFunc));
         //Con::printf("Analog Action %s, Handle: %I64u, Index: %d", actionName, actionHandle, retIndex);
         return retIndex;
      }
      Con::warnf("SteamInput::addAnalogAction failed for action: %s.", actionName);
   }
   return -1;
}

S32 SteamInput::addDigitalAction(const char* actionName, const char* callbackFunc)
{
   if (actionName && callbackFunc)
   {
      ControllerDigitalActionHandle_t actionHandle = SteamController()->GetDigitalActionHandle(actionName);
      if (actionHandle > 0)
      {
         S32 retIndex = mDigitalActions.size();
         mDigitalActions.push_back(DigitalAction(actionHandle, actionName, callbackFunc));
         //Con::printf("Digital Action %s, Handle: %I64u, Index: %d", actionName, actionHandle, retIndex);
         return retIndex;
      }
      Con::warnf("SteamInput::addDigitalAction failed for action: %s.", actionName);
   }
   return -1;
}

bool SteamInput::activateActionSet(S32 controllerIndex, S32 setIndex)
{
   if (setIndex < mActionSets.size())
   {
      if (controllerIndex < 0)
      {
         SteamController()->ActivateActionSet(STEAM_CONTROLLER_HANDLE_ALL_CONTROLLERS, mActionSets[setIndex].setHandle);
         return true;
      }
      else if (controllerIndex < mNumControllers)
      {
         SteamController()->ActivateActionSet(mActiveControllers[controllerIndex], mActionSets[setIndex].setHandle);
         return true;
      }
   }
   return false;
}

const char* SteamInput::getCurrentActionSet(S32 controllerIndex)
{
   if (controllerIndex < mNumControllers)
   {
      ControllerActionSetHandle_t setHandle = SteamController()->GetCurrentActionSet(mActiveControllers[controllerIndex]);
      int numSets = mActionSets.size();
      for (int i = 0; i < numSets; ++i)
      {
         if (setHandle == mActionSets[i].setHandle)
            return mActionSets[i].setName;
      }
   }
   return nullptr;
}

bool SteamInput::activateActionSetLayer(S32 controllerIndex, S32 layerIndex)
{
   if (layerIndex < mActionSetLayers.size())
   {
      if (controllerIndex < 0)
      {
         SteamController()->ActivateActionSetLayer(STEAM_CONTROLLER_HANDLE_ALL_CONTROLLERS, mActionSetLayers[layerIndex].setHandle);
         return true;
      }
      else if (controllerIndex < mNumControllers)
      {
         SteamController()->ActivateActionSetLayer(mActiveControllers[controllerIndex], mActionSetLayers[layerIndex].setHandle);
         return true;
      }
   }
   return false;
}

bool SteamInput::deactivateActionSetLayer(S32 controllerIndex, S32 layerIndex)
{
   if (layerIndex < mActionSetLayers.size())
   {
      if (controllerIndex < 0)
      {
         SteamController()->DeactivateActionSetLayer(STEAM_CONTROLLER_HANDLE_ALL_CONTROLLERS, mActionSetLayers[layerIndex].setHandle);
         return true;
      }
      else if (controllerIndex < mNumControllers)
      {
         SteamController()->DeactivateActionSetLayer(mActiveControllers[controllerIndex], mActionSetLayers[layerIndex].setHandle);
         return true;
      }
   }
   return false;
}

bool SteamInput::deactivateAllActionSetLayers(S32 controllerIndex)
{
   if (controllerIndex < 0)
   {
      SteamController()->DeactivateAllActionSetLayers(STEAM_CONTROLLER_HANDLE_ALL_CONTROLLERS);
      return true;
   }
   else if (controllerIndex < mNumControllers)
   {
      SteamController()->DeactivateAllActionSetLayers(mActiveControllers[controllerIndex]);
      return true;
   }
   return false;
}

S32 SteamInput::getAnalogActionOrigins(S32 controllerIndex, S32 setIndex, S32 actionIndex, String &originsOut)
{
   static EControllerActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
   S32 count = 0;
   if ((controllerIndex < mNumControllers) && (setIndex < mActionSets.size()) && (actionIndex < mAnalogActions.size()))
   {
      count = SteamController()->GetAnalogActionOrigins(mActiveControllers[controllerIndex],
         mActionSets[setIndex].setHandle, mAnalogActions[actionIndex].actionHandle, origins);
      for (S32 i = 0; i < count; ++i)
         originsOut += String::ToString("%I64u\t", origins[i]);
   }
   return count;
}

S32 SteamInput::getDigitalActionOrigins(S32 controllerIndex, S32 setIndex, S32 actionIndex, String & originsOut)
{
   static EControllerActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
   S32 count = 0;
   if ((controllerIndex < mNumControllers) && (setIndex < mActionSets.size()) && (actionIndex < mDigitalActions.size()))
   {
      count = SteamController()->GetDigitalActionOrigins(mActiveControllers[controllerIndex],
         mActionSets[setIndex].setHandle, mDigitalActions[actionIndex].actionHandle, origins);
      for (S32 i = 0; i < count; ++i)
         originsOut += String::ToString("%I64u\t", origins[i]);
   }
   return count;
}

const char* SteamInput::getGlyphForActionOrigin(S32 origin)
{
   if (origin > k_EControllerActionOrigin_None && origin < k_EControllerActionOrigin_Count)
   {
      // The Steam API will return an empty string for Right-Sticks if the art matches
      // the art for the Left-Stick (like xbox controllers). I'm changing right to left
      // here so the function returns the correct art for right sticks. (Steamworks 1.4.2)
      if (origin >= k_EControllerActionOrigin_XBoxOne_RightStick_Move && origin <= k_EControllerActionOrigin_XBoxOne_RightStick_DPadEast)
         origin -= k_EControllerActionOrigin_XBoxOne_RightStick_Move - k_EControllerActionOrigin_XBoxOne_LeftStick_Move;

      else if (origin >= k_EControllerActionOrigin_XBox360_RightStick_Move && origin <= k_EControllerActionOrigin_XBox360_RightStick_DPadEast)
         origin -= k_EControllerActionOrigin_XBox360_RightStick_Move - k_EControllerActionOrigin_XBox360_LeftStick_Move;

      return SteamController()->GetGlyphForActionOrigin((EControllerActionOrigin)origin);
   }
   return nullptr;
}

const char * SteamInput::getStringForActionOrigin(S32 origin)
{
   if (origin > k_EControllerActionOrigin_None && origin < k_EControllerActionOrigin_Count)
      return SteamController()->GetStringForActionOrigin((EControllerActionOrigin)origin);
   return nullptr;
}

void SteamInput::setLEDColor(S32 controllerIndex, ColorI ledColor, bool userDefault)
{
   if (controllerIndex < mNumControllers)
   {
      unsigned int nFlags = userDefault ? k_ESteamControllerLEDFlag_RestoreUserDefault : k_ESteamControllerLEDFlag_SetColor;
      SteamController()->SetLEDColor(mActiveControllers[controllerIndex], ledColor.red, ledColor.green, ledColor.blue, nFlags);
   }
}

bool SteamInput::showBindingPanel(S32 controllerIndex)
{
   if (controllerIndex < mNumControllers)
   {
      return SteamController()->ShowBindingPanel(mActiveControllers[controllerIndex]);
   }
   return false;
}

S32 SteamInput::getInputTypeForDevice(S32 controllerIndex)
{
   if (controllerIndex < mNumControllers)
   {
      return (S32) SteamController()->GetInputTypeForHandle(mActiveControllers[controllerIndex]);
   }
   return -1;
}

void SteamInput::stopAnalogActionMomentum(S32 controllerIndex, S32 actionIndex)
{
   if ((controllerIndex < mNumControllers) && (actionIndex < mAnalogActions.size()))
   {
      SteamController()->StopAnalogActionMomentum(mActiveControllers[controllerIndex], 
         mAnalogActions[actionIndex].actionHandle);
   }
}

void SteamInput::triggerRepeatedHapticPulse(S32 controllerIndex, ESteamControllerPad eTargetPad,
   unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags)
{
   if (controllerIndex < mNumControllers)
   {
      SteamController()->TriggerRepeatedHapticPulse(mActiveControllers[controllerIndex], eTargetPad,
         usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
   }
}

void SteamInput::triggerVibration(S32 controllerIndex, unsigned short usLeftSpeed, unsigned short usRightSpeed)
{
   if (controllerIndex < mNumControllers)
   {
      SteamController()->TriggerVibration(mActiveControllers[controllerIndex], usLeftSpeed, usRightSpeed);
   }
}

S32 SteamInput::getControllerForGamepadIndex(int nIndex)
{
   ControllerHandle_t controllerHandle = SteamController()->GetControllerForGamepadIndex(nIndex);
   for (int i = 0; i < mNumControllers; ++i)
   {
      if (mActiveControllers[i] == controllerHandle)
         return i;
   }
   return -1;
}

S32 SteamInput::getGamepadIndexForController(S32 controllerIndex)
{
   if (controllerIndex < mNumControllers)
   {
      return SteamController()->GetGamepadIndexForController(mActiveControllers[controllerIndex]);
   }
   return -1;
}

DefineEngineFunction(isSteamInputInitialized, bool, (), ,
   "Returns true if the SteamController interface is installed and initialized.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->isInputInitialized();
   return false;
}

DefineEngineStaticMethod(SteamInput, getNumControllers, S32, (), ,
   "Gets the number of controllers currently recognized by steam.\n\n"
   "@return The number of controllers or -1 if SteamInput has not been initialized. "
   "This will be 0 while the SteamController library is still initializing. If there are "
   "controllers connected initially, listen for the onSteamControllerChange() callback "
   "that will fire when the controllers are detected.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->getNumControllers();
   return -1;
}

DefineEngineStaticMethod(SteamInput, getControllerHandle, String, (S32 controllerIndex), ,
   "Gets a controllers device handle ID. This value should only be needed "
   "if you need to keep track of multiple players on the same client. If a controller "
   "is disconnected and re-connected, it's index value may change, but the device handle will not.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1.\n"
   "@return The controller handle ID or 0 if there is no active controller for the passed index.\n"
   "@ingroup Steamworks")
{
   String outStr;
   ControllerHandle_t handle = 0;
   if (gSteamInput && controllerIndex >= 0)
      handle =  gSteamInput->getControllerHandle(controllerIndex);
   outStr = String::ToString("%I64u", handle);
   return outStr;
}

DefineEngineStaticMethod(SteamInput, addActionSet, S32, (const char* setName), ,
   "Adds an action set and saves the handle for it.\n\n"
   "@param setName The action set identifier from the IGA .vdf file.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action set. "
   "The return value will be -1 if the action set could not be found by Steamworks.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->addActionSet(setName);
   return -1;
}

DefineEngineStaticMethod(SteamInput, addActionSetLayer, S32, (const char* layerName), ,
   "Adds an action set layer and saves the handle for it.\n\n"
   "@param setName The action set layer identifier from the IGA .vdf file.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action set layer. "
   "The return value will be -1 if the action set could not be found by Steamworks.\n"
   "@see https://partner.steamgames.com/doc/features/steam_controller/action_set_layers for additional information.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->addActionSetLayer(layerName);
   return -1;
}

DefineEngineStaticMethod(SteamInput, addAnalogAction, S32, (const char* actionName, const char* callbackFunc), ,
   "Adds an analog action, maps it's callback function and saves the handle for it.\n\n"
   "@param actionName The action name from the IGA .vdf file.\n"
   "@param callbackFunc The function to call anytime the axes data changes.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by Steamworks. The integer "
   "identifier is needed to reference the action in calls to get updated glyphs and bindings from steam.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->addAnalogAction(actionName, callbackFunc);
   return -1;
}

DefineEngineStaticMethod(SteamInput, addDigitalAction, S32, (const char* actionName, const char* callbackFunc), ,
   "Adds a digital action, maps it's callback function and saves the handle for it.\n\n"
   "@param actionName The action name from the IGA .vdf file.\n"
   "@param callbackFunc The function to call when the input state changes.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by Steamworks. The integer "
   "identifier is needed to reference the action in calls to get updated glyphs and bindings from steam.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->addDigitalAction(actionName, callbackFunc);
   return -1;
}

DefineEngineStaticMethod(SteamInput, activateActionSet, bool, (S32 controllerIndex, S32 setIndex), ,
   "Reconfigure the controller to use the specified action set.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the update applied to all controllers.\n"
   "@param setIndex The index value that was returned from SteamInput::addActionSet().\n"
   "@return True if the controller and set are valid and the update was applied, otherwise false.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && setIndex >= 0)
      return gSteamInput->activateActionSet(controllerIndex, setIndex);
   return false;
}

DefineEngineStaticMethod(SteamInput, getCurrentActionSet, const char*, (S32 controllerIndex), ,
   "Get the currently active action set for the specified controller.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@return The name passed when adding the action set.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->getCurrentActionSet(controllerIndex);
   return NULL;
}

DefineEngineStaticMethod(SteamInput, activateActionSetLayer, bool, (S32 controllerIndex, S32 layerIndex), ,
   "Activates the specified action set layer. Activating an action set layer that is "
   "already active will have no effect.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the update applied to all controllers.\n"
   "@param layerIndex The index value that was returned from SteamInput::addActionSetLayer().\n"
   "@return True if the controller and layer are valid and the update was applied, otherwise false.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && layerIndex >= 0)
      return gSteamInput->activateActionSetLayer(controllerIndex, layerIndex);
   return false;
}

DefineEngineStaticMethod(SteamInput, deactivateActionSetLayer, bool, (S32 controllerIndex, S32 layerIndex), ,
   "Deactivates the specified action set layer.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the update applied to all controllers.\n"
   "@param layerIndex The index value that was returned from SteamInput::addActionSetLayer().\n"
   "@return True if the controller and layer are valid and the update was applied, otherwise false.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && layerIndex >= 0)
      return gSteamInput->deactivateActionSetLayer(controllerIndex, layerIndex);
   return false;
}

DefineEngineStaticMethod(SteamInput, deactivateAllActionSetLayers, bool, (S32 controllerIndex), ,
   "Deactivates the specified action set layer.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the update applied to all controllers.\n"
   "@return True if the controller updated, otherwise false.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->deactivateAllActionSetLayers(controllerIndex);
   return false;
}

DefineEngineStaticMethod(SteamInput, getAnalogActionOrigins, String, (S32 controllerIndex, S32 setIndex, S32 actionIndex), ,
   "Get the origin(s) for an analog action within an action set. Use this to display "
   "the appropriate on-screen prompt for the action.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1.\n"
   "@param setIndex The index value that was returned from SteamInput::addActionSet().\n"
   "@param actionIndex The index value that was returned from SteamInput::addAnalogAction().\n"
   "@return All origin identifiers bound to the action as a tab separated string. Use getField() "
   "and getFieldCount() to parse values from the string.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamController#EControllerActionOrigin for "
   "possible values.\n"
   "@ingroup Steamworks")
{
   String outStr;
   if (gSteamInput && controllerIndex >= 0 && setIndex >= 0 && actionIndex >= 0)
      gSteamInput->getAnalogActionOrigins(controllerIndex, setIndex, actionIndex, outStr);
   return outStr;
}

DefineEngineStaticMethod(SteamInput, getDigitalActionOrigins, String, (S32 controllerIndex, S32 setIndex, S32 actionIndex), ,
   "Get the origin(s) for a digital action within an action set. Use this to display "
   "the appropriate on-screen prompt for the action.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1.\n"
   "@param setIndex The index value that was returned from SteamInput::addActionSet().\n"
   "@param actionIndex The index value that was returned from SteamInput::addDigitalAction().\n"
   "@return All origin identifiers bound to the action as a tab separated string. Use getField() "
   "and getFieldCount() to parse values from the string.\n"
   "@see https://partner.steamgames.com/doc/api/ISteamController#EControllerActionOrigin for "
   "possible values.\n"
   "@ingroup Steamworks")
{
   String outStr;
   if (gSteamInput && controllerIndex >= 0 && setIndex >= 0 && actionIndex >= 0)
      gSteamInput->getDigitalActionOrigins(controllerIndex, setIndex, actionIndex, outStr);
   return outStr;
}

DefineEngineStaticMethod(SteamInput, getGlyphForActionOrigin, const char*, (S32 origin), ,
   "Gets a local path to art for on-screen glyph for a particular origin. The art will come "
   "from Steam's installed library and is available for all supported controllers. Example Path: "
   "\"C:/Program Files (x86)/Steam/tenfoot/resource/images/library/controller/api/ps4_button_x.png\".\n\n"
   "@param origin An origin value returned from getAnalogActionOrigins or getDigitalActionOrigins.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->getGlyphForActionOrigin(origin);
   return NULL;
}

DefineEngineStaticMethod(SteamInput, getStringForActionOrigin, const char*, (S32 origin), ,
   "Returns a localized string (from Steam's language setting) for the specified origin.\n\n"
   "@param origin An origin value returned from getAnalogActionOrigins or getDigitalActionOrigins.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->getStringForActionOrigin(origin);
   return NULL;
}

DefineEngineStaticMethod(SteamInput, setLEDColor, void, (S32 controllerIndex, ColorI ledColor, bool userDefault), ,
   "Set the controller LED color on supported controllers. The Valve Steam Controller "
   "does not support any color but white, and will interpret the RGB values as a "
   "greyscale value affecting the brightness of the Steam button LED. The PlayStation "
   "Dual Shock 4 responds to full color information and uses the values to set the "
   "color & brightness of the lightbar.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@param ledColor (ColorI) RGB color values (0-255).\n"
   "@param userDefault If true, the LED will be returned to it's user default LED state. "
   "If false, the passed color will be applied.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      gSteamInput->setLEDColor(controllerIndex, ledColor, userDefault);
}

DefineEngineStaticMethod(SteamInput, showBindingPanel, bool, (S32 controllerIndex), ,
   "Invokes the Steam overlay and brings up the binding screen.\n\n"
   "@param controllerIndex Zero-based index of the controller you want to bring "
   "up the binding screen for, 0 - getNumControllers()-1.\n"
   "@return true for success; false if overlay is disabled or unavailable.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0)
      return gSteamInput->showBindingPanel(controllerIndex);
   return false;
}

DefineEngineStaticMethod(SteamInput, getInputTypeForDevice, S32, (S32 controllerIndex), ,
   "Gets the current device type for a controller index.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@return The device type ID. See ESteamInputType for possible values or convert this "
   "value into a localization tag using getInputStringForType().\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0)
      return gSteamInput->getInputTypeForDevice(controllerIndex);
   return -1;
}

DefineEngineStaticMethod(SteamInput, getInputStringForType, SteamInputDevices, (S32 inputType), ,
   "Gets the device type identifier string for a controller index.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@return The device type string. See SteamInputDevices for possible values.\n"
   "@ingroup Steamworks")
{
   return (ESteamInputType) inputType;
}

DefineEngineStaticMethod(SteamInput, stopAnalogActionMomentum, void, (S32 controllerIndex, S32 analogActionIndex), ,
   "Stops the momentum of an analog action (where applicable, ie a touchpad w/ virtual trackball settings). "
   "This will also stop all associated haptics. This is useful for situations where you want to indicate to "
   "the user that the limit of an action has been reached, such as spinning a carousel or scrolling a gui.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@param analogActionIndex The index value that was returned from SteamInput::addAnalogAction().\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0 && analogActionIndex >= 0)
      gSteamInput->stopAnalogActionMomentum(controllerIndex, analogActionIndex);
}

DefineEngineStaticMethod(SteamInput, triggerRepeatedHapticPulse, void,
   (S32 controllerIndex, SteamControllerPads eTargetPad, S32 usDurationMicroSec, S32 usOffMicroSec, S32 usRepeat), ,
   "Triggers a repeated haptic pulse on supported controllers. Currently only the VSC supports "
   "haptic pulses. This API call will be ignored for incompatible controller models. Changing "
   "the duration_micro and offtime_micro parameters will change the 'texture' of the haptic pulse.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@param eTargetPad Which haptic touch pad to affect. 'Left' or 'Right' (see SteamControllerPads).\n"
   "@param usDurationMicroSec Duration of the pulse, in microseconds (1/1,000,000th of a second) 0-65535.\n"
   "@param usOffMicroSec Duration of the pause between pulses, in microseconds 0-65535.\n"
   "@param usRepeat Number of times to repeat the usDurationMicroSec / usOffMicroSec duty cycle 0-65535.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0)
      gSteamInput->triggerRepeatedHapticPulse(controllerIndex, eTargetPad, usDurationMicroSec, usOffMicroSec, usRepeat);
}

DefineEngineStaticMethod(SteamInput, triggerVibration, void, (S32 controllerIndex, S32 leftSpeed, S32 rightSpeed), ,
   "Trigger a vibration event on supported controllers. This API call will be ignored for "
   "incompatible controller models. This generates the traditional 'rumble' vibration effect. "
   "The VSC will emulate traditional rumble using its haptics.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@param leftSpeed The period of the left rumble motor's vibration, in microseconds 0-65535.\n"
   "@param rightSpeed The period of the right rumble motor's vibration, in microseconds 0-65535.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0)
      gSteamInput->triggerVibration(controllerIndex, leftSpeed, rightSpeed);
}

DefineEngineStaticMethod(SteamInput, getControllerForGamepadIndex, S32, (S32 nIndex), ,
   "Returns the associated controller index for the specified emulated gamepad.\n\n"
   "@param nIndex The index of the emulated gamepad you want to get a controller index for.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput)
      return gSteamInput->getControllerForGamepadIndex(nIndex);
   return -1;
}

DefineEngineStaticMethod(SteamInput, getGamepadIndexForController, S32, (S32 controllerIndex), ,
   "Returns the associated gamepad index for the specified controller, if emulating a gamepad.\n\n"
   "@param controllerIndex Zero-based index of the controller, 0 - getNumControllers()-1.\n"
   "@ingroup Steamworks")
{
   if (gSteamInput && controllerIndex >= 0)
      return gSteamInput->getGamepadIndexForController(controllerIndex);
   return -1;
}
