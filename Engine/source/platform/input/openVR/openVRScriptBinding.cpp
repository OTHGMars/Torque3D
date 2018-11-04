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

#include "console/engineAPI.h"
#include "platform/input/openVR/openVRProvider.h"
#include "T3D/gameBase/gameConnection.h"
#include "math/mTransform.h"

DECLARE_SCOPE(OpenVR);
IMPLEMENT_SCOPE(OpenVR, OpenVRProvider, , "");
ConsoleDoc(
   "@class OpenVRProvider\n"
   "@brief This class is the interface between TorqueScript and OpenVR.\n\n"
   "@ingroup OpenVR\n"
);

// Enum impls

ImplementEnumType(OpenVROverlayInputMethod,
   "Types of input supported by VR Overlays. .\n\n"
   "@ingroup OpenVR")
{ vr::VROverlayInputMethod_None, "None" },
{ vr::VROverlayInputMethod_Mouse, "Mouse" },
{ vr::VROverlayInputMethod_DualAnalog, "DualAnalog" },
EndImplementEnumType;

ImplementEnumType(OpenVROverlayTransformType,
   "Allows the caller to figure out which overlay transform getter to call. .\n\n"
   "@ingroup OpenVR")
{ vr::VROverlayTransform_Absolute, "Absolute" },
{ vr::VROverlayTransform_TrackedDeviceRelative, "TrackedDeviceRelative" },
{ vr::VROverlayTransform_SystemOverlay, "SystemOverlay" },
{ vr::VROverlayTransform_TrackedComponent, "TrackedComponent" },
EndImplementEnumType;

ImplementEnumType(OpenVRGamepadTextInputMode,
   "Types of input supported by VR Overlays. .\n\n"
   "@ingroup OpenVR")
{ vr::k_EGamepadTextInputModeNormal, "Normal", },
{ vr::k_EGamepadTextInputModePassword, "Password", },
{ vr::k_EGamepadTextInputModeSubmit, "Submit" },
EndImplementEnumType;

ImplementEnumType(OpenVRGamepadTextInputLineMode,
   "Types of input supported by VR Overlays. .\n\n"
   "@ingroup OpenVR")
{ vr::k_EGamepadTextInputLineModeSingleLine, "SingleLine" },
{ vr::k_EGamepadTextInputLineModeMultipleLines, "MultipleLines" },
EndImplementEnumType;

ImplementEnumType(OpenVRTrackingResult,
   ". .\n\n"
   "@ingroup OpenVR")
{ vr::TrackingResult_Uninitialized, "None" },
{ vr::TrackingResult_Calibrating_InProgress, "Calibrating_InProgress" },
{ vr::TrackingResult_Calibrating_OutOfRange, "Calibrating_OutOfRange" },
{ vr::TrackingResult_Running_OK, "Running_Ok" },
{ vr::TrackingResult_Running_OutOfRange, "Running_OutOfRange" },
EndImplementEnumType;

ImplementEnumType(OpenVRTrackingUniverseOrigin,
   "Identifies which style of tracking origin the application wants to use for the poses it is requesting. .\n\n"
   "@ingroup OpenVR")
{ vr::TrackingUniverseSeated, "Seated" },
{ vr::TrackingUniverseStanding, "Standing" },
{ vr::TrackingUniverseRawAndUncalibrated, "RawAndUncalibrated" },
EndImplementEnumType;

ImplementEnumType(OpenVROverlayDirection,
   "Directions for changing focus between overlays with the gamepad. .\n\n"
   "@ingroup OpenVR")
{ vr::OverlayDirection_Up, "Up" },
{ vr::OverlayDirection_Down, "Down" },
{ vr::OverlayDirection_Left, "Left" },
{ vr::OverlayDirection_Right, "Right" },
EndImplementEnumType;

