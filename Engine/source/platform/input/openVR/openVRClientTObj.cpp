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
#include "platform/input/openVR/openVRClientTObj.h"
#include "platform/input/openVR/openVRProvider.h"
#include "platform/input/openVR/openVRRenderModel.h"
//#include "platform/input/openVR/openVRInput.h"

#include "T3D/gameBase/gameConnection.h"

#include "math/mathIO.h"
#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "core/resourceManager.h"
#include "materials/materialManager.h"
#include "materials/baseMatInstance.h"
#include "renderInstance/renderPassManager.h"
#include "lighting/lightQuery.h"
#include "console/engineAPI.h"
#include "gfx/gfxTextureManager.h"
#include "gfx/sim/debugDraw.h"
#include "gfx/gfxTransformSaver.h"
#include "environment/skyBox.h"
#include "collision/boxConvex.h"
#include "collision/concretePolyList.h"
#include "T3D/physics/physicsPlugin.h"
#include "T3D/physics/physicsCollision.h"
#include "T3D/physics/physicsBody.h"

#define DEBUG_CONTROLLER_MODELS 1

//static const U32 sCollisionMoveMask = (PlayerObjectType |
//   StaticShapeObjectType | VehicleObjectType);

vr::VRBoneTransform_t OpenVRClientTObj::smBoneData[eBone_Count] = { 0 };
//U32 OpenVRClientTObj::sClientCollisionMask = sCollisionMoveMask;

//-----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(OpenVRClientTObj);

ConsoleDocClass(OpenVRClientTObj,
   "@brief Renders and handles interactions for OpenVR controllers and tracked objects.\n\n"
   "This class implements client-side rendering and interactions with OpenVR controllers.\n\n"
   "@ingroup OpenVR\n");


OpenVRClientTObj::OpenVRClientTObj() :
   mShapeInstance(NULL),
   mBasicModel(NULL),
   mDeviceIndex(-1),
   mDeviceHandle(vr::k_ulInvalidInputValueHandle),
   //mIsRightHand(true),
   //mMappedMoveIndex(-1),
   mPoseActionIndex(-1),
   mSkeletonActionHandle(vr::k_ulInvalidInputValueHandle),
   mRenderHand(false),
   mRenderOVRModel(true),
   mLaserActive(false),
   mStartRadius(0.03f),
   mNumSides(12),
   mMaxPointerLength(10.0f),
   mLaserMatInst(NULL),
   mLaserHoverMatInst(NULL),
   mHandBox(Box3F::Zero),
   mControllerBox(Box3F::Zero),
   mPointerBox(Box3F::Zero),
   mCurrentLaserLength(0.0f),
   mInteractObject(NULL),
   mCanInteract(false),
   mStatusMeshIdx(-1),
   mConnection(NULL)
{
   mTypeMask |= StaticObjectType | StaticShapeObjectType;
   mNetFlags = IsGhost | ScopeLocal; // <-- Don't ever create this on a server...

   mModelName = StringTable->insert("");
   mShapeName = StringTable->insert("");
   mInputSource = StringTable->insert("");
   mPoseActionName = StringTable->insert("");
   mSkeletonActionName = StringTable->insert("");
   mLaserHoverMatName = StringTable->insert("");
   mLaserOffsetMat.identity();
}

OpenVRClientTObj::~OpenVRClientTObj()
{
   clearRenderData();
}

void OpenVRClientTObj::updateRenderData()
{
   mBasicModel = NULL;
   mRenderComponents.clear();
   mStatusMeshIdx = -1;

   if (!mModelName || mModelName[0] == '\0')
      setModelFromProperty();
   setupRenderDataFromModel(true);
}

