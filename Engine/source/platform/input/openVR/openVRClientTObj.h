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

#ifndef _OPENVR_CLIENT_TOBJ_H_
#define _OPENVR_CLIENT_TOBJ_H_

#ifndef _SCENEOBJECT_H_
#include "scene/sceneObject.h"
#endif
#ifndef _GFXVERTEXBUFFER_H_
#include "gfx/gfxVertexBuffer.h"
#endif
#ifndef _GFXPRIMITIVEBUFFER_H_
#include "gfx/gfxPrimitiveBuffer.h"
#endif
#ifndef _TSSHAPEINSTANCE_H_
#include "ts/tsShapeInstance.h"
#endif
#include "math/mTransform.h"

#include <openvr.h>

// Maximum of 128 polys on the laser pointer. Each side is one poly.
#define MAX_LASER_POLYS 128

class BaseMatInstance;
class OpenVRRenderModel;
//class PhysicsBody;

/// Implements a client only tracked controller
class OpenVRClientTObj : public SceneObject
{
   typedef SceneObject Parent;

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
   static vr::VRBoneTransform_t smBoneData[eBone_Count];

   // OpenVR render model components
   struct RenderModelSlot
   {
      StringTableEntry componentName; ///< Component name
      S16 mappedNodeIdx;              ///< Mapped node idx in mShape
      OpenVRRenderModel *nativeModel; ///< Native model
   };

   /// @name Rendering
   /// {
   TSShapeInstance *mShapeInstance; ///< Shape used to render the VR hand
   StringTableEntry mModelName;  ///< Name of the OpenVR render model, set from device properties
   OpenVRRenderModel *mBasicModel; ///< Basic model
   Vector<RenderModelSlot> mRenderComponents;
   /// }

   StringTableEntry mInputSource; ///< Input source to obtain device index from
   S32 mDeviceIndex; ///< Controller idx in openvr (for direct updating)
   vr::VRInputValueHandle_t mDeviceHandle;
   StringTableEntry  mPoseActionName; ///< The pose acion name from the action manifest
   S32 mPoseActionIndex;
   StringTableEntry  mSkeletonActionName; ///< The animation name from the action manifest
   vr::VRActionHandle_t mSkeletonActionHandle;

   StringTableEntry  mShapeName;

   // Object boxes for models and pointer
   Box3F mHandBox, mControllerBox, mPointerBox;

   SimObjectPtr<SceneObject> mCollisionObject; ///< Object we're currently colliding with

   /// Object the laserpointer is currently on
   SceneObject *mInteractObject;

   /// Is the laser pointed at an object the user can interact with
   bool mCanInteract;

protected:
   GameConnection *mConnection;
   bool mRenderHand;
   bool mRangeWithController;
   bool mRenderOVRModel;
   bool mLaserActive;

   // Radius in world units of the laser beam at it's base "0 0 0"
   F32 mStartRadius;
   // The number of sides on the laser beam. 3-MAX_LASER_POLYS (128)
   S32 mNumSides;

   F32 mMaxPointerLength;
   F32 mCurrentLaserLength;
   MatrixF mLaserOffsetMat;
   TransformF mOffsetTransform;
   S32 mStatusMeshIdx;

   // The names of the Materials we will use for the laser pointer
   String  mLaserMatName, mLaserHoverMatName;
   // The actual Material instances
   BaseMatInstance *mLaserMatInst, *mLaserHoverMatInst;

   // The GFX vertex and primitive buffers for laser pointer
   typedef GFXVertexPNT VertexType;
   GFXVertexBufferHandle< VertexType > mVertexBuffer;
   GFXPrimitiveBufferHandle            mPrimitiveBuffer;

   // Create the geometry for rendering the laser pointer
   void updateLaserGeometry();

   void updateMaterials();

public:
   void interpolateTick( F32 delta );
   void processTick(const Move *move);
   void advanceTime( F32 timeDelta ) {}


public:
   OpenVRClientTObj();
   virtual ~OpenVRClientTObj();

   void updateRenderData();
   void setupRenderDataFromModel(bool loadComponentModels);

   void clearRenderData();

   DECLARE_CONOBJECT(OpenVRClientTObj);

   void setRenderShape(bool newVal);
   void setRenderController(bool newVal);
   void setRenderLaser(bool newVal);
   void setStartRadius(F32 newVal);
   void setLaserSides(S32 sides);
   void setLaserOffset(TransformF &txfm);
   void radiusChanged();
   void onStateChanged();
   static void initPersistFields();

   bool onAdd();
   void onRemove();

   void setInteractObject(SceneObject* object, bool holding);
   SceneObject *getInteractObject() { return mInteractObject; }

   void setTransform(const MatrixF &mat);
   void setModelName(String &modelName);
   void setModelFromProperty();

   void prepRenderImage(SceneRenderState *state);

   bool resetDeviceIndex();
   S32 getDeviceIndex() { return mDeviceIndex; }

private:
   void setShapeNodes();

   static bool _setStartRadius( void *object, const char *index, const char *data );
   static bool _setLaserSidesField( void *object, const char *index, const char *data );
   static bool _setRenderShapeField( void *object, const char *index, const char *data );
   static bool _setRenderControllerField( void *object, const char *index, const char *data );
   static bool _setRenderLaserField( void *object, const char *index, const char *data );
   static bool _setLaserOffsetField( void *object, const char *index, const char *data );
};

#endif // _OPENVR_CLIENT_TOBJ_H_