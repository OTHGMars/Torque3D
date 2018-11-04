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
#include "platform/input/openVR/openVRTrackedHand.h"
#include "platform/input/openVR/openVRProvider.h"

#include "core/stream/bitStream.h"
#include "sim/netConnection.h"
#include "T3D/shapeBase.h"

#ifdef TORQUE_EXTENDED_MOVE
#include "T3D/gameBase/extended/extendedMove.h"
#endif


//-----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(OpenVRTrackedHand);

ConsoleDocClass(OpenVRTrackedHand,
   "@brief Implements a networked VR Hand Skeleton.\n"
   "@ingroup OpenVR\n");

//-----------------------------------------------------------------------------
// Object setup and teardown
//-----------------------------------------------------------------------------
OpenVRTrackedHand::OpenVRTrackedHand()
:  mAnimateOnServer(true),
   mAnimationActionIndex(-1),
   mBinaryBlobSize(0)
{
}

OpenVRTrackedHand::~OpenVRTrackedHand()
{
}

bool OpenVRTrackedHand::extractBlobData(U8* dataPtr, U32 dataSize)
{
   if (isServerObject() && !mAnimateOnServer)
      return true;

   if (vr::VRInput())
   {
      vr::EVRSkeletalTransformSpace poseSpace;
      vr::VRBoneTransform_t boneData[eBone_Count];
      vr::EVRInputError vrError = vr::VRInput()->DecompressSkeletalBoneData(
         (void *)dataPtr, dataSize, &poseSpace, boneData, eBone_Count);
      if (vrError == vr::VRInputError_None)
      {
         setBonePoses(boneData);
         return true;
      }
   }

   return false;
}

void OpenVRTrackedHand::setBonePoses(vr::VRBoneTransform_t* bonePose)
{
   for (S32 i = 0; i < eBone_Count; ++i)
   {
      mCurrentPoses[i].rotation.set(bonePose[i].orientation.x, -bonePose[i].orientation.z, bonePose[i].orientation.y, bonePose[i].orientation.w);
      mCurrentPoses[i].position.set(bonePose[i].position.v[0], -bonePose[i].position.v[2], bonePose[i].position.v[1]);
   }
}

void OpenVRTrackedHand::getSkeletonIndex()
{
   mAnimationActionIndex = -1;
   if (!mAnimationSourceStringHandle.isValidString() || !vr::VRInput())
      return;

   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      mAnimationActionIndex = OPENVR->getSkeletonIndex(mAnimationSourceStringHandle.getString());
}

void OpenVRTrackedHand::setLocalSkeletonPose()
{
   vr::VRBoneTransform_t boneData[eBone_Count];
   if (OPENVR->getSkeletonNodes(mAnimationActionIndex, boneData))
   {
      setBonePoses(boneData);

      // Skeleton was rotated on import to make +z the up axis. Bone rotations need the same adjustment.
      const MatrixF shapeRot(EulerF(-M_HALFPI_F, 0.0f, 0.0f));

      if (mShapeInstance)
      {
         MatrixF boneMat;
         for (S32 i = 0; i < eBone_Count; ++i)
         {
            QuatF boneRot(boneData[i].orientation.x, -boneData[i].orientation.z, boneData[i].orientation.y, boneData[i].orientation.w);
            boneRot.setMatrix(&boneMat);
            boneMat.inverse();
            boneMat.mul(shapeRot);
            boneMat.setPosition(Point3F(boneData[i].position.v[0], -boneData[i].position.v[2], boneData[i].position.v[1]));
            mShapeInstance->mNodeTransforms[i + 1] = boneMat;
         }
      }
   }
}

//-----------------------------------------------------------------------------
// Object Editing
//-----------------------------------------------------------------------------
void OpenVRTrackedHand::initPersistFields()
{
   addGroup( "Skeleton Data" );
      addField("animateOnServer", TypeBool, Offset(mAnimateOnServer, OpenVRTrackedHand),
         "If true, the hand skeleton will be animated on the server. If false, the animation data will "
         "only be passed to connected clients.\n");
      addProtectedField("animSource", TypeRealString, Offset(mAnimationSourceName, OpenVRTrackedHand),
         &_setFieldAnimSource, &defaultProtectedGetFn,
         "The input source from the action manifest file. If the source is found, it will be used "
         "to provide the real-time skeleton animation when rendering on the controlling client. If not "
         "found or blank, the compressed skeleton data from the move manager will be used on the "
         "controlling client.");
   endGroup( "Skeleton Data" );

   Parent::initPersistFields();
}

bool OpenVRTrackedHand::_setFieldAnimSource(void* object, const char* index, const char* data)
{
   OpenVRTrackedHand *tObj = static_cast<OpenVRTrackedHand*>( object );
   if ( tObj )
      tObj->setAnimSource(data);
   return false;
}