ImplementEnumType(OpenVRState,
   "Status of the overall system or tracked objects. .\n\n"
   "@ingroup OpenVR")
{ vr::VRState_Undefined, "Undefined" },
{ vr::VRState_Off, "Off" },
{ vr::VRState_Searching, "Searching" },
{ vr::VRState_Searching_Alert, "Searching_Alert" },
{ vr::VRState_Ready, "Ready" },
{ vr::VRState_Ready_Alert, "Ready_Alert" },
{ vr::VRState_NotReady, "NotReady" },
EndImplementEnumType;

ImplementEnumType(OpenVRTrackedDeviceClass,
   "Types of devices which are tracked .\n\n"
   "@ingroup OpenVR")
{ vr::TrackedDeviceClass_Invalid, "Invalid" },
{ vr::TrackedDeviceClass_HMD, "HMD" },
{ vr::TrackedDeviceClass_Controller, "Controller" },
{ vr::TrackedDeviceClass_GenericTracker, "GenericTracker" },
{ vr::TrackedDeviceClass_TrackingReference, "TrackingReference" },
{ vr::TrackedDeviceClass_DisplayRedirect, "Other" },
EndImplementEnumType;

ImplementEnumType(OpenVRControllerAxisType,
   "Types of controller axes.\n\n"
   "@ingroup OpenVR")
{ vr::k_eControllerAxis_None, "None" },
{ vr::k_eControllerAxis_TrackPad, "TrackPad" },
{ vr::k_eControllerAxis_Joystick, "Joystick" },
{ vr::k_eControllerAxis_Trigger, "Trigger" },
EndImplementEnumType;

ImplementEnumType(OpenVRTrackedControllerRole,
   "Types of controller roles.\n\n"
   "@ingroup OpenVR")
{ vr::TrackedControllerRole_Invalid, "Invalid" },
{ vr::TrackedControllerRole_LeftHand, "LeftHand" },
{ vr::TrackedControllerRole_RightHand, "RightHand" },
EndImplementEnumType;

ImplementEnumType(OpenVRActionType,
   "Types of controller roles.\n\n"
   "@ingroup OpenVR")
{ OpenVRActionType_Digital, "Digital" },
{ OpenVRActionType_Analog, "Analog" },
{ OpenVRActionType_Pose, "Pose" },
{ OpenVRActionType_Skeleton, "Skeleton" },
EndImplementEnumType;

DefineEngineStaticMethod(OpenVR, isHmdPresent, bool, (), ,
   "Returns true if there is an HMD attached. This check is as lightweight as possible and "
   "can be called outside of VR_Init/VR_Shutdown (OpenVR::setEnabled(true/false). It should "
   "be used when an application wants to know if initializing VR is a possibility but isn't "
   "ready to take that step yet.\n"
   "@ingroup OpenVR")
{
   Con::printf("Point3F: %d, QuatF: %d, TransformF: %d", sizeof(Point3F), sizeof(QuatF), sizeof(TransformF));
   return vr::VR_IsHmdPresent();
}

DefineEngineStaticMethod(OpenVR, getControllerModel, const char*, (S32 idx), ,
   "@brief Returns the model name for the device at the passed index.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return "";
   }

   return OPENVR->getControllerModel(idx);
}

DefineEngineStaticMethod(OpenVR, isDeviceActive, bool, (), ,
   "@brief Used to determine if the OpenVR input device is active\n\n"

   "The OpenVR device is considered active when the library has been "
   "initialized and either a real of simulated HMD is present.\n\n"
   "@return True if the OpenVR input device is active.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return false;
   }

   return OPENVR->getActive();
}


DefineEngineStaticMethod(OpenVR, setEnabled, bool, (bool value), ,
   "@brief Enable or disable OpenVR\n\n"
   "Enabling will initialize the vr interfaces, load the action manifest and begin "
   "polling for input. Disabling will stop input polling and call vr::VR_Shutdown().\n\n"
   "@return True if the OpenVR is enabled. False if it is disabled or there was an error enabling.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return false;
   }

   return value ? OPENVR->enable() : OPENVR->disable();
}


