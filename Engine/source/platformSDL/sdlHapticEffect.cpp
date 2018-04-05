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
#include "math/mPoint3.h"
#include "math/mPoint4.h"
#include "sdlHapticEffect.h"

IMPLEMENT_CONOBJECT(SDLHapticEffect);

ImplementEnumType(SDLEffectType,
   "\n\n"
   "@ingroup SDLHaptic")
{ SDLHapticEffect::ConstantEffect, "Constant" },
{ SDLHapticEffect::SineEffect, "Sine" },
{ SDLHapticEffect::LeftRightEffect, "LeftRight" },
{ SDLHapticEffect::TriangleEffect, "Triangle" },
{ SDLHapticEffect::SawUpEffect, "SawToothUp" },
{ SDLHapticEffect::SawDownEffect, "SawToothDown" },
{ SDLHapticEffect::RampEffect, "Ramp" },
{ SDLHapticEffect::SpringEffect, "Spring" },
{ SDLHapticEffect::DamperEffect, "Damper" },
{ SDLHapticEffect::InertiaEffect, "Inertia" },
{ SDLHapticEffect::FrictionEffect, "Friction" },
{ SDLHapticEffect::CustomEffect, "Custom" },
EndImplementEnumType;

ImplementEnumType(SDLCoordinateType,
   "\n\n"
   "@ingroup SDLHaptic")
{ SDLHapticEffect::Polar, "Polar" },
{ SDLHapticEffect::Cartesian, "Cartesian" },
{ SDLHapticEffect::Spherical, "Spherical" },
EndImplementEnumType;

SDLHapticEffect::SDLHapticEffect()
{
   mEffectId = -1;
   dMemset(&mEffect, 0, sizeof(SDL_HapticEffect));
}

SDLHapticEffect::~SDLHapticEffect()
{
}

bool SDLHapticEffect::initEffect(EffectType effectMask)
{
   dMemset(&mEffect, 0, sizeof(SDL_HapticEffect));
   mEffect.type = effectMask;

   // Setup some 'reasonable' default values for the chosen effect type.
   switch (effectMask)
   {
   case ConstantEffect:
      mEffect.constant.type = ConstantEffect;
      mEffect.constant.direction.type = SDL_HAPTIC_POLAR;
      mEffect.constant.direction.dir[0] = 18000;

      mEffect.constant.length = 3000;
      mEffect.constant.delay = 0;

      mEffect.constant.button = 1;
      mEffect.constant.interval = 500;

      mEffect.constant.attack_length = 500; 
      mEffect.constant.attack_level = 0x4000; // 65535 * 0.25...25% strength
      mEffect.constant.fade_length = 500;
      mEffect.constant.fade_level = 0x4000; // 65535 * 0.25...25% strength

      mEffect.constant.level = 26214;  // 32767 * 0.8...80% strength
      break;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      mEffect.periodic.type = effectMask;
      mEffect.periodic.direction.type = SDL_HAPTIC_POLAR;
      mEffect.periodic.direction.dir[0] = 18000;

      mEffect.periodic.length = 3000;
      mEffect.periodic.delay = 0;

      mEffect.periodic.button = 1;
      mEffect.periodic.interval = 500;

      mEffect.periodic.attack_length = 500;
      mEffect.periodic.attack_level = 0x4000; // 65535 * 0.25...25% strength
      mEffect.periodic.fade_length = 500;
      mEffect.periodic.fade_level = 0x4000; // 65535 * 0.25...25% strength

      mEffect.periodic.period = 200;  // 5 cycles per second
      mEffect.periodic.magnitude = 26214;  // 32767 * 0.8...80% strength
      mEffect.periodic.offset = 0x4000;  // 32767 * 0.5...50% strength
      mEffect.periodic.phase = 0;  // Phase shift
      break;

   case SpringEffect:
   case DamperEffect:
   case InertiaEffect:
   case FrictionEffect:
      mEffect.condition.type = effectMask;

      mEffect.condition.length = 3000;
      mEffect.condition.delay = 0;

      mEffect.condition.button = 1;
      mEffect.condition.interval = 500;

      for (S32 i = 0; i < 3; ++i)
      {
         mEffect.condition.right_sat[i] = 0x7FFF;
         mEffect.condition.left_sat[i] = 0x7FFF;
         mEffect.condition.right_coeff[i] = 100;
         mEffect.condition.left_coeff[i] = 100;
         mEffect.condition.deadband[i] = 6553; // 32767 * 0.2...20% deadzone
      }
      break;

   case RampEffect:
      mEffect.ramp.type = RampEffect;
      mEffect.ramp.direction.type = SDL_HAPTIC_POLAR;
      mEffect.ramp.direction.dir[0] = 18000;

      mEffect.ramp.length = 3000;
      mEffect.ramp.delay = 0;

      mEffect.ramp.button = 1;
      mEffect.ramp.interval = 500;

      mEffect.ramp.start = 1;
      mEffect.ramp.end = 32767;
      break;

   case LeftRightEffect:
      mEffect.leftright.type = LeftRightEffect;

      mEffect.leftright.length = 3000;

      mEffect.leftright.large_magnitude = 0xB000;  // 65535 * 0.75...75% strength
      mEffect.leftright.small_magnitude = 0x4000; // 65535 * 0.25...25% strength
      break;

   case CustomEffect:
      mEffect.custom.type = CustomEffect;
      mEffect.custom.direction.type = SDL_HAPTIC_POLAR;
      mEffect.custom.direction.dir[0] = 18000;

      mEffect.custom.length = 3000;
      mEffect.custom.delay = 0;

      mEffect.custom.button = 1;
      mEffect.custom.interval = 500;

      mEffect.custom.attack_length = 500;
      mEffect.custom.attack_level = 0x4000; // 65535 * 0.25...25% strength
      mEffect.custom.fade_length = 500;
      mEffect.custom.fade_level = 0x4000; // 65535 * 0.25...25% strength

      // Channels, period, samples and data must be defined for custom effects. See SDL_HapticCustom
      break;

   default:
      Con::errorf("SDLHapticEffect::initEffect Unknown effect type: 0x%04x", effectMask);
      return false;
   }
   return true;
}