void OpenVRClientTObj::setupRenderDataFromModel(bool loadComponentModels)
{
   mControllerBox = Box3F::Zero;
   if (!OPENVR || !OPENVR->isEnabled())
      return;

   vr::IVRRenderModels *models = OPENVR->getRenderModels();
   if (!models)
      return;

   if (mModelName && mModelName[0] != '\0')
   {
      bool failed = false;
      S32 idx = OPENVR->preloadRenderModel(mModelName, mModelName);
      while (!OPENVR->getRenderModel(idx, &mBasicModel, failed))
      {
         if (failed)
            break;
      }
   }

   if (loadComponentModels)
   {
      mRenderComponents.setSize(models->GetComponentCount(mModelName));

      for (U32 i = 0, sz = mRenderComponents.size(); i < sz; i++)
      {
         RenderModelSlot &slot = mRenderComponents[i];
         char buffer[1024];

         slot.mappedNodeIdx = -1;
         slot.componentName = NULL;
         slot.nativeModel = NULL;

         U32 result = models->GetComponentName(mModelName, i, buffer, sizeof(buffer));
         if (result == 0)
            continue;

         slot.componentName = StringTable->insert(buffer, true);

         result = models->GetComponentRenderModelName(mModelName, slot.componentName, buffer, sizeof(buffer));
         if (result == 0)
         {
#ifdef DEBUG_CONTROLLER_MODELS
            Con::printf("Controller[%s] component %i NAME == %s, No Model", mModelName, i, buffer);
#endif
            continue;
         }

#ifdef DEBUG_CONTROLLER_MODELS
         Con::printf("Controller[%s] component %i NAME == %s, Loaded: %s", mModelName, i, slot.componentName, buffer);
#endif

         bool failed = false;
         S32 idx = OPENVR->preloadRenderModel(mModelName, StringTable->insert(buffer, true));
         while (!OPENVR->getRenderModel(idx, &slot.nativeModel, failed))
         {
            if (failed)
               break;
         }
         if (!failed)
         {
            mControllerBox.intersect(slot.nativeModel->mLocalBox);
            if (dStricmp("status", slot.componentName) == 0)
               mStatusMeshIdx = i;
         }
      }
   }
}

void OpenVRClientTObj::clearRenderData()
{
   mBasicModel = NULL;
   mRenderComponents.clear();
   mStatusMeshIdx = -1;

   if (mShapeInstance)
      delete mShapeInstance;
   mShapeInstance = NULL;
   SAFE_DELETE(mLaserMatInst);
   SAFE_DELETE(mLaserHoverMatInst);
}

//-----------------------------------------------------------------------------
// Object Editing
//-----------------------------------------------------------------------------
bool OpenVRClientTObj::_setStartRadius( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   object->setStartRadius(dAtof(data));
   return false;
}

bool OpenVRClientTObj::_setLaserSidesField( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   object->setLaserSides(dAtoi(data));
   return false;
}

bool OpenVRClientTObj::_setRenderShapeField( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   object->setRenderShape(dAtob(data));
   return false;
}

bool OpenVRClientTObj::_setRenderControllerField( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   object->setRenderController(dAtob(data));
   return false;
}

bool OpenVRClientTObj::_setRenderLaserField( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   object->setRenderLaser(dAtob(data));
   return false;
}

bool OpenVRClientTObj::_setLaserOffsetField( void *obj, const char *index, const char *data )
{
   OpenVRClientTObj *object = static_cast<OpenVRClientTObj*>(obj);
   if (object)
   {
      TransformF txfm;
      Con::setData( TypeTransformF, &txfm, 0, 1, &data );
      object->setLaserOffset(txfm);
      return false;
   }
   return true;
}

