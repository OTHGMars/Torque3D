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

#ifndef _OPENVR_PROVIDER_H_
#define _OPENVR_PROVIDER_H_

#include "math/mQuat.h"
#include "math/mPoint4.h"
#include "math/util/frustum.h"
#include "core/util/tSingleton.h"

#include "gfx/gfxDevice.h"
#include "gfx/gfxVertexBuffer.h"
#include "gfx/gfxPrimitiveBuffer.h"
#include "gfx/gfxTarget.h"

#include "platform/input/OpenVR/openVRStructs.h"
#include "platform/input/IInputDevice.h"
#include "platform/input/event.h"
#include "platform/output/IDisplayDevice.h"
#include "materials/materialDefinition.h"
#include "materials/baseMatInstance.h"

class OpenVRHMDDevice;
class OpenVROverlay;
class BaseMatInstance;
class SceneRenderState;
struct MeshRenderInst;
class Namespace;
class NamedTexTarget;
class OpenVRRenderModel;

typedef vr::VROverlayInputMethod OpenVROverlayInputMethod;
typedef vr::VROverlayTransformType OpenVROverlayTransformType;
typedef vr::EGamepadTextInputMode OpenVRGamepadTextInputMode;
typedef vr::EGamepadTextInputLineMode OpenVRGamepadTextInputLineMode;
typedef vr::ETrackingResult OpenVRTrackingResult;
typedef vr::ETrackingUniverseOrigin OpenVRTrackingUniverseOrigin;
typedef vr::EOverlayDirection OpenVROverlayDirection;
typedef vr::EVRState OpenVRState;
typedef vr::TrackedDeviceClass OpenVRTrackedDeviceClass;
typedef vr::EVRControllerAxisType OpenVRControllerAxisType;
typedef vr::ETrackedControllerRole OpenVRTrackedControllerRole;

DefineEnumType(OpenVROverlayInputMethod);
DefineEnumType(OpenVROverlayTransformType);
DefineEnumType(OpenVRGamepadTextInputMode);
DefineEnumType(OpenVRGamepadTextInputLineMode);
DefineEnumType(OpenVRTrackingResult);
DefineEnumType(OpenVRTrackingUniverseOrigin);
DefineEnumType(OpenVROverlayDirection);
DefineEnumType(OpenVRState);
DefineEnumType(OpenVRTrackedDeviceClass);
DefineEnumType(OpenVRControllerAxisType);
DefineEnumType(OpenVRTrackedControllerRole);

namespace OpenVRUtil
{
   /// Convert a matrix in OVR space to torque space
   void convertTransformFromOVR(const MatrixF &inRotTMat, MatrixF& outRotation);

   /// Convert a matrix in torque space to OVR space
   void convertTransformToOVR(const MatrixF& inRotation, MatrixF& outRotation);

   /// Converts vr::HmdMatrix34_t to a MatrixF
   MatrixF convertSteamVRAffineMatrixToMatrixFPlain(const vr::HmdMatrix34_t &mat);

   /// Converts a MatrixF to a vr::HmdMatrix34_t
   void convertMatrixFPlainToSteamVRAffineMatrix(const MatrixF &inMat, vr::HmdMatrix34_t &outMat);

   U32 convertOpenVRButtonToTorqueButton(uint32_t vrButton);

   /// Converts a point to OVR coords
   inline Point3F convertPointToOVR(const Point3F &point)
   {
      return Point3F(-point.x, -point.z, point.y);
   }

   /// Converts a point from OVR coords
   inline Point3F convertPointFromOVR(const Point3F &point)
   {
      return Point3F(-point.x, point.z, -point.y);
   }

   // Converts a point from OVR coords, from an input float array
   inline Point3F convertPointFromOVR(const vr::HmdVector3_t& v)
   {
      return Point3F(-v.v[0], v.v[2], -v.v[1]);
   }
};

