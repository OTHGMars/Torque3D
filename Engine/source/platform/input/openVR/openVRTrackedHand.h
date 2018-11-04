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

#ifndef _OPENVR_TRACKED_HAND_H_
#define _OPENVR_TRACKED_HAND_H_
#include "platform/input/openVR/openVRTrackedObject.h"
#include "T3D/gameBase/extended/extendedMove.h"

/// Implements a GameObject which tracks an OpenVR hand skeleton animation
class OpenVRTrackedHand : public OpenVRTrackedObject
{
   typedef OpenVRTrackedObject Parent;

   // Hand skeleton bone indexes copied from https://github.com/ValveSoftware/openvr/wiki/Hand-Skeleton
   typedef int32_t BoneIndex_t;
   const BoneIndex_t INVALID_BONEINDEX = -1;
   enum HandSkeletonBone : BoneIndex_t
   {
      eBone_Root = 0,
      eBone_Wrist,
      eBone_Thumb0,
      eBone_Thumb1,
      eBone_Thumb2,
      eBone_Thumb3,
      eBone_IndexFinger0,
      eBone_IndexFinger1,
      eBone_IndexFinger2,
      eBone_IndexFinger3,
      eBone_IndexFinger4,
      eBone_MiddleFinger0,
      eBone_MiddleFinger1,
      eBone_MiddleFinger2,
      eBone_MiddleFinger3,
      eBone_MiddleFinger4,
      eBone_RingFinger0,
      eBone_RingFinger1,
      eBone_RingFinger2,
      eBone_RingFinger3,
      eBone_RingFinger4,
      eBone_PinkyFinger0,
      eBone_PinkyFinger1,
      eBone_PinkyFinger2,
      eBone_PinkyFinger3,
      eBone_PinkyFinger4,
      eBone_Aux_Thumb,
      eBone_Aux_IndexFinger,
      eBone_Aux_MiddleFinger,
      eBone_Aux_RingFinger,
      eBone_Aux_PinkyFinger,
      eBone_Count
   };

   struct NodePose
   {
      QuatF rotation = QuatF::Identity;
      Point3F position = Point3F::Zero;
   };

protected:
   enum MaskBits
   {
      SkeletonMask = Parent::NextFreeMask << 0,
      NextFreeMask = Parent::NextFreeMask << 1
   };

   bool mAnimateOnServer; ///< If true the hand skeleton will be animated on the server.
   String mAnimationSourceName; ///< If set, the controlling client will use real-time data from this animation for rendering
   NetStringHandle mAnimationSourceStringHandle;
   S32 mAnimationActionIndex; ///< Action index. Set on controlling client when the input source is resolved

   NodePose mCurrentPoses[eBone_Count];
   NodePose mLastPoses[eBone_Count];

public:
   OpenVRTrackedHand();
   virtual ~OpenVRTrackedHand();

private:
   U32 mBinaryBlobSize;
   U8 mBlobData[ExtendedMove::MaxBinBlobSize];

   bool extractBlobData(U8* dataPtr, U32 dataSize);
   void setBonePoses(vr::VRBoneTransform_t* bonePose);

protected:
   virtual void getSkeletonIndex();
   void setLocalSkeletonPose();

public:
   DECLARE_CONOBJECT(OpenVRTrackedHand);

   static void initPersistFields();
   static bool _setFieldAnimSource(void* object, const char* index, const char* data);
   void setAnimSource(const char* name);

   bool onAdd();
   void onRemove();

   U32  packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn, BitStream *stream);

   // Time management
   void updateMove(const Move *move);
   void processTick(const Move *move);
   void interpolateTick(F32 delta);
   void advanceTime(F32 dt);
};

#endif // _OPENVR_TRACKED_HAND_H_