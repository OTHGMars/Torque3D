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

#ifndef _OPENVR_STRUCTS_H_
#define _OPENVR_STRUCTS_H_

#include <openvr.h>

template<int TEXSIZE> class VRTextureSet
{
public:
   static const int TextureCount = TEXSIZE;
   GFXTexHandle mTextures[TEXSIZE];
   U32 mIndex;

   VRTextureSet() : mIndex(0)
   {
   }

   void init(U32 width, U32 height, GFXFormat fmt, GFXTextureProfile *profile, const String &desc)
   {
      for (U32 i = 0; i < TextureCount; i++)
      {
         mTextures[i].set(width, height, fmt, profile, desc);
      }
   }

   void clear()
   {
      for (U32 i = 0; i < TextureCount; i++)
      {
         mTextures[i] = NULL;
      }
   }

   void advance()
   {
      mIndex = (mIndex + 1) % TextureCount;
   }

   GFXTexHandle& getTextureHandle()
   {
      return mTextures[mIndex];
   }
};

struct OpenVRRenderState
{
   vr::IVRSystem *mHMD;

   FovPort mEyeFov[2];
   MatrixF mEyePose[2];
   MatrixF mHMDPose;

   RectI mEyeViewport[2];
   GFXTextureTargetRef mStereoRT;

   GFXTexHandle mStereoRenderTexture;
   GFXTexHandle mStereoDepthTexture;

   VRTextureSet<4> mOutputEyeTextures;

   GFXDevice::GFXDeviceRenderStyles mRenderMode;

   bool setupRenderTargets(GFXDevice::GFXDeviceRenderStyles mode);
   void _onTextureEvent( GFXTexCallbackCode code );

   void renderPreview();

   void reset(vr::IVRSystem* hmd);
   void updateHMDProjection();
};

class OpenVRRenderModel;
struct LoadedRenderModel
{
   StringTableEntry deviceName;
   StringTableEntry name;
   vr::RenderModel_t *vrModel;
   OpenVRRenderModel *model;
   vr::EVRRenderModelError modelError;
   S32 textureId;
   bool loadedTexture;
};

struct LoadedRenderTexture
{
   U32 vrTextureId;
   vr::RenderModel_TextureMap_t *vrTexture;
   vr::EVRRenderModelError textureError;

   StringTableEntry textureName;
   StringTableEntry texturePath;
   bool textureCached;
};

struct VRActionSet
{
   vr::VRActionSetHandle_t setHandle;
   StringTableEntry setName;

   VRActionSet() { setHandle = 0U; setName = StringTable->EmptyString(); }
   VRActionSet(vr::VRActionSetHandle_t initHandle, const char* initName)
   {
      setHandle = initHandle;
      setName = StringTable->insert(initName, true);
   }
};

struct VRAnalogAction
{
   S32 setIndex;
   vr::VRActionHandle_t actionHandle;
   StringTableEntry actionName;
   StringTableEntry callback;
   bool active;

   VRAnalogAction() { setIndex = -1; actionHandle = 0U; actionName = StringTable->EmptyString(); callback = StringTable->EmptyString(); active = false; }
   VRAnalogAction(S32 initSet, vr::VRActionHandle_t initHandle, const char* initName, const char* initCallback)
   {
      setIndex = initSet;
      actionHandle = initHandle;
      actionName = StringTable->insert(initName, true);
      callback = StringTable->insert(initCallback, false);
      active = false;
   }
};

struct VRDigitalAction
{
   S32 setIndex = -1;
   vr::VRActionHandle_t actionHandle = 0U;
   StringTableEntry actionName;
   StringTableEntry callback;
   bool active = false;

   VRDigitalAction() { actionName = StringTable->EmptyString(); callback = StringTable->EmptyString(); }
   VRDigitalAction(S32 initSet, vr::VRActionHandle_t initHandle, const char* initName, const char* initCallback)
   {
      setIndex = initSet;
      actionHandle = initHandle;
      actionName = StringTable->insert(initName, true);
      callback = StringTable->insert(initCallback, false);
   }
};

struct VRPoseAction
{
   S32 setIndex = -1;
   vr::VRActionHandle_t actionHandle = 0U;
   StringTableEntry actionName;
   StringTableEntry poseCallback;
   StringTableEntry velocityCallback;
   QuatF lastRotation = QuatF::Identity;
   Point3F lastPosition = Point3F::Zero;
   S32 eMoveIndex = -1;
   bool validPose = false;
   bool active = false;

   VRPoseAction()
   { 
      actionName = StringTable->EmptyString();
      poseCallback = StringTable->EmptyString();
      velocityCallback = StringTable->EmptyString();
   }
   VRPoseAction(S32 initSet, vr::VRActionHandle_t initHandle, const char* initName,
         const char* initPoseCB, const char* initVelCB, S32 moveIndex)
   {
      setIndex = initSet;
      actionHandle = initHandle;
      actionName = StringTable->insert(initName, true);

      // Callbacks are optional with pose actions
      if (initPoseCB && initPoseCB[0])
         poseCallback = StringTable->insert(initPoseCB, false);
      else
         poseCallback = StringTable->EmptyString();
      if (initVelCB && initVelCB[0])
         velocityCallback = StringTable->insert(initVelCB, false);
      else
         velocityCallback = StringTable->EmptyString();

      eMoveIndex = moveIndex;
   }
};

struct VRSkeletalAction
{
   S32 setIndex = -1;
   vr::VRActionHandle_t actionHandle = 0U;
   StringTableEntry actionName;
   S32 eMoveIndex = -1;
   bool rangeWithController = false;
   bool active = false;

   VRSkeletalAction() { actionName = StringTable->EmptyString(); }
   VRSkeletalAction(S32 initSet, vr::VRActionHandle_t initHandle, const char* initName, S32 moveIndex)
   {
      setIndex = initSet;
      actionHandle = initHandle;
      actionName = StringTable->insert(initName, true);
      eMoveIndex = moveIndex;
   }
};

#endif // _OPENVR_STRUCTS_H_