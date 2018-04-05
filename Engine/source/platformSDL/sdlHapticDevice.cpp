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

IMPLEMENT_CONOBJECT(SDLHapticDevice);

SDLHapticDevice::SDLHapticDevice()
{
   mDevice = NULL;
   mDeviceCaps = 0;
   mGain = 100;
   mAutocenter = 0;
}

SDLHapticDevice::SDLHapticDevice(SDL_Haptic* pDevice)
{
   mDevice = pDevice;
   mDeviceCaps = 0;
   mGain = 100;
   mAutocenter = 0;

   if (mDevice)
   {
      mDeviceCaps = SDL_HapticQuery(mDevice);
   }
}

SDLHapticDevice::~SDLHapticDevice()
{
}

void SDLHapticDevice::onRemove()
{
   if (mEffects.size() > 0)
   {
      for (S32 i = mEffects.size() - 1; i >= 0; i--)
         removeEffect(mEffects[i]);
   }

   Parent::onRemove();
}

const char* SDLHapticDevice::getDeviceName()
{
   S32 index = SDL_HapticIndex(mDevice);
   if (index >= 0)
   {
      const char* ret = SDL_HapticName(index);
      if (ret)
         return ret;
   }

   Con::errorf("SDL_Haptic error: %s", SDL_GetError());
   return NULL;
}

S32 SDLHapticDevice::getDeviceIndex()
{
   S32 index = -1;
   if (mDevice)
   {
      index = SDL_HapticIndex(mDevice);
      if (index < 0)
         Con::errorf("SDL_Haptic error: %s", SDL_GetError());
   }
   return index;
}

S32 SDLHapticDevice::setGain(S32 gain)
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticSetGain(mDevice, gain);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::setGain() error: %s", SDL_GetError());
      else
         mGain = gain;
   }
   else
      Con::errorf("SDLHapticDevice::setGain() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::setAutocenter(S32 autocenter)
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticSetAutocenter(mDevice, autocenter);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::setAutocenter() error: %s", SDL_GetError());
      else
         mAutocenter = autocenter;
   }
   else
      Con::errorf("SDLHapticDevice::setAutocenter() - Device not opened!");

   return retVal;
}