void OpenVRClientTObj::initPersistFields()
{
   addGroup( "VR Controller" );
      addField("inputSource",   TypeCaseString,  Offset( mInputSource, OpenVRClientTObj ),
         "OpenVR input source e.g. /user/hand/right, /user/hand/left." );
      addField("poseAction",   TypeCaseString,  Offset( mPoseActionName, OpenVRClientTObj ),
         "The name for the hand pose from the Action manifest file (e.g. /actions/demo/in/hand_left)." );
      addField("skeletonAction",   TypeCaseString,  Offset( mSkeletonActionName, OpenVRClientTObj ),
         "The name for the skeleton from the Action Manifest file (e.g. /actions/demo/in/lh_anim)." );
      addField("shapeName",   TypeShapeFilename,  Offset( mShapeName, OpenVRClientTObj ),
         "Path and filename of the model file (.DTS, .DAE) to use for the vr hand." );
      addProtectedField("renderHand", TypeBool, Offset(mRenderHand, OpenVRClientTObj), &_setRenderShapeField, &defaultProtectedGetFn,
         "Toggles rendering of the vr hand model.");
      addProtectedField("renderOVRModel", TypeBool, Offset(mRenderOVRModel, OpenVRClientTObj), &_setRenderControllerField, &defaultProtectedGetFn,
         "Toggles rendering of the tracked device model obtained from openVR.");
      addField("rangeWithController",   TypeBool,  Offset( mRangeWithController, OpenVRClientTObj ),
         "True to use hand pose with controller. False to enable closed fist. (Not currently enabled by the runtime?)" );
   endGroup( "VR Controller" );

   addGroup( "Laser Pointer" );
      addProtectedField("activeLaser", TypeBool, Offset(mLaserActive, OpenVRClientTObj), &_setRenderLaserField, &defaultProtectedGetFn,
         "Toggles activation and rendering of the laser pointer.");
      addProtectedField("startRadius", TypeF32, Offset(mStartRadius, OpenVRClientTObj), &_setStartRadius, &defaultProtectedGetFn,
         "Radius in world units of the laser beam at starting (0 0 0) position.");
      addProtectedField("laserSides", TypeS32, Offset(mNumSides, OpenVRClientTObj), &_setLaserSidesField, &defaultProtectedGetFn,
         "The number of sides on the laser beam (3 - 128) == number of polygons.");
      addField("maxPointerLength", TypeF32, Offset(mMaxPointerLength, OpenVRClientTObj),
         "Maximum length for pointer and raycast.");
      addField("laserMaterial", TypeMaterialName, Offset( mLaserMatName, OpenVRClientTObj ),
         "The name of the material used to render the laser pointer in it's normal state." );
      addField("laserHoverMaterial", TypeMaterialName, Offset( mLaserHoverMatName, OpenVRClientTObj ),
         "The name of the material used to render the laser pointer when it is pointed at an interactable object." );
      addProtectedField( "laserOffset", TypeTransformF, Offset( mOffsetTransform, OpenVRClientTObj ), &_setLaserOffsetField, &defaultProtectedGetFn,
         "3D offset for the laser pointer base position relative to the tracked device orientation." );
   endGroup( "Laser Pointer" );

   Parent::initPersistFields();
}

bool OpenVRClientTObj::onAdd()
{
   if (!Parent::onAdd())
      return false;

   mConnection = GameConnection::getConnectionToServer();
   if (!mConnection)
   {  
      Con::errorf("OpenVRClientTObj::onAdd - No Serverconnection");
      //return false;
   }

   mPoseActionIndex = -1;
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      mPoseActionIndex = OPENVR->getPoseIndex(mPoseActionName);
   if (mPoseActionIndex == -1)
      return false;

   vr::EVRInputError vrError = vr::VRInput()->GetActionHandle(mSkeletonActionName, &mSkeletonActionHandle);
   if (vrError != vr::VRInputError_None)
      return false;

   if (mShapeName && mShapeName[0] != '\0')
   {
      Resource<TSShape> shape;
      shape = ResourceManager::get().load(mShapeName);
      if ( bool(shape) == false )
         Con::errorf( "OpenVRClientTObj::onAdd() - Unable to load shape: %s", mShapeName );
      else
      {
         mHandBox = shape->mBounds;
         mShapeInstance = new TSShapeInstance( shape, true );
      }
   }

   mObjBox = mControllerBox;
   resetWorldBox();

   updateMaterials();

   if (mInputSource && mInputSource[0] != '\0')
      resetDeviceIndex();

   // Add this object to the scene
   addToScene();

   onStateChanged();

   return true;
}

void OpenVRClientTObj::onRemove()
{
   // Remove this object from the scene
   removeFromScene();

   clearRenderData();

   Parent::onRemove();
}