DefineEngineStaticMethod(OpenVR, setHMDAsGameConnectionDisplayDevice, bool, (GameConnection* conn), ,
   "@brief Sets the first HMD to be a GameConnection's display device\n\n"
   "@param conn The GameConnection to set.\n"
   "@return True if the GameConnection display device was set.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      Con::errorf("OpenVR::setHMDAsGameConnectionDisplayDevice(): No Open VR Device present.");
      return false;
   }

   if (!conn)
   {
      Con::errorf("OpenVR::setHMDAsGameConnectionDisplayDevice(): Invalid GameConnection.");
      return false;
   }

   conn->setDisplayDevice(OPENVR);
   return true;
}

DefineEngineStaticMethod(OpenVR, resetSensors, void, (), ,
   "@brief Sets the zero pose for the seated tracker coordinate system to the "
   "current position and yaw of the HMD.\n"
   "NOTE: This function overrides the user's previously saved seated zero pose "
   "and should only be called as the result of a user action. Users are also "
   "able to set their seated zero pose via the OpenVR Dashboard.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return;
   }

   OPENVR->resetSensors();
}

DefineEngineStaticMethod(OpenVR, setRoomTracking, void, (bool roomTracking), (true),
   "@brief Sets the tracking universe for OpenVR\n\n"
   "If room tracking is true, the standing tracking universe is used and "
   "poses are provided relative to the safe bounds configured by the user.\n"
   "If room tracking is false, the seated tracking universe is used and "
   "Poses are provided relative to the seated zero pose.\n\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return;
   }

   OPENVR->setRoomTracking(roomTracking);
}

DefineEngineFunction(OpenVRIsCompiledIn, bool, (), , "")
{
   return true;
}

DefineEngineStaticMethod(OpenVR, orientUniverse, void, (TransformF txfm), ,
   "Sets the yaw of the tracking universe in the T3D world. "
   "Pitch and roll from the passed transform are ignored.\n"
   "@param txfm object transform to set.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return;
   }

   if ( !txfm.hasRotation() )
      OPENVR->rotateUniverse(0.0f);
   else
      OPENVR->orientUniverse(txfm.getMatrix());
}

DefineEngineStaticMethod(OpenVR, rotateUniverse, void, (F32 yaw), (0.0f),
   "Sets the yaw of the tracking universe in the T3D world.\n"
   "@param yaw Tracking universe rotation about the z axis in radians.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return;
   }

   OPENVR->rotateUniverse(yaw);
}

DefineEngineStaticMethod(OpenVR, getDevicePropertyString, String, (U32 deviceIdx, U32 propID), ,
   "Returns a device property string value.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param propID The property id value from vr::ETrackedDeviceProperty. "
   "See: https://github.com/ValveSoftware/openvr/blob/ebdea152f8aac77e9a6db29682b81d762159df7e/headers/openvr.h#L229\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return String::EmptyString;
   }
   return OPENVR->getDevicePropertyString(deviceIdx, propID);
}

DefineEngineStaticMethod(OpenVR, getDevicePropertyBool, bool, (U32 deviceIdx, U32 propID), ,
   "Returns a device property boolean value.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param propID The property id value from vr::ETrackedDeviceProperty. "
   "See: https://github.com/ValveSoftware/openvr/blob/ebdea152f8aac77e9a6db29682b81d762159df7e/headers/openvr.h#L229\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return false;
   }
   return OPENVR->getDevicePropertyBool(deviceIdx, propID);
}

DefineEngineStaticMethod(OpenVR, getDevicePropertyInt, S32, (U32 deviceIdx, U32 propID), ,
   "Returns a device property int32 value.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param propID The property id value from vr::ETrackedDeviceProperty. "
   "See: https://github.com/ValveSoftware/openvr/blob/ebdea152f8aac77e9a6db29682b81d762159df7e/headers/openvr.h#L229\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return 0;
   }
   return OPENVR->getDevicePropertyInt(deviceIdx, propID);
}

