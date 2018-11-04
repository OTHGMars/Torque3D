//-----------------------------------------------------------------------------
// OpenVRInput setup
//-----------------------------------------------------------------------------

/// addActionSet - Looks up the set handle from steamvr, saves the handle and a
/// tag for reference from script when activating or adding actions.
/// %setTag - The tag string that will be used to identify the set in script calls.
/// %setName - The name exactly as it appears in the action manifest file. 
function ovrModule::addActionSet(%this, %setTag, %setName)
{
   %this.ActionSet[%setTag] = OpenVR::addActionSet(%setName);
   %this.setNames[%setTag] = %setName;
   %this.analogActions[%setTag] = 0;
   %this.digitalActions[%setTag] = 0;

   %this.setTag[%this.actionSets] = %setTag;
   %this.actionSets++;

   if (%this.ActionSet[%setTag] == -1)
      warn("Action Set: " @ %setName @ " Not found!");
}

/// addAnalogAction - Looks up the analog action and attaches the callback.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %callback - The script function that will be called when this action is
/// active and has input. Analog callbacks get the following parameters:
/// %controller, %xAxis, %yAxis, %zAxis. Unused axes will be 0 for inputs that
/// use less than 3 vectors.
function ovrModule::addAnalogAction(%this, %setTag, %actionName, %callback)
{
   %actionDef = %this.setNames[%setTag] @ "/in/" @ %actionName;
   %this.analogAction[%setTag, %actionName] =
         OpenVR::addAnalogAction(%this.ActionSet[%setTag], %actionDef, %callback);

   %this.analogTag[%setTag, %this.analogActions[%setTag]] = %actionName;
   %this.analogActions[%setTag]++;

   if (%this.analogAction[%setTag, %actionName] == -1)
      warn("Analog Action: " @ %actionName @ " Not found!");
   if (!isFunction(%callback))
      warn("Callback function: " @ %callback @ " For " @ %setTag SPC %actionName @ " Not found!");
}

/// addDigitalAction - Looks up the digital action and attaches the callback.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %callback - The script function that will be called when this action is
/// active and has input. Digital callbacks get the following parameters:
/// %controller, %state bool (pressed/released).
function ovrModule::addDigitalAction(%this, %setTag, %actionName, %callback)
{
   %actionDef = %this.setNames[%setTag] @ "/in/" @ %actionName;
   %this.digitalAction[%setTag, %actionName] =
         OpenVR::addDigitalAction(%this.ActionSet[%setTag], %actionDef, %callback);

   %this.digitalTag[%setTag, %this.digitalActions[%setTag]] = %actionName;
   %this.digitalActions[%setTag]++;

   if (%this.digitalAction[%setTag, %actionName] == -1)
      warn("Digital Action: " @ %actionName @ " Not found!");
   if (!isFunction(%callback))
      warn("Callback function: " @ %callback @ " For " @ %setTag SPC %actionName @ " Not found!");
}

/// addPoseAction - Looks up the pose action and attaches the callbacks or move.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %poseCallback - The script function that will be called with updated
///  position and rotation data. Pose callbacks get the following parameters:
///  %controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot.
/// %velocityCallback - The script function to call with updated linear and
///  angular velocity data. Velocity callbacks get the following parameters:
///  %controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel
/// %moveIndex - Position and rotation data will be automatically written into
///  the extended move manager if set to a valid device index 0 - 2.
/// Either or both callbacks can be empty if the pose is automatically written
/// to the move manager.
function ovrModule::addPoseAction(%this, %setTag, %actionName, %poseCallback, %velocityCallback, %moveIndex)
{
   if (%moveIndex $= "")
      %moveIndex = -1;

   %actionDef = %this.setNames[%setTag] @ "/in/" @ %actionName;
   %this.poseAction[%setTag, %actionName] =
         OpenVR::addPoseAction(%this.ActionSet[%setTag], %actionDef, %poseCallback, %velocityCallback, %moveIndex);

   %this.poseTag[%setTag, %this.poseActions[%setTag]] = %actionName;
   %this.poseActions[%setTag]++;

   if (%this.poseAction[%setTag, %actionName] == -1)
      warn("Pose Action: " @ %actionName @ " Not found!");
}