bool SDLHapticEffect::hasDirection()
{
   switch (mEffect.type)
   {
   case ConstantEffect:
   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
   case RampEffect:
   case CustomEffect:
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::getDirectionCoordType(S32 &coordType)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      coordType = mEffect.constant.direction.type;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      coordType = mEffect.periodic.direction.type;
      return true;

   case RampEffect:
      coordType = mEffect.ramp.direction.type;
      return true;

   case CustomEffect:
      coordType = mEffect.custom.direction.type;
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::setDirectionCoordType(S32 coordType)
{
   if (coordType < SDL_HAPTIC_POLAR || coordType > SDL_HAPTIC_SPHERICAL)
   {
      Con::errorf("SDLHapticEffect::setDirectionCoordType() invalid coordinate type.");
      return false;
   }

   switch (mEffect.type)
   {
   case ConstantEffect:
      mEffect.constant.direction.type = coordType;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      mEffect.periodic.direction.type = coordType;
      return true;

   case RampEffect:
      mEffect.ramp.direction.type = coordType;
      return true;

   case CustomEffect:
      mEffect.custom.direction.type = coordType;
      return true;

   default:
      Con::errorf("SDLHapticEffect::setDirectionCoordType() effect does not support direction data.");
      break;
   }
   return false;
}

bool SDLHapticEffect::getDirectionCoordData(Point3I &coordData)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      coordData.set(mEffect.constant.direction.dir[0], mEffect.constant.direction.dir[1], mEffect.constant.direction.dir[2]);
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      coordData.set(mEffect.periodic.direction.dir[0], mEffect.periodic.direction.dir[1], mEffect.periodic.direction.dir[2]);
      return true;

   case RampEffect:
      coordData.set(mEffect.ramp.direction.dir[0], mEffect.ramp.direction.dir[1], mEffect.ramp.direction.dir[2]);
      return true;

   case CustomEffect:
      coordData.set(mEffect.custom.direction.dir[0], mEffect.custom.direction.dir[1], mEffect.custom.direction.dir[2]);
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::setDirectionCoordData(Point3I coordData)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      mEffect.constant.direction.dir[0] = coordData.x;
      mEffect.constant.direction.dir[1] = coordData.y;
      mEffect.constant.direction.dir[2] = coordData.z;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      mEffect.periodic.direction.dir[0] = coordData.x;
      mEffect.periodic.direction.dir[1] = coordData.y;
      mEffect.periodic.direction.dir[2] = coordData.z;
      return true;

   case RampEffect:
      mEffect.ramp.direction.dir[0] = coordData.x;
      mEffect.ramp.direction.dir[1] = coordData.y;
      mEffect.ramp.direction.dir[2] = coordData.z;
      return true;

   case CustomEffect:
      mEffect.custom.direction.dir[0] = coordData.x;
      mEffect.custom.direction.dir[1] = coordData.y;
      mEffect.custom.direction.dir[2] = coordData.z;
      return true;

   default:
      Con::errorf("SDLHapticEffect::setDirectionCoordData() effect does not support direction data.");
      break;
   }
   return false;
}