void OpenVRClientTObj::processTick(const Move *move)
{
   //if ((mDeviceIndex < 0) || (mDeviceHandle == vr::k_ulInvalidInputValueHandle))
   //   return;

   if (!mConnection)
      mConnection = GameConnection::getConnectionToServer();
   if (!mConnection || mPoseActionIndex == -1)
      return;

   Point3F posVal;
   QuatF rotVal;
   if (!OPENVR->getCurrentPose(mPoseActionIndex, posVal, rotVal))
      return;

   MatrixF temp(1);
   mConnection->getControlVRWorldTransform(&temp);

   MatrixF trackedMat;
   rotVal.setMatrix(&trackedMat);
   trackedMat.setPosition(posVal);
   trackedMat.mulL(temp);

   setTransform(trackedMat);

   if (mLaserActive)
   {
      // The laserpointer is active so do ray trace to find first intersected object
      trackedMat.mul(mLaserOffsetMat);
      Point3F controllerPos = trackedMat.getPosition();
      VectorF rayVec = trackedMat.getForwardVector();
      Point3F endPos = controllerPos + rayVec * mMaxPointerLength;
      RayInfo rInfo;

      if ( gClientContainer.castRayRendered(controllerPos, endPos,
         /*ActionShapeObjectType |*/ ItemObjectType | PlayerObjectType | StaticShapeObjectType, &rInfo) )
      {
         if (mInteractObject != rInfo.object)
            mCanInteract = dAtob(Con::executef("canObjectInteract", rInfo.object->getIdString()));
         mInteractObject = rInfo.object;
         mCurrentLaserLength = rInfo.distance;
      }
      else
      {
         mCanInteract = false;
         mInteractObject = NULL;
         mCurrentLaserLength = mMaxPointerLength;
      }
   }
}

void OpenVRClientTObj::interpolateTick(F32 delta)
{
   if (!mConnection || mPoseActionIndex == -1)
      return;

   Point3F posVal;
   QuatF rotVal;
   if (!OPENVR->getCurrentPose(mPoseActionIndex, posVal, rotVal))
      return;

   MatrixF temp(1);
   mConnection->getControlVRWorldTransform(&temp);

   MatrixF trackedMat;
   rotVal.setMatrix(&trackedMat);
   trackedMat.setPosition(posVal);
   trackedMat.mulL(temp);

   setRenderTransform(trackedMat);

   if (!mRenderHand || (mSkeletonActionHandle == vr::k_ulInvalidInputValueHandle))
      return;

   vr::InputSkeletalActionData_t skeletonData;
   vr::EVRInputError vrError = vr::VRInput()->GetSkeletalActionData(mSkeletonActionHandle,
      &skeletonData, sizeof(vr::InputSkeletalActionData_t), vr::k_ulInvalidInputValueHandle);
   if ((vrError != vr::VRInputError_None) || !skeletonData.bActive)
   {
      //Con::printf("GetSkeletalActionData failed");
      return;
   }

   if (skeletonData.boneCount > eBone_Count)
      skeletonData.boneCount = eBone_Count;
   vr::EVRSkeletalMotionRange motionRange = mRangeWithController ? vr::VRSkeletalMotionRange_WithController : vr::VRSkeletalMotionRange_WithoutController;
   if (vr::VRInputError_None != vr::VRInput()->GetSkeletalBoneData(mSkeletonActionHandle, vr::VRSkeletalTransformSpace_Model,
      motionRange, smBoneData, skeletonData.boneCount, vr::k_ulInvalidInputValueHandle))
      Con::printf("GetSkeletalBoneData failed");
   else
      setShapeNodes();
}

void OpenVRClientTObj::setShapeNodes()
{
   // Skeleton was rotated on import to make +z the up axis. Bone rotations need the same adjustment.
   const MatrixF shapeRot(EulerF(-M_HALFPI_F, 0.0f, 0.0f));

   if (mShapeInstance)
   {
      MatrixF boneMat;
      for (S32 i = 0; i < eBone_Count; ++i)
      {
         QuatF boneRot(smBoneData[i].orientation.x, -smBoneData[i].orientation.z, smBoneData[i].orientation.y, smBoneData[i].orientation.w);
         boneRot.setMatrix(&boneMat);
         boneMat.inverse();
         boneMat.mul(shapeRot);
         boneMat.setPosition(Point3F(smBoneData[i].position.v[0], -smBoneData[i].position.v[2], smBoneData[i].position.v[1]));
         mShapeInstance->mNodeTransforms[i + 1] = boneMat;
      }
   }
}

void OpenVRClientTObj::setInteractObject(SceneObject* object, bool holding)
{
   mInteractObject = object;
}