/// addSkeletalAction - Looks up the skeleton action and attaches move index.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %moveIndex - The move manager device index to use for the data. 0 - 2.
function ovrModule::addSkeletalAction(%this, %setTag, %actionName, %moveIndex)
{
   if (%moveIndex $= "")
      %moveIndex = -1;

   %actionDef = %this.setNames[%setTag] @ "/in/" @ %actionName;
   %this.skeletalAction[%setTag, %actionName] =
         OpenVR::addSkeletalAction(%this.ActionSet[%setTag], %actionDef, %moveIndex);

   %this.skeletalTag[%setTag, %this.skeletalActions[%setTag]] = %actionName;
   %this.skeletalActions[%setTag]++;
}

/// addHapticEvent - Looks up a haptic event and stores the handle.
/// %setTag - The action set that the action is listed with.
/// %actionName - The vibration event name from the action manifest .json file.
function ovrModule::addHapticEvent(%this, %setTag, %actionName)
{
   %actionDef = %this.setNames[%setTag] @ "/out/" @ %actionName;
   %this.hapticEvents[%setTag, %actionName] =
         OpenVR::addHapticOutput(%actionDef);

   if (%this.hapticEvents[%setTag, %actionName] == -1)
      warn("Haptic Event: " @ %actionName @ " Not found!");
}

/// triggerHapticEvent - Triggers the event with the passed parameters
/// %setTag - The action set that the action is listed with.
/// %actionName - The vibration event name from the action manifest .json file.
/// %delay - Seconds before event begins.
/// %duration - Durration of vibration (seconds).
/// %frequency - The frequency in cycles per second of the haptic event (Hz).
/// %amplitude - The magnitude of the haptic event. This value must be between 0.0 and 1.0.
function ovrModule::triggerHapticEvent(%this, %setTag, %actionName, %delay,
         %duration, %frequency, %amplitude)
{
   OpenVR::triggerHapticEvent(%this.hapticEvents[%setTag, %actionName],
      %delay, %duration, %frequency, %amplitude);
}

/// setPoseCallbacks - Changes the callback functions associated with a pose action.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %poseCallback - The script function that will be called with updated
///  position and rotation data. Pose callbacks get the following parameters:
///  %controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot.
/// %velocityCallback - The script function to call with updated linear and
///  angular velocity data. Velocity callbacks get the following parameters:
///  %controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel
function ovrModule::setPoseCallbacks(%this, %setTag, %actionName, %poseCallback, %velocityCallback)
{
   OpenVR::setPoseCallbacks(%this.poseAction[%setTag, %actionName],
      %poseCallback, %velocityCallback);
}

/// setSkeletonMode - Sets the base pose for the hand skeleton.
/// %setTag - The action set that the action is listed with.
/// %actionName - The name that appears in the action manifest file.
/// %withController - True for hand pose with controller, false for without.
function ovrModule::setSkeletonMode(%this, %setTag, %actionName, %withController)
{
   OpenVR::setSkeletonMode(%this.skeletalAction[%setTag, %actionName], %withController);
}

/// activateActionSet - Activates the action set on the controller.
/// %setTag - The action set tag that was passed to addActionSet.
/// %controller - The controller to activate the input on. Pass an empty string
/// or -1 to have the set activated on all controllers.
function ovrModule::activateActionSet(%this, %setTag, %controller)
{
   if (%controller $= "")
      %controller = -1; // All controllers
   clearAllMoves();
   if (OpenVR::activateActionSet(%controller, %this.ActionSet[%setTag]))
      $InputMap::CurrentActionSet = %setTag;

   if ($IsTrackedDemo)
      OpenVR::pushActionSetLayer(%controller, %this.ActionSet["Demo"]);
}

