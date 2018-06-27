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

#ifndef _STEAMINPUT_H_
#define _STEAMINPUT_H_
#include "steam_gameserver.h"

#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif

class SteamInput
{
protected:
   struct ActionSet
   {
      ControllerActionSetHandle_t setHandle;
      StringTableEntry setName;

      ActionSet() { setHandle = 0U; setName = StringTable->EmptyString(); }
      ActionSet(ControllerActionSetHandle_t initHandle, const char* initName)
      {
         setHandle = initHandle;
         setName = StringTable->insert(initName, true);
      }
   };

   struct AnalogAction
   {
      ControllerAnalogActionHandle_t actionHandle;
      StringTableEntry actionName;
      StringTableEntry callback;
      float lastXAxis[STEAM_CONTROLLER_MAX_COUNT];
      float lastYAxis[STEAM_CONTROLLER_MAX_COUNT];

      AnalogAction() { actionHandle = 0U; actionName = StringTable->EmptyString(); callback = StringTable->EmptyString(); }
      AnalogAction(ControllerActionSetHandle_t initHandle, const char* initName, const char* initCallback)
      {
         actionHandle = initHandle;
         actionName = StringTable->insert(initName, true);
         callback = StringTable->insert(initCallback, false);
         dMemset(lastXAxis, 0, sizeof(lastXAxis));
         dMemset(lastYAxis, 0, sizeof(lastYAxis));
      }
   };

   struct DigitalAction
   {
      ControllerDigitalActionHandle_t actionHandle;
      StringTableEntry actionName;
      StringTableEntry callback;
      bool lastState[STEAM_CONTROLLER_MAX_COUNT];

      DigitalAction() { actionHandle = 0U; actionName = StringTable->EmptyString(); callback = StringTable->EmptyString(); }
      DigitalAction(ControllerActionSetHandle_t initHandle, const char* initName, const char* initCallback)
      {
         actionHandle = initHandle;
         actionName = StringTable->insert(initName, true);
         callback = StringTable->insert(initCallback, false);
         dMemset(lastState, 0, sizeof(lastState));
      }
   };

private:
   bool mInputInitialized;

   Vector<ActionSet> mActionSets;
   Vector<ActionSet> mActionSetLayers;
   Vector<AnalogAction> mAnalogActions;
   Vector<DigitalAction> mDigitalActions;
   int mNumControllers;
   ControllerHandle_t mActiveControllers[STEAM_CONTROLLER_MAX_COUNT];

public:
   DECLARE_STATIC_CLASS(SteamInput);

   SteamInput();
   virtual ~SteamInput();

   bool init();
   void shutdown();
   static void process();
   void pollControllers();

   inline bool isInputInitialized() { return mInputInitialized; }
   inline int getNumControllers() { return mNumControllers; }
   ControllerHandle_t getControllerHandle(S32 controllerIndex);

   S32 addActionSet(const char* setName);
   S32 addActionSetLayer(const char* layerName);
   S32 addAnalogAction(const char* actionName, const char* callbackFunc);
   S32 addDigitalAction(const char* actionName, const char* callbackFunc);

   bool activateActionSet(S32 controllerIndex, S32 setIndex);
   const char* getCurrentActionSet(S32 controllerIndex);
   bool activateActionSetLayer(S32 controllerIndex, S32 layerIndex);
   bool deactivateActionSetLayer(S32 controllerIndex, S32 layerIndex);
   bool deactivateAllActionSetLayers(S32 controllerIndex);

   S32 getAnalogActionOrigins(S32 controllerIndex, S32 setIndex, S32 actionIndex, String &originsOut);
   S32 getDigitalActionOrigins(S32 controllerIndex, S32 setIndex, S32 actionIndex, String &originsOut);
   const char* getGlyphForActionOrigin(S32 origin);
   const char* getStringForActionOrigin(S32 origin);

   void setLEDColor(S32 controllerIndex, ColorI ledColor, bool userDefault);
   bool showBindingPanel(S32 controllerIndex);
   S32 getInputTypeForDevice(S32 controllerIndex);

   void stopAnalogActionMomentum(S32 controllerIndex, S32 actionIndex);
   void triggerRepeatedHapticPulse(S32 controllerIndex, ESteamControllerPad eTargetPad,
      unsigned short usDurationMicroSec, unsigned short usOffMicroSec, unsigned short unRepeat, unsigned int nFlags = 0);
   void triggerVibration(S32 controllerIndex, unsigned short usLeftSpeed, unsigned short usRightSpeed);

   S32 getControllerForGamepadIndex(int nIndex);
   S32 getGamepadIndexForController(S32 controllerIndex);
};

#endif // _STEAMINPUT_H_