bool SDLHapticEffect::hasEnvelope()
{
   switch (mEffect.type)
   {
   case ConstantEffect:
   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
   case RampEffect:
   case CustomEffect:
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::getEnvelopeData(Point4I &envData)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      envData.x = mEffect.constant.attack_length;
      envData.y = mEffect.constant.attack_level;
      envData.z = mEffect.constant.fade_length;
      envData.w = mEffect.constant.fade_level;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      envData.x = mEffect.periodic.attack_length;
      envData.y = mEffect.periodic.attack_level;
      envData.z = mEffect.periodic.fade_length;
      envData.w = mEffect.periodic.fade_level;
      return true;

   case RampEffect:
      envData.x = mEffect.ramp.attack_length;
      envData.y = mEffect.ramp.attack_level;
      envData.z = mEffect.ramp.fade_length;
      envData.w = mEffect.ramp.fade_level;
      return true;

   case CustomEffect:
      envData.x = mEffect.custom.attack_length;
      envData.y = mEffect.custom.attack_level;
      envData.z = mEffect.custom.fade_length;
      envData.w = mEffect.custom.fade_level;
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::setEnvelopeData(S32 attack_length, S32 attack_level, S32 fade_length, S32 fade_level)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      mEffect.constant.attack_length = attack_length;
      mEffect.constant.attack_level = attack_level;
      mEffect.constant.fade_length = fade_length;
      mEffect.constant.fade_level = fade_level;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      mEffect.periodic.attack_length = attack_length;
      mEffect.periodic.attack_level = attack_level;
      mEffect.periodic.fade_length = fade_length;
      mEffect.periodic.fade_level = fade_level;
      return true;

   case RampEffect:
      mEffect.ramp.attack_length = attack_length;
      mEffect.ramp.attack_level = attack_level;
      mEffect.ramp.fade_length = fade_length;
      mEffect.ramp.fade_level = fade_level;
      return true;

   case CustomEffect:
      mEffect.custom.attack_length = attack_length;
      mEffect.custom.attack_level = attack_level;
      mEffect.custom.fade_length = fade_length;
      mEffect.custom.fade_level = fade_level;
      return true;

   default:
      Con::errorf("SDLHapticEffect::setEnvelopeData() effect does not support attack/fade settings data.");
      break;
   }
   return false;
}

bool SDLHapticEffect::getReplayData(Point2I &replayData)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      replayData.x = mEffect.constant.length;
      replayData.y = mEffect.constant.delay;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      replayData.x = mEffect.periodic.length;
      replayData.y = mEffect.periodic.delay;
      return true;

   case SpringEffect:
   case DamperEffect:
   case InertiaEffect:
   case FrictionEffect:
      replayData.x = mEffect.condition.length;
      replayData.y = mEffect.condition.delay;
      return true;

   case LeftRightEffect:
      replayData.x = mEffect.leftright.length;
      replayData.y = 0; // SDL_HapticLeftRight effect does not have a delay field
      return true;

   case RampEffect:
      replayData.x = mEffect.ramp.length;
      replayData.y = mEffect.ramp.delay;
      return true;

   case CustomEffect:
      replayData.x = mEffect.custom.length;
      replayData.y = mEffect.custom.delay;
      return true;

   default:
      break;
   }
   return false;
}

bool SDLHapticEffect::setReplaydata(S32 length, S32 delay)
{
   switch (mEffect.type)
   {
   case ConstantEffect:
      mEffect.constant.length = length;
      mEffect.constant.delay = delay;
      return true;

   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      mEffect.periodic.length = length;
      mEffect.periodic.delay = delay;
      return true;

   case SpringEffect:
   case DamperEffect:
   case InertiaEffect:
   case FrictionEffect:
      mEffect.condition.length = length;
      mEffect.condition.delay = delay;
      return true;

   case LeftRightEffect:
      mEffect.leftright.length = length;
      return true;

   case RampEffect:
      mEffect.ramp.length = length;
      mEffect.ramp.delay = delay;
      return true;

   case CustomEffect:
      mEffect.custom.length = length;
      mEffect.custom.delay = delay;
      return true;

   default:
      Con::errorf("SDLHapticEffect::setEnvelopeData() effect does not support attack/fade settings data.");
      break;
   }
   return false;
}