/// pushActionSetLayer - Activate the specified action set as the highest 
///   priority set on the stack.
/// %setTag - The action set tag that was passed to addActionSet.
/// %controller - The controller to activate the input on. Pass an empty string
/// or -1 to have the set activated on all controllers.
function ovrModule::pushActionSetLayer(%this, %setTag, %controller)
{
   if (%controller $= "")
      %controller = -1; // All controllers
   clearAllMoves();
   OpenVR::pushActionSetLayer(%controller, %this.ActionSet[%setTag]);
}

/// popActionSetLayer - Removes the specified action set from the stack and
///   deactivates it's actions. You cannot pop the last action set layer, use 
///   activateActionSet() with a different set to replace it.
/// %setTag - The action set tag that was passed to addActionSet.
/// %controller - The controller to activate the input on. Pass an empty string
/// or -1 to have the set activated on all controllers.
function ovrModule::popActionSetLayer(%this, %setTag, %controller)
{
   if (%controller $= "")
      %controller = -1; // All controllers
   clearAllMoves();
   OpenVR::popActionSetLayer(%controller, %this.ActionSet[%setTag]);
}

/// showBindsForAction - Shows the current binding for the action in-headset.
/// %type - 'Digital', 'Analog' or 'Pose' action.
/// %controller - The controller to get binding for.
/// %setTag - Tag name passed to addActionSet when the set was added.
/// %actionName - The action name.
function ovrModule::showBindsForAction(%this, %type, %controller, %setTag, %actionName)
{
   %setId = %this.ActionSet[%setTag];
   switch$(%type)
   {
      case "Digital":
         %actionId = %this.digitalAction[%setTag, %actionName];
      case "Analog":
         %actionId = %this.analogAction[%setTag, %actionName];
      case "Pose":
         %actionId = %this.poseAction[%setTag, %actionName];
   }
   OpenVR::showActionOrigins(%setId, %type, %actionId);
}

/// showActionSetBinds - Shows the current binding for all of the actions in the
/// specified action set.
/// %setTag - Tag name passed to addActionSet when the set was added.
function ovrModule::showActionSetBinds(%this, %setTag)
{
   %setId = %this.ActionSet[%setTag];
   if (%setId $= "")
      %setId = 0;
   OpenVR::showActionSetBinds(%setId);
}

/// assignHands - Initializes hand objects and assigns/updates the device index.
/// %leftIdx - The device index of the left hand controller
/// %rightIdx - The device index of the right hand controller
function ovrModule::assignHands(%this, %leftIdx, %rightIdx)
{
   if (!$IsTrackedDemo)
      return;

   // Create the client side hand tracker objects
   if (%rightIdx !$= "")
   {
      %controllerModel = OpenVR::getDevicePropertyString(%rightIdx, 1001);
      echo("Right Controller: " @ %controllerModel);
      CreateVR_RHTracker(%rightIdx, %controllerModel);
   }
   if (%leftIdx !$= "")
   {
      %controllerModel = OpenVR::getDevicePropertyString(%leftIdx, 1001);
      echo("Left Controller: " @ %controllerModel);
      CreateVR_LHTracker(%leftIdx, %controllerModel);
   }
}

/// showCurrentBinds - Display the controller binding panel in the headset.
function ovrModule::showCurrentBinds(%this)
{
   %this.showActionSetBinds($InputMap::CurrentActionSet);
}

/// resetUniverse - Resets the seated zero pose or opens the height calibration dialog.
function ovrModule::resetUniverse(%this)
{
   if ($pref::OVR::Seated)
      OpenVR::resetSensors();
   else
   {
      $GameCanvas.pushDialog(OpenVRHeightDlg);
   }
}

function clearAllMoves()
{
   $mvForwardAction = 0;
   $mvBackwardAction = 0;
   $mvYawLeftSpeed = 0;
   $mvYawRightSpeed = 0;
   $mvPitchDownSpeed = 0;
   $mvPitchUpSpeed = 0;
}
