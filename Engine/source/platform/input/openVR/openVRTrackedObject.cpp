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
#include "platform/input/openVR/openVRTrackedObject.h"
#include "platform/input/openVR/openVRProvider.h"

#include "math/mathIO.h"
#include "scene/sceneRenderState.h"
#include "console/consoleTypes.h"
#include "core/stream/bitStream.h"
#include "core/resourceManager.h"
#include "materials/materialManager.h"
#include "materials/baseMatInstance.h"
#include "renderInstance/renderPassManager.h"
#include "lighting/lightQuery.h"
#include "gfx/gfxTextureManager.h"
#include "gfx/gfxTransformSaver.h"
#include "gfx/sim/debugDraw.h"
#include "sim/netConnection.h"
#include "T3D/shapeBase.h"

#ifdef TORQUE_EXTENDED_MOVE
#include "T3D/gameBase/extended/extendedMove.h"
#endif

//-----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(OpenVRTrackedObjectData);

OpenVRTrackedObjectData::OpenVRTrackedObjectData() :
   mShapeFile( StringTable->EmptyString()),
   mTrackingScale(1.5f),
   mShapeOffset(TransformF::Identity),
   mShapeRot(QuatF::Identity),
   mDisconnectedTransform(TransformF::Identity)
{
}

OpenVRTrackedObjectData::~OpenVRTrackedObjectData()
{
}

bool OpenVRTrackedObjectData::onAdd()
{
   return Parent::onAdd();
}

bool OpenVRTrackedObjectData::preload(bool server, String &errorStr)
{
   if (!Parent::preload(server, errorStr))
      return false;

   if (!server)
   {
      mShape = mShapeFile ? ResourceManager::get().load(mShapeFile) : NULL;
   }

   if (mShapeOffset.hasRotation())
      mShapeRot = QuatF(mShapeOffset.mOrientation.axis,mShapeOffset.mOrientation.angle);

   return true;
}

void OpenVRTrackedObjectData::initPersistFields()
{
   addGroup("Tracked Object");
      addField("shape", TypeShapeFilename, Offset(mShapeFile, OpenVRTrackedObjectData),
         "Shape file to use for tracked model.");
      addField("trackingScale", TypeF32, Offset(mTrackingScale, OpenVRTrackedObjectData),
         "The maximum value for an axis component of the tracked position. This will clamp "
         "position data sent to remote clients to +/- the value.");
      addField( "renderOffset", TypeTransformF, Offset( mShapeOffset, OpenVRTrackedObjectData ), 
         "Offset from tracked position to use when rendering shape." );
      addField( "disconnectedOffset", TypeTransformF, Offset( mDisconnectedTransform, OpenVRTrackedObjectData ), 
         "Offset from player position to place this object when the tracked device is disconnected." );
   endGroup("Tracked Object");

   Parent::initPersistFields();
}

void OpenVRTrackedObjectData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeString(mShapeFile);
   stream->write(mTrackingScale);

   mathWrite(*stream, mShapeOffset.getPosition());
   mathWrite(*stream, mShapeOffset.getOrientation().axis);
   stream->write(mShapeOffset.getOrientation().angle);

   mathWrite(*stream, mDisconnectedTransform.getPosition());
   mathWrite(*stream, mDisconnectedTransform.getOrientation().axis);
   stream->write(mDisconnectedTransform.getOrientation().angle);
}

void OpenVRTrackedObjectData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   mShapeFile = stream->readSTString();
   stream->read(&mTrackingScale);

   Point3F pos, axis;
   F32 angle;
   mathRead(*stream, &pos);
   mathRead(*stream, &axis);
   stream->read(&angle);
   mShapeOffset.mPosition = pos;
   mShapeOffset.mOrientation.axis = axis;
   mShapeOffset.mOrientation.angle = angle;

   mathRead(*stream, &pos);
   mathRead(*stream, &axis);
   stream->read(&angle);
   mDisconnectedTransform.mPosition = pos;
   mDisconnectedTransform.mOrientation.axis = axis;
   mDisconnectedTransform.mOrientation.angle = angle;
}

//-----------------------------------------------------------------------------


IMPLEMENT_CO_NETOBJECT_V1(OpenVRTrackedObject);

ConsoleDocClass(OpenVRTrackedObject,
   "@brief Renders and handles interactions OpenVR controllers and tracked objects.\n\n"
   "This class implements basic rendering and interactions with OpenVR controllers.\n\n"
   "The object should be controlled by a player object. Controllers will be rendered at\n"
   "the correct position regardless of the current transform of the object.\n"
   "@ingroup OpenVR\n");