void OpenVRTrackedHand::setAnimSource(const char* name)
{
   if ( !isGhost() )
   {
      if ( name[0] != '\0' )
      {
         if ( name[0] == StringTagPrefixByte )
            mAnimationSourceStringHandle = NetStringHandle( U32(dAtoi(name + 1)) );
         else
            mAnimationSourceStringHandle = NetStringHandle( name );
      }
      else
         mAnimationSourceStringHandle = NetStringHandle();

      setMaskBits( UpdateMask );
   }
}

bool OpenVRTrackedHand::onAdd()
{
   if (!Parent::onAdd())
      return false;

   return true;
}

void OpenVRTrackedHand::onRemove()
{
   Parent::onRemove();
}

U32 OpenVRTrackedHand::packUpdate(NetConnection *conn, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);

   // Write our device information
   if (stream->writeFlag(mask & UpdateMask))
   {
      conn->packNetStringHandleU( stream, mAnimationSourceStringHandle );
   }

   // Don't send skeleton data to the owning client
   if( mOwnerObject && (conn == (NetConnection *) mOwnerObject->getControllingClient()) )
      return retMask;

   if (stream->writeFlag(mask & SkeletonMask))
   {
      stream->writeInt(mBinaryBlobSize, ExtendedMove::MaxBlobSizeBits);
      stream->writeBits(mBinaryBlobSize * 8, mBlobData);
   }

   return retMask;
}

void OpenVRTrackedHand::unpackUpdate(NetConnection *conn, BitStream *stream)
{
   // Let the Parent read any info it sent
   Parent::unpackUpdate(conn, stream);

   if (stream->readFlag())  // UpdateMask
   {
      NetStringHandle newStringHandle = conn->unpackNetStringHandleU(stream);
      if (mAnimationSourceStringHandle != newStringHandle)
      {
         mAnimationSourceStringHandle = newStringHandle;
         getSkeletonIndex();
      }
   }

   if (mbOwnedByClient)
      return;

   if (stream->readFlag())  // SkeletonMask
   {
      mBinaryBlobSize = stream->readInt(ExtendedMove::MaxBlobSizeBits);
      stream->readBits(mBinaryBlobSize * 8, mBlobData);

      dMemcpy(mLastPoses, mCurrentPoses, sizeof(mCurrentPoses));
      extractBlobData(mBlobData, mBinaryBlobSize);
   }
}

void OpenVRTrackedHand::updateMove(const Move *move)
{
#ifdef TORQUE_EXTENDED_MOVE
   const ExtendedMove* emove = dynamic_cast<const ExtendedMove*>(move);
   if (!emove)
      return;

   U32 emoveIndex = mMappedMoveIndex;
   if (emoveIndex >= ExtendedMove::MaxPositionsRotations)
      emoveIndex = 0;

   if (emove->DeviceIsActive[emoveIndex] && emove->binBlobSize[emoveIndex] > 0)
   {
      dMemcpy(mLastPoses, mCurrentPoses, sizeof(mCurrentPoses));

      if (extractBlobData((U8 *)emove->binaryBlob[emoveIndex], emove->binBlobSize[emoveIndex]))
      {
         if (isServerObject())
         {
            dMemcpy(&mBlobData, (void *)emove->binaryBlob[emoveIndex], emove->binBlobSize[emoveIndex]);
            mBinaryBlobSize = emove->binBlobSize[emoveIndex];
            setMaskBits(SkeletonMask);
         }
      }
   }
#endif
}

void OpenVRTrackedHand::processTick(const Move *move)
{
   if (move)
   {
      updateMove(move);
   }

   Parent::processTick(move);
}

void OpenVRTrackedHand::interpolateTick(F32 delta)
{
   if (mbOwnedByClient && !mbRenderFirstPerson)
      return;  // This object will not be rendered.

   if (mbOwnedByClient && mAnimationActionIndex != -1)
   {  // Set the node transforms direct from the device
      setLocalSkeletonPose();
      Parent::interpolateTick(delta);
      return;
   }

   if (mBinaryBlobSize > 0)
   {
      // Skeleton was rotated on import to make +z the up axis. Bone rotations need the same adjustment.
      const MatrixF shapeRot(EulerF(-M_HALFPI_F, 0.0f, 0.0f));
      MatrixF boneMat;
      Point3F pos;
      QuatF rot;

      if (mShapeInstance)
      {
         for (S32 i = 0; i < eBone_Count; ++i)
         {
            rot.interpolate(mCurrentPoses[i].rotation, mLastPoses[i].rotation, delta);
            pos.interpolate(mCurrentPoses[i].position, mLastPoses[i].position, delta);
            rot.setMatrix(&boneMat);
            boneMat.inverse();
            boneMat.mul(shapeRot);
            boneMat.setPosition(pos);
            mShapeInstance->mNodeTransforms[i + 1] = boneMat;
         }
      }
   }
   Parent::interpolateTick(delta);
}

void OpenVRTrackedHand::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);
}