DefineEngineStaticMethod(OpenVR, getDevicePropertyUInt, String, (U32 deviceIdx, U32 propID), ,
   "Returns a device property UInt64 value.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param propID The property id value from vr::ETrackedDeviceProperty. "
   "See: https://github.com/ValveSoftware/openvr/blob/ebdea152f8aac77e9a6db29682b81d762159df7e/headers/openvr.h#L229\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return String::EmptyString;
   }
   return OPENVR->getDevicePropertyUInt(deviceIdx, propID);
}

DefineEngineStaticMethod(OpenVR, getDevicePropertyFloat, F32, (U32 deviceIdx, U32 propID), ,
   "Returns a device property floating point value.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param propID The property id value from vr::ETrackedDeviceProperty. "
   "See: https://github.com/ValveSoftware/openvr/blob/ebdea152f8aac77e9a6db29682b81d762159df7e/headers/openvr.h#L229\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return 0.0f;
   }
   return OPENVR->getDevicePropertyFloat(deviceIdx, propID);
}

DefineEngineStaticMethod(OpenVR, getDeviceClass, String, (U32 deviceIdx), ,
   "Returns the device class for the openvr device at index deviceIdx.\n"
   "@param deviceIdx device to read property value for.\n"
   "@return One of the OpenVRTrackedDeviceClass enumeration values. "
   "(Invalid, HMD, Controller, GenericTracker, TrackingReference or Other)\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return String::EmptyString;
   }
   return OPENVR->getDeviceClass(deviceIdx);
}

DefineEngineStaticMethod(OpenVR, getControllerAxisType, String, (U32 deviceIdx, U32 axisID), ,
   "Marked depreciated in openvr 1.0.15.\n"
   "Returns an openvr controller axis type.\n"
   "@param deviceIdx device to read property value for.\n"
   "@param axisID ID of the axis..\n"
   "@return One of the OpenVRControllerAxisType enumeration values. "
   "(None, TrackPad, Joystick or Trigger)\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return String::EmptyString;
   }
   return OPENVR->getControllerAxisType(deviceIdx, axisID);
}

DefineEngineStaticMethod(OpenVR, getControllerRole, String, (U32 deviceIdx), ,
   "Marked depreciated in openvr 1.0.15.\n"
   "Returns The Role assigned to this controller at this index (LeftHand or RightHand). "
   "Invalid will be returned if there is not an active and valid controller device at the passed index.\n"
   "@ingroup OpenVR")
{
   if (!ManagedSingleton<OpenVRProvider>::instanceOrNull())
   {
      return String::EmptyString;
   }
   return OPENVR->getControllerRole(deviceIdx);
}

DefineEngineStaticMethod(OpenVR, addActionSet, S32, (const char* setName), ,
   "Adds an action set and saves the handle for it.\n\n"
   "@param setName The action set identifier from the action manifest .json file.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action set. "
   "The return value will be -1 if the action set could not be found by IVRInput.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addActionSet(setName);
   return -1;
}

DefineEngineStaticMethod(OpenVR, addAnalogAction, S32, (U32 setIndex, const char* actionName, const char* callbackFunc), ,
   "Adds an analog action, maps it's callback function and saves the handle for it.\n\n"
   "@param setIndex The action set index returned from addActionSet() that this action is added to.\n"
   "@param actionName The action name from the action manifest .json file.\n"
   "@param callbackFunc The function to call anytime the axes data changes.\n"
   " parameters: %controller, %xAxis, %yAxis, %zAxis\n\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by IVRInput. The integer "
   "identifier is needed to reference the action in calls to get updated glyphs and bindings from IVRInput.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addAnalogAction(setIndex, actionName, callbackFunc);
   return -1;
}