bool SDLHapticEffect::getConstantEffectData(F32 &level)
{
   if (mEffect.type != ConstantEffect)
   {
      Con::errorf("SDLHapticEffect::getConstantEffectData() - wrong effect type.");
      return false;
   }

   level = mEffect.constant.level;
   return true;
}

bool SDLHapticEffect::setConstantEffectData(F32 level)
{
   if (mEffect.type != ConstantEffect)
   {
      Con::errorf("SDLHapticEffect::setConstantEffectData() - wrong effect type.");
      return false;
   }

   mEffect.constant.level = level;
   return true;
}

bool SDLHapticEffect::isPeriodicEffect()
{
   switch (mEffect.type)
   {
   case SineEffect:
   case TriangleEffect:
   case SawUpEffect:
   case SawDownEffect:
      return true;
   default:
      return false;
   }
}

bool SDLHapticEffect::getPeriodicEffectData(Point4I &periodicData)
{
   if (!isPeriodicEffect())
   {
      Con::errorf("SDLHapticEffect::getPeriodicEffectData() - wrong effect type.");
      return false;
   }

   periodicData.x = mEffect.periodic.period;
   periodicData.y = mEffect.periodic.magnitude;
   periodicData.z = mEffect.periodic.offset;
   periodicData.w = mEffect.periodic.phase;
   return true;
}

bool SDLHapticEffect::setPeriodicEffectData(S32 period, S32 magnitude, S32 offset, S32 phase)
{
   if (!isPeriodicEffect())
   {
      Con::errorf("SDLHapticEffect::getPeriodicEffectData() - wrong effect type.");
      return false;
   }

   while (phase < 0)
      phase += 36000;
   while (phase > 36000)
      phase -= 36000;

   mEffect.periodic.period = period;
   mEffect.periodic.magnitude = magnitude;
   mEffect.periodic.offset = offset;
   mEffect.periodic.phase = phase;
   return true;
}

bool SDLHapticEffect::getLeftRightEffectData(Point2I &leftRightData)
{
   if (mEffect.type != LeftRightEffect)
   {
      Con::errorf("SDLHapticEffect::getLeftRightEffectData() - wrong effect type.");
      return false;
   }

   leftRightData.x = mEffect.leftright.large_magnitude;
   leftRightData.y = mEffect.leftright.small_magnitude;
   return true;
}

bool SDLHapticEffect::setLeftRightEffectdata(S32 large_magnitude, S32 small_magnitude)
{
   if (mEffect.type != LeftRightEffect)
   {
      Con::errorf("SDLHapticEffect::setLeftRightEffectData() - wrong effect type.");
      return false;
   }

   mEffect.leftright.large_magnitude = large_magnitude;
   mEffect.leftright.small_magnitude = small_magnitude;
   return true;
}

bool SDLHapticEffect::getRampEffectData(Point2I &rampData)
{
   if (mEffect.type != RampEffect)
   {
      Con::errorf("SDLHapticEffect::getRampEffectData() - wrong effect type.");
      return false;
   }

   rampData.x = mEffect.ramp.start;
   rampData.y = mEffect.ramp.end;
   return true;
}

bool SDLHapticEffect::setRampEffectdata(S32 ramp_start, S32 ramp_end)
{
   if (mEffect.type != RampEffect)
   {
      Con::errorf("SDLHapticEffect::setRampEffectdata() - wrong effect type.");
      return false;
   }

   mEffect.ramp.start = ramp_start;
   mEffect.ramp.end = ramp_end;
   return true;
}

bool SDLHapticEffect::isConditionEffect()
{
   switch (mEffect.type)
   {
   case SpringEffect:
   case DamperEffect:
   case InertiaEffect:
   case FrictionEffect:
      return true;
   default:
      return false;
   }
}

bool SDLHapticEffect::getConditionEffectRSat(Point3I &satData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectRSat() - wrong effect type.");
      return false;
   }

   satData.x = mEffect.condition.right_sat[0];
   satData.y = mEffect.condition.right_sat[1];
   satData.z = mEffect.condition.right_sat[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectRSat(Point3I satData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectRSat() - wrong effect type.");
      return false;
   }

   mEffect.condition.right_sat[0] = satData.x;
   mEffect.condition.right_sat[1] = satData.y;
   mEffect.condition.right_sat[2] = satData.z;
   return true;
}

