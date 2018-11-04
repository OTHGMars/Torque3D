//-----------------------------------------------------------------------------
// OpenVR core system functions.
//-----------------------------------------------------------------------------

// Call this function from GameConnection::initialControlSet() just before
// your "Canvas.setContent(PlayGui);" call, or at any time you wish to switch
// to rendering in the HMD.
// Parameters:
// %gameConnection - The client GameConnection instance
// %trueStereoRendering - If true, each eye will be rendered and submitted to
// the compositor separately. If false, both eyes will be rendered side-by-side
// on a wide texture and submitted to the compositor together.
function enableOpenVRDisplay(%gameConnection, %trueStereoRendering)
{
   OpenVR::setHMDAsGameConnectionDisplayDevice(%gameConnection);
   PlayGui.renderStyle = %trueStereoRendering ? "stereo separate" : "stereo side by side";

   VRSetupOverlay();

   //OpenVR::resetSensors();

   setupDefaultOpenVRBinds(MoveMap);
   setupOVRControllers();
   $OpenVR::HMDRotateYawWithMoveActions = $pref::OVR::Seated; // jamesu - allow gamepad to rotate view
}

// Call this function when ever you wish to disable display to HMD
function disableOpenVRDisplay(%gameConnection)
{
   if (isObject(OVROverlayCanvas) && ($GameCanvas == OVROverlayCanvas))
   {
      OVROverlayCanvas.popDialog();
      OVROverlayCanvas.hideOverlay();
   }
   $GameCanvas = Canvas;

   if (isObject(%gameConnection))
   {
      %gameConnection.clearDisplayDevice();
   }
   PlayGui.renderStyle = "standard";

   if (isObject(OVR_LHTracker))
      OVR_LHTracker.delete();
   if (isObject(OVR_RHTracker))
      OVR_RHTracker.delete();
}

// Reset the seated zero pose.
function resetOpenVRSensors()
{
   OpenVR::resetSensors();
}

function initOpenVR(%enableVR)
{
   if ( %enableVR )
   {
      if (!$Video::VREnabled)
      {
         OpenVR::setEnabled(true);
         $Video::VREnabled = OpenVR::isDeviceActive();
      }
      if ($Video::VREnabled)
      {
         OpenVR::setRoomTracking($pref::OVR::Standing);
      }
   }
   else
   {
      if ($Video::VREnabled)
      {
         OpenVR::setEnabled(false);
         $Video::forceDisplayAdapter = "";
         $Video::VREnabled = false;
      }
   }
}

function GameBase::setControl(%this, %isControlled, %client)
{  // This is a client side callback when an objects controlling client is set
   if ($Video::VREnabled && (ServerConnection.getId() == %client))
   {
      %controlClass = %this.getClassName();
      if (%isControlled && (%controlClass !$= "Player"))
      {  // Controling an object that's not a player. Setup for fixed eye node.
         $OpenVR::HMDRotateYawWithMoveActions = false;
         OpenVR::rotateUniverse(0);
      }
      else
      {  // We'll only get this callback for the player when client control is 
         // initially assigned. We get the control release for any objects that
         // were mounted instead of reassignment to the player.
         if (%controlClass $= "Player")
            %xfm = %this.getTransform();
         else
            %xfm = isObject(ServerConnection.getControlObject()) ? ServerConnection.getControlObject().getTransform() : "0 0 0";
         OpenVR::orientUniverse(%xfm);
         $OpenVR::HMDRotateYawWithMoveActions = $pref::OVR::Seated;

         if ($pref::OVR::Standing)
         {
            if ($pref::OVR::HMDHeight $= "")
               $GameCanvas.pushDialog(OpenVRHeightDlg);
            else
               OpenVR::setHMDTrackingHeight($pref::OVR::HMDHeight);
         }
      }
      ServerConnection.setFirstPerson(true);
   }
}

function clientCmdOrientVR(%xfm)
{
   if (%xfm $= "")
      %xfm = "0 0 0";
   OpenVR::orientUniverse(%xfm);
}

// HMD Pose callback
function onHMDPose(%position, %rotation, %velocity, %angVel)
{
   //echo("onHMDPose(" @ %position@", "@%rotation@", "@%velocity@", "@%angVel@")");
   $mvDeviceIsActive0 = true;

   $mvPosX0 = %position.x;
   $mvPosY0 = %position.y;
   $mvPosZ0 = %position.z;

   $mvRotX0 = %rotation.x;
   $mvRotY0 = %rotation.y;
   $mvRotZ0 = %rotation.z;
   $mvRotW0 = %rotation.w;
}
