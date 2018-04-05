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

#include "platform/platform.h"
#include "core/module.h"
#include "math/mMathFn.h"
#include "sdlHapticDevice.h"
#include "sdlHaptic.h"

IMPLEMENT_STATIC_CLASS( SDLHaptic,, "@brief Module exposing the SDL_Haptic API to Torque Script.\n"
   "SDLHaptic provides access to all of the functions of the SDL_Haptic api through static class "
   "functions or returned SimObjects. Functions that would return a SDL_Haptic* to reference a "
   "device instead return an object id for an SDLHapticDevice object that can be used from script "
   "to access device features. Effects are similarly handled by SDLHapticEffect.\n\n"

   "@tsexample\n"
   "if (SDLHaptic::numHaptics() > 0)\n"
   "{\n"
   "   %device = SDLHaptic::open(0); // Open the first device\n"
   "   if (%device.rumbleSupported())\n"
   "      %device.rumblePlay(0.5, 2000); // Rumble at 50% for 2 seconds\n"
   "   SDLHaptic::close(0);\n"
   "}\n"
"@endtsexample\n\n");
SDLHaptic *gSDLHaptic;

MODULE_BEGIN( SDLHaptic )
MODULE_SHUTDOWN_BEFORE(Sim)

MODULE_INIT
   {
      gSDLHaptic = new SDLHaptic;
      gSDLHaptic->init();
   }
   MODULE_SHUTDOWN
   {
      gSDLHaptic->shutdown();
      delete gSDLHaptic;
   }
MODULE_END;

SDLHaptic::SDLHaptic()
{
   for (S32 i = 0; i < MaxHaptics; ++i)
      mDevices[i] = NULL;
}

SDLHaptic::~SDLHaptic()
{
}

bool SDLHaptic::init()
{
   Con::setIntVariable("$SDLTypeMasks::HapticConstant",     SDL_HAPTIC_CONSTANT);
   Con::setIntVariable("$SDLTypeMasks::HapticSine",         SDL_HAPTIC_SINE);
   Con::setIntVariable("$SDLTypeMasks::HapticLeftRight",    SDL_HAPTIC_LEFTRIGHT);
   Con::setIntVariable("$SDLTypeMasks::HapticTriangle",     SDL_HAPTIC_TRIANGLE);
   Con::setIntVariable("$SDLTypeMasks::HapticSawToothUp",   SDL_HAPTIC_SAWTOOTHUP);
   Con::setIntVariable("$SDLTypeMasks::HapticSawToothDown", SDL_HAPTIC_SAWTOOTHDOWN);
   Con::setIntVariable("$SDLTypeMasks::HapticRamp",         SDL_HAPTIC_RAMP);
   Con::setIntVariable("$SDLTypeMasks::HapticSpring",       SDL_HAPTIC_SPRING);
   Con::setIntVariable("$SDLTypeMasks::HapticDamper",       SDL_HAPTIC_DAMPER);
   Con::setIntVariable("$SDLTypeMasks::HapticInertia",      SDL_HAPTIC_INERTIA);
   Con::setIntVariable("$SDLTypeMasks::HapticFriction",     SDL_HAPTIC_FRICTION);
   Con::setIntVariable("$SDLTypeMasks::HapticCustom",       SDL_HAPTIC_CUSTOM);
   Con::setIntVariable("$SDLTypeMasks::HapticGain",         SDL_HAPTIC_GAIN);
   Con::setIntVariable("$SDLTypeMasks::HapticAutocenter",   SDL_HAPTIC_AUTOCENTER);
   Con::setIntVariable("$SDLTypeMasks::HapticStatus",       SDL_HAPTIC_STATUS);
   Con::setIntVariable("$SDLTypeMasks::HapticPause",        SDL_HAPTIC_PAUSE);
   return true;
}

void SDLHaptic::shutdown()
{
   for (S32 i = 0; i < MaxHaptics; ++i)
   {
      if (mDevices[i])
      {
         mDevices[i]->unregisterObject();
         delete mDevices[i];
      }

      mDevices[i] = NULL;
   }
}