bool SDLHapticEffect::getConditionEffectLSat(Point3I &satData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectLSat() - wrong effect type.");
      return false;
   }

   satData.x = mEffect.condition.left_sat[0];
   satData.y = mEffect.condition.left_sat[1];
   satData.z = mEffect.condition.left_sat[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectLSat(Point3I satData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectLSat() - wrong effect type.");
      return false;
   }

   mEffect.condition.left_sat[0] = satData.x;
   mEffect.condition.left_sat[1] = satData.y;
   mEffect.condition.left_sat[2] = satData.z;
   return true;
}

bool SDLHapticEffect::getConditionEffectRCoeff(Point3I &coeffData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectRCoeff() - wrong effect type.");
      return false;
   }

   coeffData.x = mEffect.condition.right_coeff[0];
   coeffData.y = mEffect.condition.right_coeff[1];
   coeffData.z = mEffect.condition.right_coeff[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectRCoeff(Point3I coeffData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectRCoeff() - wrong effect type.");
      return false;
   }

   mEffect.condition.right_coeff[0] = coeffData.x;
   mEffect.condition.right_coeff[1] = coeffData.y;
   mEffect.condition.right_coeff[2] = coeffData.z;
   return true;
}

bool SDLHapticEffect::getConditionEffectLCoeff(Point3I &coeffData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectLCoeff() - wrong effect type.");
      return false;
   }

   coeffData.x = mEffect.condition.left_coeff[0];
   coeffData.y = mEffect.condition.left_coeff[1];
   coeffData.z = mEffect.condition.left_coeff[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectLCoeff(Point3I coeffData)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectLCoeff() - wrong effect type.");
      return false;
   }

   mEffect.condition.left_coeff[0] = coeffData.x;
   mEffect.condition.left_coeff[1] = coeffData.y;
   mEffect.condition.left_coeff[2] = coeffData.z;
   return true;
}

bool SDLHapticEffect::getConditionEffectDeadzone(Point3I &deadband)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectDeadzone() - wrong effect type.");
      return false;
   }

   deadband.x = mEffect.condition.deadband[0];
   deadband.y = mEffect.condition.deadband[1];
   deadband.z = mEffect.condition.deadband[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectDeadzone(Point3I deadband)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectDeadzone() - wrong effect type.");
      return false;
   }

   mEffect.condition.deadband[0] = deadband.x;
   mEffect.condition.deadband[1] = deadband.y;
   mEffect.condition.deadband[2] = deadband.z;
   return true;
}

bool SDLHapticEffect::getConditionEffectCenter(Point3I &centers)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::getConditionEffectCenter() - wrong effect type.");
      return false;
   }

   centers.x = mEffect.condition.center[0];
   centers.y = mEffect.condition.center[1];
   centers.z = mEffect.condition.center[2];
   return true;
}

bool SDLHapticEffect::setConditionEffectCenter(Point3I centers)
{
   if (!isConditionEffect())
   {
      Con::errorf("SDLHapticEffect::setConditionEffectCenter() - wrong effect type.");
      return false;
   }

   mEffect.condition.center[0] = centers.x;
   mEffect.condition.center[1] = centers.y;
   mEffect.condition.center[2] = centers.z;
   return true;
}


//-----------------------------------------------------------------------------
DefineEngineMethod(SDLHapticEffect, getEffectType, SDLEffectType, (), ,
   "@brief Gets the type of this effect.\n\n"
   "@return Returns The type or an empty string on error.\n"
   "@ingroup SDLHaptic")
{
   return (SDLEffectType) object->getEffectType();
}

DefineEngineMethod(SDLHapticEffect, getEffectId, S32, (), ,
   "@brief Gets the id assigned to this effect.\n\n"
   "The id will be -1 if the effect is not loaded into a device. The effect must be "
   "uploaded to a device and an id assigned before the effect can be run."
   "@return Returns the currently assigned id.\n"
   "@ingroup SDLHaptic")
{
   return object->getEffectId();
}

DefineEngineMethod(SDLHapticEffect, hasDirection, bool, (), ,
   "@return Returns true if the current effect uses direction data.\n\n"
   "@ingroup SDLHaptic")
{
   return object->hasDirection();
}

DefineEngineMethod(SDLHapticEffect, getDirectionCoordType, SDLCoordinateType, (), ,
   "@brief Gets the coordinate system used by this effect.\n\n"
   "@return Returns Polar, Cartesian or Spherical.\n\n"
   "@ingroup SDLHaptic")
{
   S32 coordType = 0;
   object->getDirectionCoordType(coordType);
   return (SDLCoordinateType) coordType;
}