/** The mappable IVRInput action types */
enum EOpenVRActionType
{
   OpenVRActionType_Digital = 0,
   OpenVRActionType_Analog = 1,
   OpenVRActionType_Pose = 2,
   OpenVRActionType_Skeleton = 3,
};
typedef EOpenVRActionType OpenVRActionType;
DefineEnumType(OpenVRActionType);

//------------------------------------------------------------

class OpenVRProvider : public IDisplayDevice, public IInputDevice
{
protected:
   enum
   {
      MaxActiveActionSets = 5,  // The maximum number of action set layers that can be active at one time.
   };

public:

   OpenVRProvider();
   ~OpenVRProvider();

   static void staticInit();

   bool enable();
   bool disable();

   bool getActive() { return mHMD != NULL; }
   inline vr::IVRRenderModels* getRenderModels() { return mRenderModels; }

   /// @name Input handling
   /// {
   virtual bool process();
   /// }

   /// @name Display handling
   /// {
   virtual bool providesFrameEyePose() const;
   virtual void getFrameEyePose(IDevicePose *pose, S32 eyeId) const;

   virtual bool providesEyeOffsets() const;
   /// Returns eye offset not taking into account any position tracking info
   virtual void getEyeOffsets(Point3F *dest) const;

   virtual bool providesFovPorts() const;
   virtual void getFovPorts(FovPort *out) const;

   virtual void getStereoViewports(RectI *out) const;
   virtual void getStereoTargets(GFXTextureTarget **out) const;

   virtual void setDrawCanvas(GuiCanvas *canvas);
   virtual void setDrawMode(GFXDevice::GFXDeviceRenderStyles style);

   virtual void setCurrentConnection(GameConnection *connection);
   virtual GameConnection* getCurrentConnection();

   virtual GFXTexHandle getPreviewTexture();

   virtual void onStartFrame();
   virtual void onEndFrame();

   virtual void onEyeRendered(U32 index);

   virtual void setRoomTracking(bool room);

   bool _handleDeviceEvent(GFXDevice::GFXDeviceEventType evt);

   /// }

   /// @name OpenVR handling
   /// {
   void processVREvent(const vr::VREvent_t & event);
   void updateHMDPose();
   IDevicePose getTrackedDevicePose(U32 idx);

   void resetSensors();
   void orientUniverse(const MatrixF &mat);
   void rotateUniverse(const F32 yaw);

   //void mapDeviceToEvent(U32 deviceIdx, S32 eventIdx);
   //void resetEventMap();

   /// }

   /// @name Overlay registration
   /// {
   void registerOverlay(OpenVROverlay* overlay);
   void unregisterOverlay(OpenVROverlay* overlay);
   /// }

   /// @name Model loading
   /// {
   const S32 preloadRenderModel(StringTableEntry deviceName, StringTableEntry name);
   const S32 preloadRenderModelTexture(StringTableEntry deviceName, U32 index);
   bool getRenderModel(S32 idx, OpenVRRenderModel **ret, bool &failed);
   bool getRenderModelTexture(S32 idx, bool &failed);
   bool getRenderModelTextureName(S32 idx, String &outName);
   void resetRenderModels();
   /// }


   /// @name Console API
   /// {
   OpenVROverlay *getGamepadFocusOverlay();
   void setOverlayNeighbour(vr::EOverlayDirection dir, OpenVROverlay *overlay);

   bool isDashboardVisible();
   void showDashboard(const char *overlayToShow);

   vr::TrackedDeviceIndex_t getPrimaryDashboardDevice();

   void setKeyboardTransformAbsolute(const MatrixF &xfm);
   void setKeyboardPositionForOverlay(OpenVROverlay *overlay, const RectI &rect);

   StringTableEntry getControllerModel(U32 idx);

   U32 getOVRDeviceType() { return mDeviceType; }

   String getDeviceClass(U32 deviceIdx);
   String getDevicePropertyString(U32 deviceIdx, U32 propID);
   bool getDevicePropertyBool(U32 deviceIdx, U32 propID);
   S32 getDevicePropertyInt(U32 deviceIdx, U32 propID);
   String getDevicePropertyUInt(U32 deviceIdx, U32 propID);
   F32 getDevicePropertyFloat(U32 deviceIdx, U32 propID);
   String getControllerAxisType(U32 deviceIdx, U32 axisID);
   String getControllerRole(U32 deviceIdx);
   /// }