//-----------------------------------------------------------------------------
// Object setup and teardown
//-----------------------------------------------------------------------------
OpenVRTrackedObject::OpenVRTrackedObject() :
   mDataBlock(NULL),
   mShapeInstance(NULL),
   mPoseActionIndex(-1),
   mMappedMoveIndex(-1),
   mbConnected(false),
   mbOwnedByClient(false),
   mbRenderFirstPerson(true),
   mOwnerObject(NULL)
{
   // Flag this object so that it will always
   // be sent across the network to clients
   mNetFlags.set(Ghostable | ScopeAlways);

   // Set it as a "static" object that casts shadows
   mTypeMask |= StaticObjectType | StaticShapeObjectType;

   mPosition.zero();
   mRotation.identity();
   //mDelta.pos.zero();
   mDelta.posVec.zero();
   mDelta.rot.identity();
}

OpenVRTrackedObject::~OpenVRTrackedObject()
{
   SAFE_DELETE(mShapeInstance);
}

//-----------------------------------------------------------------------------
bool OpenVRTrackedObject::_createShape()
{
   if (!mDataBlock || !mDataBlock->mShape)
      return false;

   SAFE_DELETE(mShapeInstance);
   mShapeInstance = new TSShapeInstance( mDataBlock->mShape, isClientObject() );
   if (!mShapeInstance)
      return false;

   mObjBox = mDataBlock->mShape->mBounds;
   return true;
}

void OpenVRTrackedObject::getActionIndex()
{
   mPoseActionIndex = -1;
   if (!mPoseSourceStringHandle.isValidString() || !vr::VRInput())
      return;

   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      mPoseActionIndex = OPENVR->getPoseIndex(mPoseSourceStringHandle.getString());
}

//-----------------------------------------------------------------------------
// Object Editing
//-----------------------------------------------------------------------------
void OpenVRTrackedObject::initPersistFields()
{
   // SceneObject already handles exposing the transform
   Parent::initPersistFields();

   addGroup( "Tracking Data" );
      addField("mappedMoveIndex", TypeS32, Offset(mMappedMoveIndex, OpenVRTrackedObject),
         "Index of movemanager state to track");
      addProtectedField("poseSource", TypeRealString, Offset(mPoseSourceName, OpenVRTrackedObject),
         &_setFieldInputSource, &defaultProtectedGetFn,
         "The input source from the action manifest file. If the source is found, it will be used "
         "to provide the real-time transform when rendering on the controlling client. If not "
         "found or blank, interpolated move data will be used to set the render transform on the "
         "controlling client.");
      addProtectedField("renderFirstPerson", TypeBool, Offset(mbRenderFirstPerson, OpenVRTrackedObject),
         &_setFieldRenderFP, &defaultProtectedGetFn,
         "Should object render on the controlling client.");
   endGroup( "Tracking Data" );
}

bool OpenVRTrackedObject::_setFieldInputSource(void* object, const char* index, const char* data)
{
   OpenVRTrackedObject *tObj = static_cast<OpenVRTrackedObject*>( object );
   if ( tObj )
      tObj->setInputSource(data);
   return false;
}

bool OpenVRTrackedObject::_setFieldRenderFP(void* object, const char* index, const char* data)
{
   OpenVRTrackedObject *tObj = static_cast<OpenVRTrackedObject*>( object );
   if ( tObj )
      tObj->setRenderFirstPerson(dAtob(data));
   return false;
}

void OpenVRTrackedObject::setInputSource(const char* name)
{
   if ( !isGhost() )
   {
      if ( name[0] != '\0' )
      {
         if ( name[0] == StringTagPrefixByte )
            mPoseSourceStringHandle = NetStringHandle( U32(dAtoi(name + 1)) );
         else
            mPoseSourceStringHandle = NetStringHandle( name );
      }
      else
         mPoseSourceStringHandle = NetStringHandle();

      setMaskBits( UpdateMask );
   }
}

void OpenVRTrackedObject::setRenderFirstPerson(bool doRender)
{
   if ( !isGhost() && (mbRenderFirstPerson != doRender))
   {
      mbRenderFirstPerson = doRender;
      setMaskBits( UpdateMask );
   }
}

void OpenVRTrackedObject::inspectPostApply()
{
   Parent::inspectPostApply();

   // Flag the network mask to send the updates to the client object
   setMaskBits(UpdateMask);
}

bool OpenVRTrackedObject::onAdd()
{
   if (!Parent::onAdd())
      return false;

   mGlobalBounds = false;
   if (!mShapeInstance && !_createShape())
      return false;

   resetWorldBox();

   // Add this object to the scene
   addToScene();

   // We'll be ticked by the player we're attached to. Don't tick twice.
   setProcessTick(false);

   return true;
}

void OpenVRTrackedObject::onRemove()
{
   // Remove this object from the scene
   removeFromScene();

   Parent::onRemove();
}