DefineEngineStaticMethod(OpenVR, addDigitalAction, S32, (U32 setIndex, const char* actionName, const char* callbackFunc), ,
   "Adds a digital action, maps it's callback function and saves the handle for it.\n\n"
   "@param setIndex The action set index returned from addActionSet() that this action is added to.\n"
   "@param actionName The action name from the action manifest .json file.\n"
   "@param callbackFunc The function to call when the input state changes.\n"
   " parameters: %controller, %state\n\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by IVRInput. The integer "
   "identifier is needed to reference the action in calls to get updated glyphs and bindings.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addDigitalAction(setIndex, actionName, callbackFunc);
   return -1;
}

DefineEngineStaticMethod(OpenVR, addPoseAction, S32,
   (U32 setIndex, const char* actionName, const char* poseCallback, const char* velocityCallback, S32 moveIndex), ( "", "", -1 ),
   "Adds a device pose action, maps it's callback function and saves the handle for it.\n\n"
   "@param setIndex The action set index returned from addActionSet() that this action is added to.\n"
   "@param actionName The action name from the action manifest .json file.\n"
   "@param poseCallback The function to call with updated position and velocity data.\n"
   " parameters: %controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot\n\n"
   "@param velocityCallback The function to call with updated linear and angular velocity data.\n"
   " parameters: %controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel\n\n"
   "@param moveIndex If set, the position and rotation will be assigned into the extended move "
   "at this index.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by IVRInput. The integer "
   "identifier is needed to reference the action in calls to get updated glyphs and bindings.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addPoseAction(setIndex, actionName, poseCallback, velocityCallback, moveIndex);
   return -1;
}

DefineEngineStaticMethod(OpenVR, setPoseCallbacks, bool,
(S32 poseIndex, const char* poseCallback, const char* velocityCallback), ( "", "" ),
"Resets the callbacks for a pose action. i.e. turning on and off the velocity "
"callback depending on object held.\n\n"
"@param poseIndex The index value returned from addPoseAction().\n"
"@param poseCallback The function to call with updated position and velocity data.\n"
" parameters: %controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot\n\n"
"@param velocityCallback The function to call with updated linear and angular velocity data.\n"
" parameters: %controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel\n\n"
"@return True if the pose was found and updated. False otherwise.\n"
"@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->setPoseCallbacks(poseIndex, poseCallback, velocityCallback);
   return false;
}

DefineEngineStaticMethod(OpenVR, addSkeletalAction, S32,
(U32 setIndex, const char* actionName, S32 moveIndex), ( -1 ),
"Adds a hand skeleton action, maps it to a move manager index and saves the handle for it.\n\n"
"@param setIndex The action set index returned from addActionSet() that this action is added to.\n"
"@param actionName The action name from the action manifest .json file.\n"
"@param moveIndex The skeleton will be assigned into the extended move manager as a "
"binary blob at this index.\n"
"@return The integer identifier to be used in subsequent calls to reference the action. "
"The return value will be -1 if the action could not be found by IVRInput. The integer "
"identifier is needed to reference the action in calls to get updated glyphs and bindings.\n"
"@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addSkeletalAction(setIndex, actionName, moveIndex);
   return -1;
}

DefineEngineStaticMethod(OpenVR, setSkeletonMode, S32,
(S32 skeletonIndex, bool withController), ( true ),
"Toggles the skeleton mode between ranged with controller and without.\n\n"
"@param skeletonIndex The index value returned from addSkeletalAction().\n"
"@param withController True to use vr::VRSkeletalMotionRange_WithController.\n"
"False to use vr::VRSkeletalMotionRange_WithoutController."
"@return True if the skeleton action was found and updated. False otherwise.\n"
"@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->setSkeletonMode(skeletonIndex, withController);
   return false;
}

DefineEngineStaticMethod(OpenVR, addHapticOutput, S32, (const char* outputName), ,
   "Loads the event handle for a vr controller haptic output event.\n\n"
   "@param outputName The vibration event name from the action manifest .json file.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by IVRInput. The integer "
   "identifier is needed to reference the action in calls to triggerHapticEvent().\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->addHapticOutput(outputName);
   return -1;
}