SDLHapticDevice* SDLHaptic::openFromMouse()
{
   SDL_Haptic* mouseDev = SDL_HapticOpenFromMouse();
   if (mouseDev)
   {
      S32 index = SDL_HapticIndex(mouseDev);
      if (index < 0)
      {
         Con::errorf("SDL_Haptic error: %s", SDL_GetError());
         SDL_HapticClose(mouseDev);
      }
      else
      {
         SDLHapticDevice*  pObj = new SDLHapticDevice(mouseDev);
         pObj->registerObject();
         mDevices[index] = pObj;
         return pObj;
      }
   }
   return NULL;
}

SDLHapticDevice* SDLHaptic::openFromJoystick(S32 sdlJoystickIndex)
{
   if (sdlJoystickIndex < 0 || sdlJoystickIndex >= SDL_NumJoysticks())
      return NULL;

   SDL_Joystick *joystickDevice = SDL_JoystickOpen(sdlJoystickIndex);
   if (joystickDevice)
   {
      SDL_Haptic* hapticDevice = SDL_HapticOpenFromJoystick(joystickDevice);
      if (hapticDevice)
      {
         S32 hapticIndex = SDL_HapticIndex(hapticDevice);
         if (hapticIndex < 0)
         {
            Con::errorf("SDLHaptic::openFromJoystick() error: %s", SDL_GetError());
            SDL_HapticClose(hapticDevice);
         }
         else
         {
            SDLHapticDevice*  pObj = new SDLHapticDevice(hapticDevice);
            pObj->registerObject();
            mDevices[hapticIndex] = pObj;
            SDL_JoystickClose(joystickDevice);
            return pObj;
         }
      }
      else
         Con::errorf("SDLHaptic::openFromJoystick() error: %s", SDL_GetError());

      SDL_JoystickClose(joystickDevice);
   }
   else
      Con::errorf("SDLHaptic::openFromJoystick() error: %s", SDL_GetError());

   return NULL;
}

const char* SDLHaptic::getDeviceName(S32 device_index)
{
   if ((device_index < 0) || (device_index >= MaxHaptics))
      return NULL;

   const char* ret = SDL_HapticName(device_index);
   if (ret)
      return ret;

   Con::errorf("SDL_Haptic error: %s", SDL_GetError());
   return NULL;
}

SDLHapticDevice* SDLHaptic::openDevice(S32 device_index)
{
   if ((device_index < 0) || (device_index >= MaxHaptics))
      return NULL;
   
   if (mDevices[device_index])
      return mDevices[device_index]; // Already open

   SDL_Haptic* devId = SDL_HapticOpen(device_index);
   if (devId)
   {
      SDLHapticDevice*  pObj = new SDLHapticDevice(devId);
      pObj->registerObject();
      mDevices[device_index] = pObj;
      return pObj;
   }
   else
      Con::errorf("SDL_Haptic error: %s", SDL_GetError());

   return NULL;
}

void SDLHaptic::closeDevice(S32 device_index)
{
   if ((device_index < 0) || (device_index >= MaxHaptics))
      return;
   if (mDevices[device_index])
   {
      SDL_HapticClose(mDevices[device_index]->getDevice());
      mDevices[device_index]->unregisterObject();
      delete mDevices[device_index];
   }

   mDevices[device_index] = NULL;
}

bool SDLHaptic::isOpen(S32 device_index)
{
   if ((device_index >= 0) && (device_index < MaxHaptics) && mDevices[device_index])
      return true;

   return false;
}

SDLHapticDevice* SDLHaptic::getDevice(S32 device_index)
{
   if ((device_index < 0) || (device_index >= MaxHaptics))
      return NULL;

   return mDevices[device_index];
}

DefineEngineStaticMethod(SDLHaptic, numHaptics, S32, (), ,
   "@brief Use this function to count the number of haptic devices attached to the system.\n\n"
   "@return Returns the number of haptic devices detected on the system or a negative error code on failure.\n"
   "@see https://wiki.libsdl.org/SDL_NumHaptics \n"
   "@ingroup SDLHaptic")
{
   S32 ret = SDL_NumHaptics();
   if (ret < 0)
      Con::errorf("SDL_Haptic error: %s", SDL_GetError());

   return ret;
}