bool OpenVRTrackedObject::onNewDataBlock(GameBaseData *dptr, bool reload)
{
   mDataBlock = dynamic_cast<OpenVRTrackedObjectData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr, reload))
      return false;

   mGlobalBounds = false;
   SAFE_DELETE(mShapeInstance);
   if (!_createShape())
      return false;

   resetWorldBox();

   scriptOnNewDataBlock();

   return true;
}

void OpenVRTrackedObject::setLocalRenderTransform()
{
   Point3F posVal;
   QuatF rotVal;
   MatrixF trackedMat(1);
   
   if (OPENVR->getCurrentPose(mPoseActionIndex, posVal, rotVal))
   {
      Point3F pos;
      rotVal.mulP(mDataBlock->mShapeOffset.mPosition, &pos);
      posVal += pos;

      // Apply the shapes render offset if any
      if (mDataBlock->mShapeOffset.hasRotation())
      {
         rotVal.inverse();
         rotVal *= mDataBlock->mShapeRot;
         rotVal.inverse();
      }
   }
   else
   {
      posVal = mDataBlock->mDisconnectedTransform.getPosition();
      rotVal = QuatF(mDataBlock->mDisconnectedTransform.getOrientation());

      if (mOwnerObject)
      {
         trackedMat = mOwnerObject->getRenderTransform();

         trackedMat.setColumn(3, Point3F::Zero);
         trackedMat.mulP(posVal);
         rotVal *= QuatF(trackedMat);
      }
   }

   rotVal.setMatrix(&trackedMat);
   trackedMat.setPosition(posVal);

   if (mOwnerObject)
   {
      MatrixF nodeMat;
      mOwnerObject->getVRCameraTransform(&nodeMat);
      trackedMat.mulL(nodeMat);
   }
   setRenderTransform(trackedMat);
}

U32 OpenVRTrackedObject::packUpdate(NetConnection *conn, U32 mask, BitStream *stream)
{
   // Allow the Parent to get a crack at writing its info
   U32 retMask = Parent::packUpdate(conn, mask, stream);

   if (stream->writeFlag(mask & InitialUpdateMask))
   {
      stream->write((S16)mMappedMoveIndex);
   }

   // Write our device information
   if (stream->writeFlag(mask & UpdateMask))
   {
      stream->writeFlag(mbRenderFirstPerson);
      conn->packNetStringHandleU( stream, mPoseSourceStringHandle );
   }

   // Don't send move data to the owning client
   if( stream->writeFlag(mOwnerObject && (conn == (NetConnection *) mOwnerObject->getControllingClient())) )
      return retMask;

   if( stream->writeFlag(!(mask & MoveMask)) )
      return retMask;

   // Write the current position and rotation
   stream->writeSignedFloat(mClampF(mPosition.x / mDataBlock->mTrackingScale, -1.0f, 1.0f), TrackedPosBits);
   stream->writeSignedFloat(mClampF(mPosition.y / mDataBlock->mTrackingScale, -1.0f, 1.0f), TrackedPosBits);
   stream->writeSignedFloat(mClampF(mPosition.z / mDataBlock->mTrackingScale, -1.0f, 1.0f), TrackedPosBits);
   stream->writeQuat(mRotation, TrackedQuatBits);
   return retMask;
}

void OpenVRTrackedObject::unpackUpdate(NetConnection *conn, BitStream *stream)
{
   // Let the Parent read any info it sent
   Parent::unpackUpdate(conn, stream);

   if (stream->readFlag())  // InitialUpdateMask
   {
      S16 readMoveIndex;
      stream->read(&readMoveIndex);
      mMappedMoveIndex = readMoveIndex;
   }

   if (stream->readFlag())  // UpdateMask
   {
      mbRenderFirstPerson = stream->readFlag();
      NetStringHandle newStringHandle = conn->unpackNetStringHandleU(stream);
      if (mPoseSourceStringHandle != newStringHandle)
      {
         mPoseSourceStringHandle = newStringHandle;
         getActionIndex();
      }
   }

   // Done if controlled by client
   mbOwnedByClient = stream->readFlag();
   if(mbOwnedByClient)
      return;

   if(stream->readFlag())
      return; // No movement data

   mDelta.posVec = mPosition;
   mDelta.rot = mRotation;

   mPosition.x = stream->readSignedFloat(TrackedPosBits) * mDataBlock->mTrackingScale;
   mPosition.y = stream->readSignedFloat(TrackedPosBits) * mDataBlock->mTrackingScale;
   mPosition.z = stream->readSignedFloat(TrackedPosBits) * mDataBlock->mTrackingScale;
   stream->readQuat(&mRotation, TrackedQuatBits);

   mDelta.posVec -= mPosition;
}

