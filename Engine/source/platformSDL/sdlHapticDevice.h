//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
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
// SDL_Haptic <--> TorqueScript interface
//-----------------------------------------------------------------------------

#ifndef _SDLHAPTICDEVICE_H_
#define _SDLHAPTICDEVICE_H_

#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif
#include "sdlHapticEffect.h"
#include "SDL.h"

class SDLHapticDevice : public SimObject
{
   typedef SimObject Parent;

protected:
   SDL_Haptic* mDevice;
   U32 mDeviceCaps;
   S32 mGain;
   S32 mAutocenter;

   Vector<SDLHapticEffect*> mEffects;

public:
   DECLARE_CONOBJECT(SDLHapticDevice);
   DECLARE_DESCRIPTION("Haptic device instance.");

   SDLHapticDevice();
   SDLHapticDevice(SDL_Haptic* pDevice);
   virtual ~SDLHapticDevice();

   virtual void onRemove();

   inline SDL_Haptic* getDevice() { return mDevice; }

   const char* getDeviceName();
   S32 getDeviceIndex();
   inline U32 getDeviceCaps() { return mDeviceCaps; }
   inline S32 getGain() { return mGain; }
   inline S32 getAutocenter() { return mAutocenter; }

   S32 setGain(S32 gain);
   S32 setAutocenter(S32 autocenter);

   bool rumbleSupported();
   bool rumblePlay(F32 strength, U32 duration);
   bool rumbleStop();

   S32 numEffects();
   S32 numEffectsPlaying();
   S32 numAxes();

   inline S32 getEffectCount() { return mEffects.size(); }
   SDLHapticEffect* getEffect(S32 effect_index);
   SDLHapticEffect* createEffect(S32 effectMask);
   bool removeEffect(SDLHapticEffect* effect);
   S32 uploadEffect(SDLHapticEffect* effect);
   bool unloadEffect(SDLHapticEffect* effect);
   S32 updateEffect(SDLHapticEffect* effect);

   bool runEffect(SDLHapticEffect* effect, U32 iterations);
   S32 stopEffect(SDLHapticEffect* effect);

   S32 getEffectStatus(S32 effectId);

   S32 pause();
   S32 unpause();
   S32 stopAll();
};

#endif