DefineEngineMethod(SDLHapticEffect, setDirectionCoordType, bool, (SDLCoordinateType coordType), ,
   "@brief Sets the coordinate system used by this effect.\n\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setDirectionCoordType((S32) coordType);
}

DefineEngineMethod(SDLHapticEffect, getDirectionCoordData, Point3I, (), ,
   "@brief Gets the direction coordinates for this effect.\n\n"
   "@ingroup SDLHaptic")
{
   Point3I coordData = Point3I::Zero;
   object->getDirectionCoordData(coordData);
   return coordData;
}

DefineEngineMethod(SDLHapticEffect, setDirectionCoordData, bool, (Point3I coordData), ,
   "@brief Sets the direction coordinates for this effect.\n\n"
   "@param coordData The coordinate data. Only the X parameter is used with polar coordinates. "
   "X and Y can be used with spherical. All three can be used with cartesian.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setDirectionCoordData(coordData);
}

DefineEngineMethod(SDLHapticEffect, hasEnvelope, bool, (), ,
   "@return Returns true if the current effect uses attack/fade envelope data.\n\n"
   "@ingroup SDLHaptic")
{
   return object->hasEnvelope();
}

DefineEngineMethod(SDLHapticEffect, getEnvelopeData, String, (), ,
   "@brief Gets the envelope data for this effect.\n\n"
   "@return Returns The four envelope fields packed as words in a space separated string.\n"
   "attack_length attack_level fade_length fade_level.\n"
   "@ingroup SDLHaptic")
{
   String returnString;
   Point4I envData = Point4I::Zero;
   if (object->getEnvelopeData(envData))
   {
      returnString = String::ToString("%d %d %d %d", envData.x, envData.y, envData.z, envData.w);
   }
   return returnString;
}

DefineEngineMethod(SDLHapticEffect, setEnvelopeData, bool, (S32 attack_length, S32 attack_level, S32 fade_length, S32 fade_level), ,
   "@brief Sets the envelope data for this effect.\n\n"
   "@param attack_length Duration of the attack phase in milliseconds.\n"
   "@param attack_level Starting level for the attack phase (0 to 32767).\n"
   "@param fade_length Duration of the fade phase in milliseconds.\n"
   "@param fade_level Ending level for the fade phase (0 to 32767).\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setEnvelopeData(attack_length, attack_level, fade_length, fade_level);
}

DefineEngineMethod(SDLHapticEffect, getReplayData, Point2I, (), ,
   "@brief Gets the length and delay settings for this effect.\n\n"
   "@return Returns The length and delay values in ms packed as words in a space separated string.\n"
   "@ingroup SDLHaptic")
{
   Point2I replayData = Point2I::Zero;
   object->getReplayData(replayData);
   return replayData;
}

DefineEngineMethod(SDLHapticEffect, setReplaydata, bool, (S32 length, S32 delay), (0),
   "@brief Sets the length and delay values for this effect.\n\n"
   "@param length Total duration of the effect in milliseconds. Must be >= attack_length + fade_Length if an attack/fade envelope is used.\n"
   "@param delay Delay from the start of the effect until the attack phase begins in milliseconds.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setReplaydata(length, delay);
}

DefineEngineMethod(SDLHapticEffect, isPeriodicEffect, bool, (), ,
   "@brief Returns true if this is one of the periodic effect types: sine, sawtooth or triangle.\n\n"
   "@ingroup SDLHaptic")
{
   return object->isPeriodicEffect();
}

DefineEngineMethod(SDLHapticEffect, isConditionEffect, bool, (), ,
   "@brief Returns true if this is one of the condition effect types: spring, damper, inertia or friction.\n\n"
   "@ingroup SDLHaptic")
{
   return object->isConditionEffect();
}

DefineEngineMethod(SDLHapticEffect, getConstantEffectData, F32, (), ,
   "@brief Gets the effect level for a constant effect.\n\n"
   "@ingroup SDLHaptic")
{
   F32 level = 0;
   object->getConstantEffectData(level);
   return level;
}

DefineEngineMethod(SDLHapticEffect, setConstantEffectData, bool, (F32 level), ,
   "@brief Sets the effect level for a constant effect.\n\n"
   "@param level The new level value.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConstantEffectData(level);
}

DefineEngineMethod(SDLHapticEffect, getPeriodicEffectData, String, (), ,
   "@brief Gets the settings values for a periodic effect.\n\n"
   "@return Returns The four fields packed as words in a space separated string.\n"
   "period magnitude offset phase.\n"
   "@ingroup SDLHaptic")
{
   String returnString;
   Point4I envData = Point4I::Zero;
   if (object->getPeriodicEffectData(envData))
   {
      returnString = String::ToString("%d %d %d %d", envData.x, envData.y, envData.z, envData.w);
   }
   return returnString;
}

