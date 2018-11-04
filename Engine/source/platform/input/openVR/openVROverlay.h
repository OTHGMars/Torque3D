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

#ifndef _OPENVR_OVERLAY_H_
#define _OPENVR_OVERLAY_H_

#ifndef _GUIOFFSCREENCANVAS_H_
#include "gui/core/guiOffscreenCanvas.h"
#endif
#ifndef _OPENVRDEVICE_H_
#include "platform/input/openVR/openVRProvider.h"
#endif
#ifndef _COLLISION_H_
#include "collision/collision.h"
#endif


typedef vr::VROverlayInputMethod OpenVROverlayInputMethod;
typedef vr::VROverlayTransformType OpenVROverlayTransformType;
typedef vr::EGamepadTextInputMode OpenVRGamepadTextInputMode;
typedef vr::EGamepadTextInputLineMode OpenVRGamepadTextInputLineMode;
typedef vr::ETrackingResult OpenVRTrackingResult;
typedef vr::ETrackingUniverseOrigin OpenVRTrackingUniverseOrigin;
typedef vr::EOverlayDirection OpenVROverlayDirection;
typedef vr::EVRState OpenVRState;

class OpenVROverlay : public GuiOffscreenCanvas
{
public:
   typedef GuiOffscreenCanvas Parent;

   enum OverlayType
   {
      OVERLAYTYPE_OVERLAY,
      OVERLAYTYPE_DASHBOARD,
   };

   vr::VROverlayHandle_t mOverlayHandle;
   vr::VROverlayHandle_t mThumbOverlayHandle;

   // Desired OpenVR state
   U32 mOverlayFlags;
   F32 mOverlayWidth;

   vr::VROverlayTransformType mOverlayTransformType;
   MatrixF mTransform;
   vr::TrackedDeviceIndex_t mTransformDeviceIndex;
   String mTransformDeviceComponent;


   vr::VROverlayInputMethod mInputMethod;
   Point2F mMouseScale;

   vr::TrackedDeviceIndex_t mControllerDeviceIndex;

   GFXTexHandle mStagingTexture; ///< Texture used by openvr

   LinearColorF mOverlayColor;

   bool mOverlayTypeDirty; ///< Overlay type is dirty
   bool mOverlayDirty; ///< Overlay properties are dirty
   OverlayType mOverlayType;
   bool mIsHQOverlay;

   //

   OpenVROverlay();
   virtual ~OpenVROverlay();

   static void initPersistFields();

   DECLARE_CONOBJECT(OpenVROverlay);

   bool onAdd();
   void onRemove();

   void resetOverlay();
   void updateOverlay();

   void showOverlay();
   void hideOverlay();
   void setCurveRange(F32 minDist, F32 maxDist);

   bool isOverlayVisible();
   bool isOverlayHoverTarget();

   bool isGamepadFocussed();
   bool isActiveDashboardOverlay();

   MatrixF getTransformForOverlayCoordinates(const Point2F &pos);
   bool castRay(const Point3F &origin, const Point3F &direction, RayInfo *info);

   void moveGamepadFocusToNeighbour();

   void handleOpenVREvents();
   void updateTextControl(GuiControl* ctrl);
   void onFrameRendered();

   virtual void enableKeyboardTranslation();
   virtual void disableKeyboardTranslation();
   virtual void setNativeAcceleratorsEnabled(bool enabled);
};

typedef OpenVROverlay::OverlayType OpenVROverlayType;
DefineEnumType(OpenVROverlayType);


#endif
