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

#ifndef _OPENVR_TRACKED_OBJECT_H_
#define _OPENVR_TRACKED_OBJECT_H_

#ifndef _GAMEBASE_H_
#include "T3D/gameBase/gameBase.h"
#endif
#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif
#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif
#ifndef _NETSTRINGTABLE_H_
#include "sim/netStringTable.h"
#endif
#ifndef _TSSHAPEINSTANCE_H_
#include "ts/tsShapeInstance.h"
#endif
#include "collision/earlyOutPolyList.h"
#include "math/mTransform.h"

#include <openvr.h>

class BaseMatInstance;

class OpenVRTrackedObjectData : public GameBaseData {
public:
   typedef GameBaseData Parent;

   StringTableEntry mShapeFile;
   Resource<TSShape> mShape; ///< Torque model

   // Maximum range for tracking positions
   F32 mTrackingScale;

   // Shape render offset
   TransformF mShapeOffset;

   // Rotation QuatF from mShapeOffset
   QuatF mShapeRot;

   // Transform when tracked device is disconnected
   TransformF mDisconnectedTransform;

public:

   OpenVRTrackedObjectData();
   ~OpenVRTrackedObjectData();

   DECLARE_CONOBJECT(OpenVRTrackedObjectData);

   bool onAdd();
   bool preload(bool server, String &errorStr);

   static void  initPersistFields();

   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};

/// Implements a GameObject which tracks an OpenVR device
class OpenVRTrackedObject : public GameBase
{
   typedef GameBase Parent;

protected:
   enum MaskBits
   {
      UpdateMask = Parent::NextFreeMask << 0,
      MoveMask = Parent::NextFreeMask << 1,
      NextFreeMask = Parent::NextFreeMask << 2
   };

   enum
   {
      TrackedPosBits = 10,
      TrackedQuatBits = 9,
   };

   struct StateDelta
   {
      //Point3F pos;
      VectorF posVec;
      QuatF rot;
   };

   OpenVRTrackedObjectData *mDataBlock;

   /// @name Rendering
   /// {
   TSShapeInstance *mShapeInstance; ///< Shape rendered
   /// }

   String mPoseSourceName; ///< If set, the controlling client will use real-time data from this pose for rendering
   NetStringHandle mPoseSourceStringHandle;
   S32 mPoseActionIndex; ///< Action index. Set on controlling client when the input source is resolved
   S32 mMappedMoveIndex; ///< Movemanager move index for rotation

   StateDelta mDelta;
   Point3F mPosition;
   QuatF mRotation;
   bool mbConnected;
   bool mbOwnedByClient;
   bool mbRenderFirstPerson;  ///< Should this object be rendered on the controlling client

   ShapeBase* mOwnerObject; ///< The ShapeBase object that this device is controlled by.

public:
   OpenVRTrackedObject();
   virtual ~OpenVRTrackedObject();

private:
   bool _createShape();

protected:
   virtual void getActionIndex();

public:
   DECLARE_CONOBJECT(OpenVRTrackedObject);

   static void initPersistFields();
   static bool _setFieldInputSource(void* object, const char* index, const char* data);
   static bool _setFieldRenderFP(void* object, const char* index, const char* data);
   void setInputSource(const char* name);
   void setRenderFirstPerson(bool doRender);

   virtual void inspectPostApply();

   bool onAdd();
   void onRemove();

   bool onNewDataBlock(GameBaseData *dptr, bool reload);

   U32  packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn, BitStream *stream);

   void prepRenderImage(SceneRenderState *state);

   // Time management
   void updateMove(const Move *move);
   void processTick(const Move *move);
   void interpolateTick(F32 delta);
   void advanceTime(F32 dt);
   void setLocalRenderTransform();

   void setOwnerObject(ShapeBase* obj) { mOwnerObject = obj; }
};

#endif // _OPENVR_TRACKED_OBJECT_H_