DefineEngineMethod(SDLHapticEffect, setPeriodicEffectData, bool, (S32 period, S32 magnitude, S32 offset, S32 phase), ,
   "@brief Sets the envelope data for this effect.\n\n"
   "@param period Period of the wave in milliseconds.\n"
   "@param magnitude Peak value; if negative, equivalent to 180 degrees extra phase shift (-32768 to 32767).\n"
   "@param offset Mean value of the wave (-32768 to 32767).\n"
   "@param phase Positive phase shift given by hundredth of a degree (0 to 36000).\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setPeriodicEffectData(period, magnitude, offset, phase);
}

DefineEngineMethod(SDLHapticEffect, getLeftRightEffectData, Point2I, (), ,
   "@brief Gets the large and small motor magnitude settings for this effect.\n\n"
   "@return Returns large_magnitude and small_magnitude as words in a space separated string.\n"
   "@ingroup SDLHaptic")
{
   Point2I lrData = Point2I::Zero;
   object->getLeftRightEffectData(lrData);
   return lrData;
}

DefineEngineMethod(SDLHapticEffect, setLeftRightEffectdata, bool, (S32 large_magnitude, S32 small_magnitude), ,
   "@brief Sets the motor magnitude values for this effect.\n\n"
   "@param large_magnitude Control of the large controller motor.\n"
   "@param small_magnitude Control of the small controller motor.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setLeftRightEffectdata(large_magnitude, small_magnitude);
}

DefineEngineMethod(SDLHapticEffect, getRampEffectData, Point2I, (), ,
   "@brief Gets the start and end levels for this ramp effect.\n\n"
   "@return Returns the start and end values as words in a space separated string.\n"
   "@ingroup SDLHaptic")
{
   Point2I rampData = Point2I::Zero;
   object->getRampEffectData(rampData);
   return rampData;
}

DefineEngineMethod(SDLHapticEffect, setRampEffectdata, bool, (S32 ramp_start, S32 ramp_end), ,
   "@brief Sets the motor magnitude values for this effect.\n\n"
   "@param ramp_start Beginning strength level.\n"
   "@param ramp_end Ending strength level.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setRampEffectdata(ramp_start, ramp_end);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectRSat, Point3I, (), ,
   "@brief Gets the right (positive) saturation data for this effect.\n\n"
   "Controls the effect level when joystick is to the positive side.\n"
   "@return Returns the right_sat value for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I satData = Point3I::Zero;
   object->getConditionEffectRSat(satData);
   return satData;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectRSat, bool, (Point3I right_sat), ,
   "@brief Sets the right (positive) saturation data for this effect.\n\n"
   "@param right_sat Positive saturation value for all 3 axes as Point3I. "
   "Controls the effect level when joystick is to the positive side.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectRSat(right_sat);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectLSat, Point3I, (), ,
   "@brief Gets the left (negative) saturation data for this effect.\n\n"
   "Controls the effect level when joystick is to the negative side.\n"
   "@return Returns the right_sat value for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I satData = Point3I::Zero;
   object->getConditionEffectLSat(satData);
   return satData;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectLSat, bool, (Point3I left_sat), ,
   "@brief Sets the left (negative) saturation data for this effect.\n\n"
   "@param left_sat Negative saturation value for all 3 axes as Point3I. "
   "Controls the effect level when joystick is to the negative side.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectLSat(left_sat);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectRCoeff, Point3I, (), ,
   "@brief Gets the right (positive) coefficient data for this effect.\n\n"
   "Controls how fast to increase the force towards the positive side\n"
   "@return Returns the right_sat value for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I coeffData = Point3I::Zero;
   object->getConditionEffectRCoeff(coeffData);
   return coeffData;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectRCoeff, bool, (Point3I right_coeff), ,
   "@brief Sets the right (positive) coefficient data for this effect.\n\n"
   "@param right_coeff Positive coefficient value for all 3 axes as Point3I. "
   "Controls how fast to increase the force towards the positive side\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectRCoeff(right_coeff);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectLCoeff, Point3I, (), ,
   "@brief Gets the left (negative) coefficient data for this effect.\n\n"
   "Controls how fast to increase the force towards the positive side\n"
   "@return Returns the right_sat value for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I coeffData = Point3I::Zero;
   object->getConditionEffectLCoeff(coeffData);
   return coeffData;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectLCoeff, bool, (Point3I left_coeff), ,
   "@brief Sets the left (negative) coefficient data for this effect.\n\n"
   "@param left_coeff Negative coefficient value for all 3 axes as Point3I. "
   "Controls how fast to increase the force towards the negative side\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectLCoeff(left_coeff);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectDeadzone, Point3I, (), ,
   "@brief Gets the deadzone for this effect.\n\n"
   "@return Returns the deadzone value for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I deadzone = Point3I::Zero;
   object->getConditionEffectDeadzone(deadzone);
   return deadzone;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectDeadzone, bool, (Point3I deadzone), ,
   "@brief Sets the deadzone for this effect.\n\n"
   "@param deadzone Deadzone value for all 3 axes as Point3I.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectDeadzone(deadzone);
}

