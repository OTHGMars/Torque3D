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

#ifndef _SDLHAPTIC_H_
#define _SDLHAPTIC_H_

#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif
#include "SDL.h"

class SDLHapticDevice;
class SDLHaptic
{
private:
   enum Constants {
      MaxHaptics = 8,    // Up to 8 haptic devices
   };

   SDLHapticDevice* mDevices[MaxHaptics];

public:
   DECLARE_STATIC_CLASS( SDLHaptic );

   SDLHaptic();
   virtual ~SDLHaptic();

   bool init();
   void shutdown();

   const char* getDeviceName(S32 device_index);
   SDLHapticDevice* openFromMouse();
   SDLHapticDevice* openDevice(S32 device_index);
   SDLHapticDevice* openFromJoystick(S32 sdlJoystickIndex);
   void closeDevice(S32 device_index);
   bool isOpen(S32 device_index);
   SDLHapticDevice* getDevice(S32 device_index);
};

#endif