bool SDLHapticDevice::rumbleSupported()
{
   if (mDevice)
   {
      S32 retVal = SDL_HapticRumbleSupported(mDevice);
      if (retVal == SDL_TRUE)
         return true;

      if (retVal < 0)
         Con::errorf("SDLHapticDevice::rumbleSupported() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::rumbleSupported() - Device not opened!");

   return false;
}

bool SDLHapticDevice::rumblePlay(F32 strength, U32 duration)
{
   if (mDevice)
   {
      if (SDL_HapticRumbleInit(mDevice) < 0)
      {
         Con::errorf("SDLHapticDevice::rumblePlay() error: %s", SDL_GetError());
         return false;
      }

      if (SDL_HapticRumblePlay(mDevice, mClampF(strength, 0, 1), duration) != 0)
      {
         Con::errorf("SDLHapticDevice::rumblePlay() error: %s", SDL_GetError());
         return false;
      }

      return true;
   }
   else
      Con::errorf("SDLHapticDevice::rumblePlay() - Device not opened!");

   return false;
}

bool SDLHapticDevice::rumbleStop()
{
   if (mDevice)
   {
      if (SDL_HapticRumbleStop(mDevice) == 0)
         return true;

      Con::errorf("SDLHapticDevice::rumbleStop() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::rumbleStop() - Device not opened!");

   return false;
}

S32 SDLHapticDevice::numEffects()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticNumEffects(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::numEffects() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::numEffects() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::numEffectsPlaying()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticNumEffectsPlaying(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::numEffectsPlaying() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::numEffectsPlaying() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::numAxes()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticNumAxes(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::numAxes() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::numAxes() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::pause()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticPause(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::pause() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::pause() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::unpause()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticUnpause(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::unpause() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::unpause() - Device not opened!");

   return retVal;
}

S32 SDLHapticDevice::stopAll()
{
   S32 retVal = -1;
   if (mDevice)
   {
      retVal = SDL_HapticStopAll(mDevice);
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::stopAll() error: %s", SDL_GetError());
   }
   else
      Con::errorf("SDLHapticDevice::stopAll() - Device not opened!");

   return retVal;
}

SDLHapticEffect* SDLHapticDevice::getEffect(S32 effect_index)
{
   if ((effect_index >= 0) && (effect_index < mEffects.size()))
      return mEffects[effect_index];
   return NULL;
}

SDLHapticEffect* SDLHapticDevice::createEffect(S32 effectMask)
{
   SDLHapticEffect* pObj = new SDLHapticEffect;
   if (pObj)
   {
      if (pObj->initEffect((SDLHapticEffect::EffectType) effectMask) && pObj->registerObject())
      {
         mEffects.push_back(pObj);
         return pObj;
      }
      delete pObj;
   }
   return NULL;
}

bool SDLHapticDevice::removeEffect(SDLHapticEffect* effect)
{
   if (effect)
   {
      unloadEffect(effect); // Make sure it's not loaded

      for (S32 i = 0; i < mEffects.size(); ++i)
      {
         if (mEffects[i] == effect)
         {
            mEffects.erase((U32) i);
            effect->unregisterObject();
            delete effect;
            return true;
         }
      }
   }
   return false;
}

S32 SDLHapticDevice::uploadEffect(SDLHapticEffect* effect)
{
   if (effect && effect->getEffectId() < 0)
   {
      S32 retVal = SDL_HapticNewEffect(mDevice, effect->getEffect());
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::uploadEffect() error: %s", SDL_GetError());
      effect->setEffectId(retVal);
      return retVal;
   }
   return -1;
}

bool SDLHapticDevice::unloadEffect(SDLHapticEffect* effect)
{
   if (effect && effect->getEffectId() > -1)
   {
      SDL_HapticDestroyEffect(mDevice, effect->getEffectId());
      effect->setEffectId(-1);
      return true;
   }
   return false;
}

S32 SDLHapticDevice::updateEffect(SDLHapticEffect* effect)
{
   if (effect && effect->getEffectId() > -1)
   {
      S32 retVal = SDL_HapticUpdateEffect(mDevice, effect->getEffectId(), effect->getEffect());
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::updateEffect() error: %s", SDL_GetError());
      return retVal;
   }
   return -1;
}

bool SDLHapticDevice::runEffect(SDLHapticEffect* effect, U32 iterations)
{
   if (effect && effect->getEffectId() > -1)
   {
      S32 retVal = SDL_HapticRunEffect(mDevice, effect->getEffectId(), iterations);
      if (retVal > -1)
         return true;

      Con::errorf("SDLHapticDevice::runEffect() error: %s", SDL_GetError());
   }
   return false;
}

S32 SDLHapticDevice::stopEffect(SDLHapticEffect* effect)
{
   if (effect && effect->getEffectId() > -1)
   {
      S32 retVal = SDL_HapticStopEffect(mDevice, effect->getEffectId());
      if (retVal < 0)
         Con::errorf("SDLHapticDevice::stopEffect() error: %s", SDL_GetError());
      return retVal;
   }
   return -1;
}

S32 SDLHapticDevice::getEffectStatus(S32 effectId)
{
   S32 retVal = SDL_HapticGetEffectStatus(mDevice, effectId);
   if (retVal < 0)
      Con::errorf("SDLHapticDevice::getEffectStatus() error: %s", SDL_GetError());
   return retVal;
}

//-----------------------------------------------------------------------------
DefineEngineMethod(SDLHapticDevice, getName, const char*, (), ,
   "@brief Use this function to get the implementation dependent name of a haptic device.\n\n"
   "@return Returns the name of the device or NULL on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticName \n"
   "@ingroup SDLHaptic")
{
   return object->getDeviceName();
}

DefineEngineMethod(SDLHapticDevice, getDeviceIndex, S32, (), ,
   "@brief Use this function to get the index of an open haptic device.\n\n"
   "@return Returns the index of the device or a negative error code on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticIndex \n"
   "@ingroup SDLHaptic")
{
   return object->getDeviceIndex();
}

DefineEngineMethod(SDLHapticDevice, query, S32, (), ,
   "@brief Use this function to get the haptic device's supported features in bitwise manner.\n\n"
   "@return Returns a list of supported haptic features in bitwise manner (OR'd), or 0 on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticQuery \n"
   "@ingroup SDLHaptic")
{
   return object->getDeviceCaps();
}

DefineEngineMethod(SDLHapticDevice, getGain, S32, (), ,
   "@brief Use this function to get the global gain of the device.\n\n"
   "@return Returns the current gain value.\n"
   "@ingroup SDLHaptic")
{
   return object->getGain();
}

DefineEngineMethod(SDLHapticDevice, getAutocenter, S32, (), ,
   "@brief Use this function to get the global autocenter of the device.\n\n"
   "@return Returns the current autocenter value.\n"
   "@ingroup SDLHaptic")
{
   return object->getAutocenter();
}

DefineEngineMethod(SDLHapticDevice, setGain, S32, (S32 gain), (100),
   "@brief Use this function to set the global gain of the device.\n\n"
   "@param gain Value to set the gain to, should be between 0 and 100.\n"
   "@return Returns 0 on success or a negative error code on failure. "
   "Device must support the SDL_HAPTIC_GAIN feature. "
   "The user may specify the maximum gain by setting the environment variable "
   "SDL_HAPTIC_GAIN_MAX which should be between 0 and 100. All calls to SDL_HapticSetGain() "
   "will scale linearly using SDL_HAPTIC_GAIN_MAX as the maximum.\n"
   "@see https://wiki.libsdl.org/SDL_HapticSetGain \n"
   "@ingroup SDLHaptic")
{
   return object->setGain(gain);
}

DefineEngineMethod(SDLHapticDevice, setAutocenter, S32, (S32 autocenter), (0),
   "@brief Use this function to set the global autocenter of the device.\n\n"
   "@param autocenter Value to set autocenter to (0-100), 0 disables autocentering.\n"
   "@return Returns 0 on success or a negative error code on failure. "
   "Device must support the SDL_HAPTIC_AUTOCENTER feature.\n"
   "@see https://wiki.libsdl.org/SDL_HapticSetAutocenter \n"
   "@ingroup SDLHaptic")
{
   return object->setAutocenter(autocenter);
}

DefineEngineMethod(SDLHapticDevice, rumbleSupported, bool, (), ,
   "@brief Use this function to check whether rumble is supported on the device.\n\n"
   "@return Returns true if rumble is supported.\n"
   "@see https://wiki.libsdl.org/SDL_HapticRumbleSupported \n"
   "@ingroup SDLHaptic")
{
   return object->rumbleSupported();
}

DefineEngineMethod(SDLHapticDevice, rumblePlay, void, (F32 strength, U32 length), (0.5, 2000),
   "@brief Use this function to run a simple rumble effect on the device.\n\n"
   "@param strength strength of the rumble to play as a 0-1 float value.\n"
   "@param length The length of the rumble to play in milliseconds.\n"
   "@return Returns true if the rumble effect was started.\n"
   "@see https://wiki.libsdl.org/SDL_HapticRumblePlay \n"
   "@ingroup SDLHaptic")
{
   object->rumblePlay(strength, length);
}

DefineEngineMethod(SDLHapticDevice, rumbleStop, bool, (), ,
   "@brief Use this function to stop the simple rumble on a haptic device.\n\n"
   "@return Returns true on success.\n"
   "@see https://wiki.libsdl.org/SDL_HapticRumbleStop \n"
   "@ingroup SDLHaptic")
{
   return object->rumbleStop();
}

DefineEngineMethod(SDLHapticDevice, numEffects, S32, (), ,
   "@brief Use this function to get the number of effects the device can store.\n\n"
   "@return Returns the number of effects the haptic device can store or a negative error code on failure. "
   "On some platforms this isn't fully supported, and therefore is an approximation. Always check to see if "
   "your created effect was actually created and do not rely solely on SDLHapticDevice::numEffects().\n"
   "@see https://wiki.libsdl.org/SDL_HapticNumEffects \n"
   "@ingroup SDLHaptic")
{
   return object->numEffects();
}

DefineEngineMethod(SDLHapticDevice, numEffectsPlaying, S32, (), ,
   "@brief Use this function to get the number of effects the device can play at the same time.\n\n"
   "@return Returns the number of effects the haptic device can play at the same time or a negative error code on failure. "
   "This is not supported on all platforms, but will always return a value. Added here for the sake of completeness.\n"
   "@see https://wiki.libsdl.org/SDL_HapticNumEffectsPlaying \n"
   "@ingroup SDLHaptic")
{
   return object->numEffectsPlaying();
}

DefineEngineMethod(SDLHapticDevice, numAxes, S32, (), ,
   "@brief Use this function to get the number of haptic axes the device has.\n\n"
   "@return Returns the number of axes on success or a negative error code on failure.\n"
   "@see https://wiki.libsdl.org/SDL_HapticNumAxes \n"
   "@ingroup SDLHaptic")
{
   return object->numAxes();
}

DefineEngineMethod(SDLHapticDevice, pause, S32, (), ,
   "@brief Use this function to pause the haptic device.\n\n"
   "Device must support the SDL_HAPTIC_PAUSE ($SDLTypeMasks::HapticPause) feature. "
   "Call SDL_HapticUnpause() (SDLHapticDevice::unpause()) to resume playback. "
   "Do not modify the effects nor add new ones while the device is paused. That can cause all sorts of weird errors.\n"
   "@return Returns 0 on success or -1 on error.\n"
   "@see https://wiki.libsdl.org/SDL_HapticPause \n"
   "@ingroup SDLHaptic")
{
   return object->pause();
}

DefineEngineMethod(SDLHapticDevice, unpause, S32, (), ,
   "@brief Use this function to unpause the haptic device.\n\n"
   "Device must support the SDL_HAPTIC_PAUSE ($SDLTypeMasks::HapticPause) feature. Call to unpause after SDL_HapticPause().\n"
   "@return Returns 0 on success or -1 on error.\n"
   "@see https://wiki.libsdl.org/SDL_HapticUnpause \n"
   "@ingroup SDLHaptic")
{
   return object->unpause();
}

DefineEngineMethod(SDLHapticDevice, stopAll, S32, (), ,
   "@brief Stops all the currently playing effects on the device.\n\n"
   "@return Returns 0 on success or -1 on error.\n"
   "@see https://wiki.libsdl.org/SDL_HapticStopAll \n"
   "@ingroup SDLHaptic")
{
   return object->stopAll();
}

DefineEngineMethod(SDLHapticDevice, getEffectCount, S32, (), ,
   "@brief Gets the total number of effects created for the device.\n\n"
   "@return Returns the number of effects or -1 on error.\n"
   "@ingroup SDLHaptic")
{
   return object->getEffectCount();
}

DefineEngineMethod(SDLHapticDevice, getEffect, SDLHapticEffect*, (S32 effect_index), (0),
   "@brief Gets a SDLHapticEffect from the device.\n\n"
   "@param effect_index The index of the effect to get. Between 0 and getEffectCount().\n"
   "@return Returns the effect or NULL on error.\n"
   "@ingroup SDLHaptic")
{
   return object->getEffect(effect_index);
}

DefineEngineMethod(SDLHapticDevice, createEffect, SDLHapticEffect*, (S32 effectMask), ,
   "@brief Creates a new effect object with the passed type.\n\n"
   "@param effectMask Identifies the type of effect to create.\n"
   "@return Returns the created effect or NULL on error.\n"
   "@ingroup SDLHaptic")
{
   return object->createEffect(effectMask);
}

DefineEngineMethod(SDLHapticDevice, removeEffect, bool, (SDLHapticEffect* effect), ,
   "@brief Removes the effect from the Sim and deletes the effect.\n\n"
   "If the effect has been uploaded to the device, it will be unloaded first. "
   "Any reference to the effect after this function returns will be invalid."
   "@param effect An SDLHapticEffect object that was created with createEffect().\n"
   "@return Returns true if the effect was deleted.\n"
   "@ingroup SDLHaptic")
{
   return object->removeEffect(effect);
}

DefineEngineMethod(SDLHapticDevice, uploadEffect, S32, (SDLHapticEffect* effect), ,
   "@brief Uploads an effect to the device so it can be run.\n\n"
   "@param effect An initialized SDLHapticEffect object.\n"
   "@return Returns the effect id for the device effect or -1 on error.\n"
   "@ingroup SDLHaptic")
{
   return object->uploadEffect(effect);
}

DefineEngineMethod(SDLHapticDevice, unloadEffect, bool, (SDLHapticEffect* effect), ,
   "@brief Unloads an effect from the device so it can be deleted or the slot can be used by a different effect.\n\n"
   "@param effect The effect object that was uploaded to the device.\n"
   "@return Returns true if the effect was unloaded or false on error.\n"
   "@ingroup SDLHaptic")
{
   return object->unloadEffect(effect);
}

DefineEngineMethod(SDLHapticDevice, updateEffect, S32, (SDLHapticEffect* effect), ,
   "@brief Updates the properties of an effect that has already been loaded into a device.\n\n"
   "Can be used dynamically, although behaviour when dynamically changing "
   "direction may be strange.Specifically the effect may reupload itself "
   "and start playing from the start. You cannot change the type either when "
   "running updateEffect()\n"
   "@param effect The effect object that was uploaded to the device.\n"
   "@return Returns 0 on success or -1 on error.\n"
   "@ingroup SDLHaptic")
{
   return object->updateEffect(effect);
}

DefineEngineMethod(SDLHapticDevice, runEffect, bool, (SDLHapticEffect* effect, S32 iterations), ,
   "@brief Runs the haptic effect.\n\n"
   "If iterations are -1, it'll run the effect over and over repeating the "
   "envelope(attack and fade) every time.\n"
   "@param effect The effect object that was uploaded to the device.\n"
   "@param iterations Number of iterations to run the effect. Use -1 for infinity.\n"
   "@return Returns true on success or false on error.\n"
   "@ingroup SDLHaptic")
{
   U32 loopCount = (iterations > 0) ? (U32)iterations : SDL_HAPTIC_INFINITY;
   return object->runEffect(effect, loopCount);
}

DefineEngineMethod(SDLHapticDevice, stopEffect, S32, (SDLHapticEffect* effect), ,
   "@brief Stops the haptic effect.\n\n"
   "@param effect The SDLHapticEffect object to stop.\n"
   "@return Returns 0 on success or -1 on error.\n"
   "@ingroup SDLHaptic")
{
   return object->stopEffect(effect);
}

DefineEngineMethod(SDLHapticDevice, getEffectStatus, S32, (S32 effectId), ,
   "@brief Use this function to get the status of the effect.\n\n"
   "The device must support the SDL_HAPTIC_STATUS ($SDLTypeMasks::HapticStatus) feature or this command will fail.\n"
   "@param effectId The identifier of the haptic effect to query its status.\n"
   "@return Returns 0 if it isn't playing, 1 if it is playing, or a negative error code on failure.\n"
   "@ingroup SDLHaptic")
{
   return object->getEffectStatus(effectId);
}