DefineEngineMethod(SDLHapticEffect, getConditionEffectCenter, Point3I, (), ,
   "@brief Gets the position of the dead zone for this effect.\n\n"
   "@return Returns the position of the dead zone for all 3 axes in a Point3I.\n"
   "@ingroup SDLHaptic")
{
   Point3I center = Point3I::Zero;
   object->getConditionEffectCenter(center);
   return center;
}

DefineEngineMethod(SDLHapticEffect, setConditionEffectCenter, bool, (Point3I center), ,
   "@brief Sets the position of the dead zone (axis center) for this effect.\n\n"
   "@param deadzone Position of the dead zone for all 3 axes as Point3I.\n"
   "@return Returns true if the change was applied, false on error.\n\n"
   "@ingroup SDLHaptic")
{
   return object->setConditionEffectCenter(center);
}

DefineEngineMethod(SDLHapticEffect, convertCoords, Point3I, (S32 fromType, S32 toType, Point3I oldCoords), ,
   "@brief Convert coordinate data from one system to another.\n\n"
   "@param fromType The coordinate system data is being converted from.\n"
   "@param toType The coordinate system data is being converted to.\n"
   "@param oldCoords The raw coordinate data to convert.\n"
   "@return Returns the equivalent direction in the new system.\n\n"
   "@ingroup SDLHaptic")
{
   Point3I coordData = Point3I::Zero;
   //coordData = oldCoords;

   if (fromType == toType)
      coordData = oldCoords;
   else
   {
      Point3F fCoords;
      fCoords.x = (F32)oldCoords.x;
      fCoords.y = (F32)oldCoords.y;
      fCoords.z = (F32)oldCoords.z;

      if (toType == SDL_HAPTIC_CARTESIAN)
      {
         if (fromType == SDL_HAPTIC_SPHERICAL)
         {
            fCoords.x += 9000;
            if (coordData.x > 36000)
               coordData.x -= 36000;

            fCoords.y = mDegToRad(fCoords.y / 100.0f);
            coordData.z = (S32)(mSin(fCoords.y) * 100.0f * M_SQRT2_F);
         }
         fCoords.x = mDegToRad(fCoords.x / 100.0f);
         coordData.x = (S32)(mSin(fCoords.x) * 100.0f);
         coordData.y = -1 * (S32)(mCos(fCoords.x) * 100.0f);
      }
      else
      {
         if (fromType == SDL_HAPTIC_CARTESIAN)
         {
            fCoords.normalizeSafe();
            fCoords.y *= -1.0f;
            F32 rotAngle = mAtan2(fCoords.x, fCoords.y);
            coordData.x = (S32)mRadToDeg(rotAngle * 100.0f);
            if (coordData.x < 0)
               coordData.x += 36000;
            if (toType == SDL_HAPTIC_SPHERICAL)
            {
               coordData.x -= 9000;
               if (coordData.x < 0)
                  coordData.x += 36000;
               if (oldCoords.z != 0)
               {
                  F32 elevAngle = mAtan2(fCoords.z, mSqrt(fCoords.x * fCoords.x + fCoords.y * fCoords.y));
                  coordData.y = (S32)mRadToDeg(elevAngle * 100.0f);
               }
            }
         }
         else
         {
            coordData.x = oldCoords.x;
            if (toType == SDL_HAPTIC_SPHERICAL)
            {
               coordData.x -= 9000;
               if (coordData.x < 0)
                  coordData.x += 36000;
            }
            else
            {
               coordData.x += 9000;
               if (coordData.x > 36000)
                  coordData.x -= 36000;
            }
         }
      }
   }

   return coordData;
}