void OpenVRClientTObj::setTransform(const MatrixF & mat)
{
   // Let SceneObject handle all of the matrix manipulation
   Parent::setTransform(mat);
}

void OpenVRClientTObj::setModelName(String &modelName)
{
   mModelName = StringTable->insert(modelName.c_str(), true);
}

void OpenVRClientTObj::setModelFromProperty()
{
   mModelName = StringTable->insert(OPENVR->getDevicePropertyString(mDeviceIndex, vr::Prop_RenderModelName_String).c_str(), true);
}

void OpenVRClientTObj::setStartRadius(F32 newVal)
{
   mStartRadius = newVal;
   radiusChanged();
}

void OpenVRClientTObj::setLaserSides(S32 sides)
{
   mNumSides = mClamp(sides, 3, MAX_LASER_POLYS);

   if ( !mVertexBuffer.isNull() )
      updateLaserGeometry();
}

void OpenVRClientTObj::setLaserOffset(TransformF &txfm)
{
   mOffsetTransform = txfm;
   mLaserOffsetMat = txfm.getMatrix();
   mPointerBox.set( Point3F( -mStartRadius, 0.0f, -mStartRadius ),
                Point3F(  mStartRadius, mMaxPointerLength,  mStartRadius ) );
   mLaserOffsetMat.mul(mPointerBox);
}

void OpenVRClientTObj::radiusChanged()
{
   // Set up the laserpointer bounding box
   mPointerBox.set( Point3F( -mStartRadius, 0.0f, -mStartRadius ),
                Point3F(  mStartRadius, mMaxPointerLength,  mStartRadius ) );
   mLaserOffsetMat.mul(mPointerBox);

   if ( !mVertexBuffer.isNull() )
      updateLaserGeometry();
}

void OpenVRClientTObj::setRenderShape(bool newVal)
{
   mRenderHand = newVal;
   onStateChanged();
}

void OpenVRClientTObj::setRenderController(bool newVal)
{
   mRenderOVRModel = newVal;
   onStateChanged();
}

void OpenVRClientTObj::setRenderLaser(bool newVal)
{
   mLaserActive = newVal;
   onStateChanged();
}

void OpenVRClientTObj::onStateChanged()
{
   // Reset object box to match current shapes
   bool needsTick = mRenderHand | mRenderOVRModel | mLaserActive;
   mObjBox = Box3F::Zero;
   if (mRenderHand)
      mObjBox.intersect(mHandBox);
   if (mRenderOVRModel)
      mObjBox.intersect(mControllerBox);
   if (mLaserActive)
      mObjBox.intersect(mPointerBox);

   // If we're rendering anything, we need to tick
   if (isProperlyAdded())
   {
      setProcessTick(needsTick);
      if (needsTick && mConnection && mConnection->getControlObject())
         processAfter(mConnection->getControlObject());
   }
}