void OpenVRTrackedObject::prepRenderImage(SceneRenderState *state)
{
   if (mbOwnedByClient && !mbRenderFirstPerson)
      return;

   // Render the shape
   if (mShapeInstance)
   {
      // Calculate the distance of this object from the camera
      Point3F cameraOffset = mRenderObjToWorld.getPosition();
      cameraOffset -= state->getDiffuseCameraPosition();
      F32 dist = cameraOffset.len();
      if (dist < 0.01f)
      dist = 0.01f;

      // Set up the LOD for the shape
      F32 invScale = (1.0f / getMax(getMax(mObjScale.x, mObjScale.y), mObjScale.z));

      mShapeInstance->setDetailFromDistance(state, dist * invScale);

      // Make sure we have a valid level of detail
      if (mShapeInstance->getCurrentDetail() < 0)
         return;

      // GFXTransformSaver is a handy helper class that restores
      // the current GFX matrices to their original values when
      // it goes out of scope at the end of the function
      GFXTransformSaver saver;

      // Set up our TS render state
      TSRenderState rdata;
      rdata.setSceneState(state);
      rdata.setFadeOverride(1.0f);

      // We might have some forward lit materials
      // so pass down a query to gather lights.
      LightQuery query;
      query.init(getWorldSphere());
      rdata.setLightQuery(&query);

      // Set the world matrix to the objects render transform
      MatrixF mat = mRenderObjToWorld;

      mat.scale(mObjScale);
      GFX->setWorldMatrix(mat);

      mShapeInstance->animate();
      mShapeInstance->render(rdata);
   }
}

void OpenVRTrackedObject::updateMove(const Move *move)
{
   // Set transform based on move
#ifdef TORQUE_EXTENDED_MOVE
   const ExtendedMove* emove = dynamic_cast<const ExtendedMove*>(move);
   if (!emove)
      return;

   U32 emoveIndex = mMappedMoveIndex;
   if (emoveIndex >= ExtendedMove::MaxPositionsRotations)
      emoveIndex = 0;

   MatrixF trackedMat(1);
   mDelta.posVec = mPosition;
   mDelta.rot = mRotation;

   if (emove->DeviceIsActive[emoveIndex])
   {
      // Get absolute position/rotation from the move info
      mRotation = QuatF(emove->rotX[emoveIndex], emove->rotY[emoveIndex], emove->rotZ[emoveIndex], emove->rotW[emoveIndex]);
      mPosition = Point3F(emove->posX[emoveIndex], emove->posY[emoveIndex], emove->posZ[emoveIndex]);

      Point3F pos;
      mRotation.mulP(mDataBlock->mShapeOffset.mPosition, &pos);
      mPosition += pos;

      // Apply the shapes render offset if any
      if (mDataBlock->mShapeOffset.hasRotation())
      {
         mRotation.inverse();
         mRotation *= mDataBlock->mShapeRot;
         mRotation.inverse();
      }
   }
   else
   {
      mPosition = mDataBlock->mDisconnectedTransform.getPosition();
      mRotation = QuatF(mDataBlock->mDisconnectedTransform.getOrientation());

      if (mOwnerObject)
      {
         trackedMat = mOwnerObject->getTransform();

         trackedMat.setColumn(3, Point3F::Zero);
         trackedMat.mulP(mPosition);
         mRotation *= QuatF(trackedMat);
      }
   }

   mDelta.posVec -= mPosition;

   mRotation.setMatrix(&trackedMat);
   trackedMat.setPosition(mPosition);

   if (mOwnerObject)
   {
      MatrixF nodeMat;
      mOwnerObject->getVRCameraTransform(&nodeMat);
      trackedMat.mulL(nodeMat);
   }

   setTransform(trackedMat);

   if (isServerObject())
      setMaskBits(MoveMask);
#endif
}

void OpenVRTrackedObject::processTick(const Move *move)
{
   if (move)
   {
      updateMove(move);
   }

   Parent::processTick(move);
}

void OpenVRTrackedObject::interpolateTick(F32 delta)
{
   if (mbOwnedByClient && !mbRenderFirstPerson)
      return;  // This object will not be rendered.

   if (mbOwnedByClient && mPoseActionIndex != -1)
   {  // Set the render transform direct from the device
      setLocalRenderTransform();
      Parent::interpolateTick(delta);
      return;
   }

   // Use interpolated move data to set render transform
   MatrixF trackedMat(1);
   QuatF rot;
   rot.interpolate(mRotation, mDelta.rot, delta);
   Point3F pos = mPosition + (mDelta.posVec * delta);

   rot.setMatrix(&trackedMat);
   trackedMat.setPosition(pos);

   if (mOwnerObject)
   {
      MatrixF nodeMat;
      mOwnerObject->getVRCameraTransform(&nodeMat);
      trackedMat.mulL(nodeMat);
   }
   setTransform(trackedMat);

   Parent::interpolateTick(delta);
}

void OpenVRTrackedObject::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);
}