DefineEngineStaticMethod(OpenVR, triggerHapticEvent, bool, (U32 actionIndex,
   float fStartSecondsFromNow, float fDurationSeconds, float fFrequency, float fAmplitude), ,
   "Loads the event handle for a vr controller haptic output event.\n\n"
   "@param outputName The vibration event name from the action manifest .json file.\n"
   "@return The integer identifier to be used in subsequent calls to reference the action. "
   "The return value will be -1 if the action could not be found by IVRInput. The integer "
   "identifier is needed to reference the action in calls to triggerHapticEvent().\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->triggerHapticEvent(actionIndex, fStartSecondsFromNow, fDurationSeconds, fFrequency, fAmplitude);
   return false;
}

DefineEngineStaticMethod(OpenVR, activateActionSet, bool, (S32 controllerIndex, U32 setIndex), ,
   "Activate the specified action set on one or both vr controllers.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the action set activated on all controllers.\n"
   "@param setIndex The index value that was returned from addActionSet().\n"
   "@return Returns true if the set was activated, false if it could not be found.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->activateActionSet(controllerIndex, setIndex);
   return false;
}

DefineEngineStaticMethod(OpenVR, pushActionSetLayer, bool, (S32 controllerIndex, U32 setIndex), ,
   "Activate the specified action set as the highest priority set on the stack.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the action set activated on all controllers.\n"
   "@param setIndex The index value that was returned from addActionSet().\n"
   "@return Returns true if the set was activated, false if it could not be found or "
   "if there are already the maximum number of layers active (default 5).\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->pushActionSetLayer(controllerIndex, setIndex);
   return false;
}

DefineEngineStaticMethod(OpenVR, popActionSetLayer, bool, (S32 controllerIndex, U32 setIndex), ,
   "Removes the specified action set from the stack and deactivates it's actions. "
   "You cannot pop the last action set layer, use activateActionSet() to replace it.\n\n"
   "@param controllerIndex Zero-based index of the controller 0 - getNumControllers()-1. "
   "The value -1 may be passed to have the action set activated on all controllers.\n"
   "@param setIndex The index value that was returned from addActionSet().\n"
   "@return Returns true if the set was deactivated, false if there was an error.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->popActionSetLayer(controllerIndex, setIndex);
   return false;
}

DefineEngineStaticMethod(OpenVR, showActionOrigins, void, (U32 setIndex, OpenVRActionType actionType, U32 actionIndex), ,
   "Shows the current binding for the action in-headset. \"At the moment this "
   "function shows the entire binding UI, but that behavior will likely change down the road.\"\n\n"
   "@param setIndex The index value that was returned from addActionSet().\n"
   "@param actionType The type of action \"Analog\", \"Digital\", \"Pose\" or \"Skeletal\".\n"
   "@param setIndex The index value that was returned from add...Action().\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->showActionOrigins(setIndex, actionType, actionIndex);
}

DefineEngineStaticMethod(OpenVR, showActionSetBinds, void, (U32 setIndex), ,
   "Shows the current binding for all of the actions in the specified action set. "
   "\"At the moment this function shows the entire binding UI, but that behavior "
   "will likely change down the road.\"\n\n"
   "@param setIndex The index value that was returned from addActionSet().\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->showActionSetBinds(setIndex);
}

DefineEngineStaticMethod(OpenVR, setHMDTrackingHeight, void, (F32 hmdHeight), ,
   "Sets the tracking height offset for the hmd. Useful for mapping the standing "
   "tracking space to your character height.\n\n"
   "@param hmdHeight The upright HMD height.\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      OPENVR->mStandingHMDHeight = hmdHeight;
}

DefineEngineStaticMethod(OpenVR, getHMDTrackingHeight, F32, (), ,
   "Gets the current tracking height offset for the hmd.\n\n"
   "@ingroup OpenVR")
{
   if (ManagedSingleton<OpenVRProvider>::instanceOrNull())
      return OPENVR->mStandingHMDHeight;
   return 0.0f;
}