void OpenVRClientTObj::prepRenderImage(SceneRenderState *state)
{
   if (!mConnection || (mDeviceIndex < 0) || (mDeviceHandle == vr::k_ulInvalidInputValueHandle))
      return;

   if( mRenderHand && mShapeInstance )
   {
      mShapeInstance->setCurrentDetail( 0 );

      if ( mShapeInstance->getCurrentDetail() >= 0 )
      {
         GFXTransformSaver saver;
   
         // Set up our TS render state.
         TSRenderState rdata;
         rdata.setSceneState( state );
         rdata.setFadeOverride( 1.0f );
         rdata.setOriginSort( true );

         // If we have submesh culling enabled then prepare
         // the object space frustum to pass to the shape.
         Frustum culler;
         if ( true )
         {
            culler = state->getCullingFrustum();
            MatrixF xfm( true );
            xfm.scale( Point3F::One / getScale() );
            xfm.mul( getRenderWorldTransform() );
            xfm.mul( culler.getTransform() );
            culler.setTransform( xfm );
            rdata.setCuller( &culler );
         }

         // We might have some forward lit materials
         // so pass down a query to gather lights.
         LightQuery query;
         query.init( getWorldSphere() );
         rdata.setLightQuery( &query );

         MatrixF mat = getRenderTransform();
         mat.scale( mObjScale );
         GFX->setWorldMatrix( mat );

         mShapeInstance->render( rdata );
      }
   }

   if (mRenderOVRModel)
   {
      RenderPassManager *renderPass = state->getRenderPass();
      if (mRenderComponents.size() > 0)
      {
         vr::IVRRenderModels *models = OPENVR->getRenderModels();
         if (!models)
            return;

         for (U32 i = 0, sz = mRenderComponents.size(); i < sz; i++)
         {
            RenderModelSlot slot = mRenderComponents[i];
            vr::RenderModel_ControllerMode_State_t modeState;
            vr::RenderModel_ComponentState_t componentState;

            modeState.bScrollWheelVisible = false;

            if (models->GetComponentStateForDevicePath(mModelName, slot.componentName, mDeviceHandle, &modeState, &componentState))
            {
               if (!(componentState.uProperties & vr::VRComponentProperty_IsVisible) && (i != mStatusMeshIdx))
                  continue;

               if (!slot.nativeModel)
                  continue;

               MeshRenderInst *ri = renderPass->allocInst<MeshRenderInst>();

               // Set our RenderInst as a standard mesh render
               ri->type = RenderPassManager::RIT_Mesh;

               // Calculate our sorting point
               if (state && slot.nativeModel)
               {
                  // Calculate our sort point manually.
                  const Box3F rBox = slot.nativeModel->getWorldBox(mRenderObjToWorld);
                  ri->sortDistSq = rBox.getSqDistanceToPoint(state->getCameraPosition());
               }
               else
               {
                  ri->sortDistSq = 0.0f;
               }

               MatrixF controllerOffsMat = OpenVRUtil::convertSteamVRAffineMatrixToMatrixFPlain(componentState.mTrackingToComponentRenderModel);
               MatrixF offComponentMat(1);
               Point3F offsetPos = controllerOffsMat.getPosition();   // Relative position of tracked device in VR space
               OpenVRUtil::convertTransformFromOVR(controllerOffsMat, offComponentMat);
               MatrixF newTransform =  mRenderObjToWorld * offComponentMat;

               // Set up our transforms
               ri->objectToWorld = renderPass->allocUniqueXform(newTransform);
               ri->worldToCamera = renderPass->allocSharedXform(RenderPassManager::View);
               ri->projection = renderPass->allocSharedXform(RenderPassManager::Projection);

               // If our material needs lights then fill the RIs
               // light vector with the best lights.  // TODO: Can this be before the loop?
               if (true)
               {
                  LightQuery query;
                  Point3F center(0, 0, 0);
                  mRenderObjToWorld.mulP(center);
                  query.init(SphereF(center, 10.0f));
                  query.getLights(ri->lights, 8);
               }

               // Draw model
               slot.nativeModel->draw(state, ri);
               state->getRenderPass()->addInst(ri);
            }
         }
      }
      else if (mBasicModel)
      {
         MeshRenderInst *ri = renderPass->allocInst<MeshRenderInst>();

         // Set our RenderInst as a standard mesh render
         ri->type = RenderPassManager::RIT_Mesh;

         // Calculate our sorting point
         if (state)
         {
            // Calculate our sort point manually.
            const Box3F rBox = mBasicModel->getWorldBox(mRenderObjToWorld);
            ri->sortDistSq = rBox.getSqDistanceToPoint(state->getCameraPosition());
         }
         else
         {
            ri->sortDistSq = 0.0f;
         }

         MatrixF newTransform = mRenderObjToWorld;
         // Set up our transforms
         ri->objectToWorld = renderPass->allocUniqueXform(newTransform);
         ri->worldToCamera = renderPass->allocSharedXform(RenderPassManager::View);
         ri->projection = renderPass->allocSharedXform(RenderPassManager::Projection);

         // If our material needs lights then fill the RIs
         // light vector with the best lights.
         if (true)
         {
            LightQuery query;
            Point3F center(0, 0, 0);
            mRenderObjToWorld.mulP(center);
            query.init(SphereF(center, 10.0f));
            query.getLights(ri->lights, 8);
         }

         // Draw model
         mBasicModel->draw(state, ri);
         state->getRenderPass()->addInst(ri);
      }
   }
   if (mLaserActive && mLaserMatInst)
   {
      if ( mVertexBuffer.isNull() )
         updateLaserGeometry();

      // If we have no material then skip out.
      if ( !mLaserMatInst || !state)
         return;

      // If we don't have a material instance after the override then 
      // we can skip rendering all together.
      BaseMatInstance *matInst = state->getOverrideMaterial( (mCanInteract && mInteractObject && mLaserHoverMatInst) ? mLaserHoverMatInst : mLaserMatInst );
      if ( !matInst )
         return;

      // Get a handy pointer to our RenderPassmanager
      RenderPassManager *renderPass = state->getRenderPass();

      // Allocate an MeshRenderInst so that we can submit it to the RenderPassManager
      MeshRenderInst *ri = renderPass->allocInst<MeshRenderInst>();

      // Set our RenderInst as a standard mesh render
      ri->type = RenderPassManager::RIT_Mesh;

      //If our material has transparency set on this will redirect it to proper render bin
      if ( matInst->getMaterial()->isTranslucent() )
      {
         ri->type = RenderPassManager::RIT_Translucent;
         ri->translucentSort = true;
      }

      // Calculate our sorting point
      if ( state )
      {
         // Calculate our sort point manually.
         const Box3F& rBox = getRenderWorldBox();
         ri->sortDistSq = rBox.getSqDistanceToPoint( state->getCameraPosition() );      
      } 
      else 
         ri->sortDistSq = 0.0f;

      // Set up our transforms
      MatrixF objectToWorld = getRenderTransform();
      objectToWorld.mul(mLaserOffsetMat);
      objectToWorld.scale( Point3F(1, mCurrentLaserLength, 1) );

      ri->objectToWorld = renderPass->allocUniqueXform( objectToWorld );
      ri->worldToCamera = renderPass->allocSharedXform(RenderPassManager::View);
      ri->projection    = renderPass->allocSharedXform(RenderPassManager::Projection);

	   // If our material needs lights then fill the RIs 
      // light vector with the best lights.
      if ( matInst->isForwardLit() )
      {
         LightQuery query;
         query.init( getWorldSphere() );
		   query.getLights( ri->lights, 8 );
      }

      // Set our Material
      ri->matInst = matInst;

      // Set up our vertex buffer and primitive buffer
      ri->vertBuff = &mVertexBuffer;
      ri->primBuff = &mPrimitiveBuffer;

      ri->prim = renderPass->allocPrim();
      ri->prim->type = GFXTriangleList;
      ri->prim->minIndex = 0;
      ri->prim->startIndex = 0;
      ri->prim->numPrimitives = mNumSides;
      ri->prim->startVertex = 0;
      ri->prim->numVertices = mNumSides * 3;

      // We sort by the material then vertex buffer
      ri->defaultKey = matInst->getStateHint();
      ri->defaultKey2 = (uintptr_t)ri->vertBuff; // Not 64bit safe!

      // Submit our RenderInst to the RenderPassManager
      state->getRenderPass()->addInst( ri );
   }
}