DefineEngineStaticMethod(SDLHaptic, mouseIsHaptic, bool, (), ,
   "@brief Use this function to get whether or not the current mouse has haptic capabilities.\n\n"
   "@return Returns true if the mouse is haptic or false if it isn't.\n"
   "@see https://wiki.libsdl.org/SDL_MouseIsHaptic \n"
   "@ingroup SDLHaptic")
{
   return (SDL_MouseIsHaptic() == SDL_TRUE);
}

DefineEngineStaticMethod(SDLHaptic, openFromMouse, SDLHapticDevice*, (), ,
   "@brief Use this function to try to open a haptic device from the current mouse.\n\n"
   "@return Returns the SDLHapticDevice object or NULL on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticOpenFromMouse \n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->openFromMouse();
}

DefineEngineStaticMethod(SDLHaptic, openFromJoystick, SDLHapticDevice*, (S32 sdlJoystickIndex), (0),
   "@brief Use this function to try to open a haptic device from a joystick.\n\n"
   "@param sdlJoystickIndex Refers to the N'th joystick presently recognized by SDL on the system. "
   "This is not always the same as the torque device instance id used when generating events for this device.\n"
   "@return Returns the SDLHapticDevice object or NULL on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticOpenFromJoystick \n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->openFromJoystick(sdlJoystickIndex);
}

DefineEngineStaticMethod(SDLHaptic, getName, const char*, (S32 device_index), (0),
   "@brief Use this function to get the implementation dependent name of a haptic device.\n\n"
   "@param device_index The index of device to get the name of. "
   "@return Returns the name of the device or an empty string on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticName \n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->getDeviceName(device_index);
}

DefineEngineStaticMethod(SDLHaptic, open, SDLHapticDevice*, (S32 device_index), (0),
   "@brief Use this function to open a haptic device for use.\n\n"
   "When opening a haptic device, its gain will be set to maximum and autocenter "
   "will be disabled. To modify these values use SDL_HapticSetGain() and SDL_HapticSetAutocenter().\n"
   "@param device_index The index passed as an argument refers to the N'th Haptic device on this system. "
   "@return Returns the SDLHapticDevice object or NULL on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticOpen \n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->openDevice(device_index);
}

DefineEngineStaticMethod(SDLHaptic, close, void, (S32 device_index), (0),
   "@brief Use this function to close a haptic device previously opened with SDL_HapticOpen().\n\n"
   "@see https://wiki.libsdl.org/SDL_HapticClose \n"
   "@ingroup SDLHaptic")
{
   gSDLHaptic->closeDevice(device_index);
   return;
}

DefineEngineStaticMethod(SDLHaptic, isOpen, bool, (S32 device_index), (0),
   "@brief Use this function to check if a device is open.\n\n"
   "@param device_index The haptic device to check. "
   "The index passed as an argument refers to the N'th Haptic device on this system.\n"
   "@return Returns true if the device is open.\n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->isOpen(device_index);
}

DefineEngineStaticMethod(SDLHaptic, getDevice, SDLHapticDevice*, (S32 device_index), (0),
   "@brief Use this function to get the SDLHapticDevice object for an open haptic device.\n\n"
   "@param device_index The index passed as an argument refers to the N'th Haptic device on this system. "
   "@return Returns the SDLHapticDevice object or NULL on failure.\n"
   "@ingroup SDLHaptic")
{
   return gSDLHaptic->getDevice(device_index);
}

DefineEngineStaticMethod(SDLHaptic, getEffectName, SDLEffectType, (S32 mask), (0),
   "@brief Use this function to get the name of an effect from the mask value.\n\n"
   "@param mask The SDL effect mask value. "
   "@return Returns the effect name or an empty string on failure.\n"
   "@ingroup SDLHaptic")
{
   return (SDLEffectType) mask;
}
