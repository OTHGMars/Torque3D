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

#ifndef _SDLHAPTICEFFECT_H_
#define _SDLHAPTICEFFECT_H_

#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif
#include "SDL.h"

class Point4I;
class SDLHapticEffect : public SimObject
{
public:
   enum EffectType
   {
      ConstantEffect = SDL_HAPTIC_CONSTANT,
      SineEffect = SDL_HAPTIC_SINE,
      LeftRightEffect = SDL_HAPTIC_LEFTRIGHT,
      TriangleEffect = SDL_HAPTIC_TRIANGLE,
      SawUpEffect = SDL_HAPTIC_SAWTOOTHUP,
      SawDownEffect = SDL_HAPTIC_SAWTOOTHDOWN,
      RampEffect = SDL_HAPTIC_RAMP,
      SpringEffect = SDL_HAPTIC_SPRING,
      DamperEffect = SDL_HAPTIC_DAMPER,
      InertiaEffect = SDL_HAPTIC_INERTIA,
      FrictionEffect = SDL_HAPTIC_FRICTION,
      CustomEffect = SDL_HAPTIC_CUSTOM,
   };

   enum CoordinateType
   {
      Polar = SDL_HAPTIC_POLAR,
      Cartesian = SDL_HAPTIC_CARTESIAN,
      Spherical = SDL_HAPTIC_SPHERICAL,
   };

protected:
   SDL_HapticEffect mEffect;
   S32 mEffectId; // The Id value assigned to the effect while it is loaded in a device.

public:
   DECLARE_CONOBJECT(SDLHapticEffect);
   DECLARE_DESCRIPTION("Haptic effect instance.");

   SDLHapticEffect();
   virtual ~SDLHapticEffect();

   bool initEffect(EffectType effectMask);

   inline S32 getEffectId() { return mEffectId; }
   inline void setEffectId(S32 idVal) { mEffectId = idVal; }
   inline SDL_HapticEffect* getEffect() { return &mEffect; }
   inline S32 getEffectType() { return mEffect.type; }

   bool hasDirection();
   bool getDirectionCoordType(S32 &coordType);
   bool setDirectionCoordType(S32 coordType);
   bool getDirectionCoordData(Point3I &coordData);
   bool setDirectionCoordData(Point3I coordData);

   bool hasEnvelope();
   bool getEnvelopeData(Point4I &envData);
   bool setEnvelopeData(S32 attack_length, S32 attack_level, S32 fade_length, S32 fade_level);

   bool getReplayData(Point2I &replayData);
   bool setReplaydata(S32 length, S32 delay);

   bool getConstantEffectData(F32 &level);
   bool setConstantEffectData(F32 level);

   bool isPeriodicEffect();
   bool getPeriodicEffectData(Point4I &periodicData);
   bool setPeriodicEffectData(S32 period, S32 magnitude, S32 offset, S32 phase);

   bool getLeftRightEffectData(Point2I &leftRightData);
   bool setLeftRightEffectdata(S32 large_magnitude, S32 small_magnitude);

   bool getRampEffectData(Point2I &rampData);
   bool setRampEffectdata(S32 ramp_start, S32 ramp_end);

   bool isConditionEffect();
   bool getConditionEffectRSat(Point3I &satData);
   bool setConditionEffectRSat(Point3I satData);
   bool getConditionEffectLSat(Point3I &satData);
   bool setConditionEffectLSat(Point3I satData);

   bool getConditionEffectRCoeff(Point3I &coeffData);
   bool setConditionEffectRCoeff(Point3I coeffData);
   bool getConditionEffectLCoeff(Point3I &coeffData);
   bool setConditionEffectLCoeff(Point3I coeffData);

   bool getConditionEffectDeadzone(Point3I &deadband);
   bool setConditionEffectDeadzone(Point3I deadband);
   bool getConditionEffectCenter(Point3I &centers);
   bool setConditionEffectCenter(Point3I centers);
};

typedef SDLHapticEffect::EffectType SDLEffectType;
typedef SDLHapticEffect::CoordinateType SDLCoordinateType;
DefineEnumType(SDLEffectType);
DefineEnumType(SDLCoordinateType);

#endif