bool OpenVRClientTObj::resetDeviceIndex()
{
   S32 oldIndex = mDeviceIndex;
   mDeviceIndex = -1;
   mDeviceHandle = vr::k_ulInvalidInputValueHandle;

   vr::EVRInputError vrError = vr::VRInput()->GetInputSourceHandle(mInputSource, &mDeviceHandle);
   if (vrError == vr::VRInputError_None)
   {
      vr::InputOriginInfo_t originInfo;
      originInfo.devicePath = mDeviceHandle;
      vrError = vr::VRInput()->GetOriginTrackedDeviceInfo(mDeviceHandle, &originInfo, (uint32_t) sizeof(originInfo));
      if (vrError == vr::VRInputError_None)
         mDeviceIndex = originInfo.trackedDeviceIndex;
   }

   if (oldIndex != mDeviceIndex)
      updateRenderData();

   return (mDeviceIndex != -1);
}

void OpenVRClientTObj::updateLaserGeometry()
{
   // Fill the vertex buffer
   VertexType *pVert = NULL;

   mVertexBuffer.set( GFX, mNumSides * 3, GFXBufferTypeStatic );
   pVert = mVertexBuffer.lock();

   Point3F startSize(mStartRadius, 1.0f, mStartRadius);

   // Make points around the base on the xz plane starting at x1z0
   // Peak is at (0, 1, 0)
   Point3F lastPoint(mStartRadius, 0, 0);
   VectorF lastNormal(1, 0, 0);
   F32 lastUV = 0;
   for (U32 i = 0; i < mNumSides; i++)
   {
      F32 uv = 1.0f / ((F32) mNumSides) * (i + 1);
      F32 angle = M_2PI_F / ((F32) mNumSides) * (i + 1);
      Point3F nextPoint(mCos(angle), 0, mSin(angle));
      VectorF nextNormal = nextPoint;
      nextPoint *= startSize;

      // Wind clockwise from last base
      pVert[i*3].point = lastPoint;
      pVert[i*3].normal = lastNormal;
      pVert[i*3].texCoord = Point2F(lastUV, 0);

      // To Top
      pVert[i*3+1].point = Point3F(0, 1, 0);
      pVert[i*3+1].normal = Point3F(0, 1, 0);
      pVert[i*3+1].texCoord = Point2F((uv - lastUV)/2.0f, 1);

      // down to next base
      pVert[i*3+2].point = nextPoint;
      pVert[i*3+2].normal = nextNormal;
      pVert[i*3+2].texCoord = Point2F(uv, 0);

      lastUV = uv;
      lastPoint = nextPoint;
      lastNormal = nextNormal;
   }

   mVertexBuffer.unlock();

   // Fill the primitive buffer
   U16 *pIdx = NULL;
   mPrimitiveBuffer.set( GFX, mNumSides * 3, mNumSides, GFXBufferTypeStatic );
   mPrimitiveBuffer.lock(&pIdx);     
   
   for (U16 i = 0; i < mNumSides * 3; i++)
      pIdx[i] = i;

   mPrimitiveBuffer.unlock();
}