   /// @name OpenVR state
   /// {
   vr::IVRSystem *mHMD;
   vr::IVRRenderModels *mRenderModels;
   String mDriver;
   String mDisplay;
   vr::TrackedDevicePose_t mTrackedDevicePose;
   IDevicePose mCurrentHMDPose;
   OpenVRRenderState mHMDRenderState;

   vr::ETrackingUniverseOrigin mTrackingSpace;
   F32 mStandingHMDHeight;

   Vector<OpenVROverlay*> mOverlays;

   Vector<LoadedRenderModel> mLoadedModels;
   Vector<LoadedRenderTexture> mLoadedTextures;
   Map<StringTableEntry, S32> mLoadedModelLookup;
   Map<U32, S32> mLoadedTextureLookup;

   /// }

   GuiCanvas* mDrawCanvas;
   GameConnection* mGameConnection;

   /// @name HMD Rotation offset
   /// {
   static F32 smUniverseYawOffset;
   static F32 smHMDmvYaw;
   static bool smRotateYawWithMoveActions;
   static MatrixF smUniverseRotMat;
   /// }

   static String smShapeCachePath;
   static String smManifestPath;

   /// @name IVRInput handling
   /// {
private:
   bool mInputInitialized;
   Vector<VRActionSet> mActionSets;
   Vector<VRAnalogAction> mAnalogActions;
   Vector<VRDigitalAction> mDigitalActions;
   Vector<VRPoseAction> mPoseActions;
   Vector<VRSkeletalAction> mSkeletalActions;
   Vector<vr::VRActionHandle_t> mHapticOutputs;

   U32 mNumSetsActive;
   vr::VRActiveActionSet_t mActiveSets[MaxActiveActionSets];
   S32 mActiveSetIndexes[MaxActiveActionSets];
   void resetActiveSets();

   bool initInput();
   void processDigitalActions();
   void processAnalogActions();
   void processPoseActions();
   void processSkeletalActions();

public:
   S32 addActionSet(const char* setName);
   S32 addAnalogAction(U32 setIndex, const char* actionName, const char* callbackFunc);
   S32 addDigitalAction(U32 setIndex, const char* actionName, const char* callbackFunc);
   S32 addPoseAction(U32 setIndex, const char* actionName, const char* poseCallback, const char* velocityCallback, S32 moveIndex);
   S32 addSkeletalAction(U32 setIndex, const char* actionName, S32 moveIndex);
   S32 addHapticOutput(const char* outputName);

   S32 getPoseIndex(const char* actionName);
   bool getCurrentPose(S32 poseIndex, Point3F& position, QuatF& rotation);
   bool setPoseCallbacks(S32 poseIndex, const char* poseCallback, const char* velocityCallback);
   S32 getSkeletonIndex(const char* actionName);
   bool getSkeletonNodes(S32 skeletonIndex, vr::VRBoneTransform_t* boneData);
   bool setSkeletonMode(S32 skeletonIndex, bool withController);

   bool activateActionSet(S32 controllerIndex, U32 setIndex);
   bool pushActionSetLayer(S32 controllerIndex, U32 setIndex);
   bool popActionSetLayer(S32 controllerIndex, U32 setIndex);
   bool triggerHapticEvent(U32 actionIndex, float fStartSecondsFromNow, float fDurationSeconds, float fFrequency, float fAmplitude);

   void showActionOrigins(U32 setIndex, OpenVRActionType actionType, U32 actionIndex);
   void showActionSetBinds(U32 setIndex);
   /// }

public:
   // For ManagedSingleton.
   static const char* getSingletonName() { return "OpenVRProvider"; }
};

/// Returns the OpenVRProvider singleton.
#define OPENVR ManagedSingleton<OpenVRProvider>::instance()

#endif   // _OPENVR_PROVIDER_H_