void OpenVRClientTObj::updateMaterials()
{
   if ( isServerObject() || mLaserMatName.isEmpty() )
      return;

   // If the material instance exists and name matches then don't bother updating it.
   if ( !mLaserMatInst || !mLaserMatName.equal( mLaserMatInst->getMaterial()->getName(), String::NoCase ) )
   {
      SAFE_DELETE( mLaserMatInst );
      mLaserMatInst = MATMGR->createMatInstance( mLaserMatName, getGFXVertexFormat< VertexType >() );
      if ( !mLaserMatInst )
         Con::errorf( "OpenVRClientTObj::updateMaterials - no Material called '%s'", mLaserMatName.c_str() );
   }

   if ( !mLaserHoverMatInst || !mLaserHoverMatName.equal( mLaserHoverMatInst->getMaterial()->getName(), String::NoCase ) )
   {
      SAFE_DELETE( mLaserHoverMatInst );
      mLaserHoverMatInst = MATMGR->createMatInstance( mLaserHoverMatName, getGFXVertexFormat< VertexType >() );
      if ( !mLaserHoverMatInst )
         Con::errorf( "OpenVRClientTObj::updateMaterials - no Material called '%s'", mLaserHoverMatName.c_str() );
   }
}

DefineEngineMethod(OpenVRClientTObj, setModelName, void, (String modelName),,
   "Sets the openvr render model to load. Model name will be automatically assigned "
   "from the connected device properties. Use this to override the assigned model.")
{
   object->setModelName(modelName);
}

DefineEngineMethod( OpenVRClientTObj, resetDeviceIndex, bool, ( ),,
   "@brief Resets the input source handle and ovr device index. Call this after all devices are connected "
   "and any time controller roles change.\n" )
{
   return object->resetDeviceIndex();
}

DefineEngineMethod( OpenVRClientTObj, getDeviceIndex, S32, ( ),,
   "@brief Returns the device index for the tracked object or -1 if the device is not connected.\n" )
{
   return object->getDeviceIndex();
}

DefineEngineMethod( OpenVRClientTObj, getInteractObject, S32, ( ),,
   "@brief Get the current interact object or 0 if the laser pointer is inactive or not pointed at a SceneObject.\n" )
{
   SceneObject* mobj = object->getInteractObject();
   return mobj ? mobj->getId(): 0;
}
