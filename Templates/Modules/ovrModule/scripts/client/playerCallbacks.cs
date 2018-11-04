//-----------------------------------------------------------------------------
// Callback functions for Action Set Player
//-----------------------------------------------------------------------------

// Player Move deadzones.
$PMove_XDZ = 0.5;
$PMove_YDZ = 0.05;
$PMove_OneOXRange = 1 / (1 - $PMove_XDZ);
$PMove_OneOYRange = 1 / (1 - $PMove_YDZ);

// Player Rotation deadzone.
$PRot_XDZ = 0.05;
$PRot_OneOXRange = 1 / (1 - $PMove_XDZ);

function filterPlayerMoveX(%xAxis)
{
   if (mAbs(%xAxis) < $PMove_XDZ)
      %xAxis = 0;
   else
   {
      if (%xAxis < 0)
         %xAxis = (%xAxis + $PMove_XDZ) * $PMove_OneOXRange;
      else
         %xAxis = (%xAxis - $PMove_XDZ) * $PMove_OneOXRange;
   }

   return %xAxis;
}

function filterPlayerMoveY(%yAxis)
{
   if (mAbs(%yAxis) < $PMove_YDZ)
      %yAxis = 0;
   else
   {
      if (%yAxis < 0)
         %yAxis = (%yAxis + $PMove_YDZ) * $PMove_OneOYRange;
      else
         %yAxis = (%yAxis - $PMove_YDZ) * $PMove_OneOYRange;
   }

   return %yAxis;
}

function in_player_move_joy(%controller, %xAxis, %yAxis, %zAxis)
{  // Simple joystick movement
   //echo("in_player_move_joy( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");
   $OpenVR::LastPMoveX = filterPlayerMoveX(%xAxis);
   $OpenVR::LastPMoveY = filterPlayerMoveY(%yAxis);
   OVRUpdatePMove(true, $OpenVR::LastPMoveX, $OpenVR::LastPMoveY);
}

function in_player_move_touch(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_player_move_touch( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");
   $OpenVR::LastPMoveX = filterPlayerMoveX(%xAxis);
   $OpenVR::LastPMoveY = filterPlayerMoveY(%yAxis);
   OVRUpdatePMove($OpenVR::PMovePressed, $OpenVR::LastPMoveX, $OpenVR::LastPMoveY);
}

function in_player_move_press(%controller, %state)
{
   $OpenVR::PMovePressed = %state;
   OVRUpdatePMove($OpenVR::PMovePressed, $OpenVR::LastPMoveX, $OpenVR::LastPMoveY);
}

function filterPlayerRotation(%xAxis)
{
   if (mAbs(%xAxis) < $PRot_XDZ)
      %xAxis = 0;
   else
   {
      if (%xAxis < 0)
         %xAxis = (%xAxis + $PRot_XDZ) * $PRot_OneOXRange;
      else
         %xAxis = (%xAxis - $PRot_XDZ) * $PRot_OneOXRange;
   }
   return %xAxis;
}

function in_player_rot_touch(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_player_rot_touch( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");

   $OpenVR::LastPRotX = filterPlayerRotation(%xAxis);
   $OpenVR::LastPRotY = %yAxis;
   OVRUpdatePRot($OpenVR::PRotPressed, $OpenVR::LastPRotX, $OpenVR::LastPRotY);
}

function in_player_rot_joy(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_player_rot_touch( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");

   $OpenVR::LastPRotX = filterPlayerRotation(%xAxis);
   $OpenVR::LastPRotY = %yAxis;
   OVRUpdatePRot(true, $OpenVR::LastPRotX, $OpenVR::LastPRotY);
}

function in_player_rot_press(%controller, %state)
{
   $OpenVR::PRotPressed = %state;
   OVRUpdatePRot($OpenVR::PRotPressed, $OpenVR::LastPRotX, $OpenVR::LastPRotY);
}

function in_player_jump(%controller, %state)
{
   //echo("in_player_jump( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount2++;
}

function in_player_crouch(%controller, %state)
{
   //echo("in_player_crouch( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount3++;
}

function in_player_sprint(%controller, %state)
{
   //echo("in_player_sprint( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount5++;
}

function in_player_weapup(%controller, %state)
{
   //echo("in_player_weapup( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('cycleWeapon', "next");
}

function in_player_weapdown(%controller, %state)
{
   //echo("in_player_weapdown( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('cycleWeapon', "prev");
}

function in_player_weapreload(%controller, %state)
{
   //echo("in_player_weapreload( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('reloadWeapon');
}

function in_player_tossweapon(%controller, %state)
{
   //echo("in_player_tossweapon( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('Throw', "Weapon");
}

function in_player_tossammo(%controller, %state)
{
   //echo("in_player_tossammo( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('Throw', "Ammo");
}

function in_player_carjack(%controller, %state)
{
   //echo("in_player_carjack( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      carjack();
}

function in_player_zoom(%controller, %state)
{
   //echo("in_player_zoom( " @ %controller @ ", " @ %state @ ")");
   toggleZoom(%state);
}

function in_player_interact(%controller, %state)
{
   echo("in_player_interact( " @ %controller @ ", " @ %state @ ")");
}

function in_player_menu(%controller, %state)
{
   //echo("in_player_menu( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      OVROverlayCanvas.showCanvas(true);
}

function in_any_fire(%controller, %state)
{
   //echo("in_any_fire( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount0++;
   //if (%state)
      //ovrModule.triggerHapticEvent("Player", "player_recoil", 0.5, 1.5, 4, 1);
}

function in_any_altfire(%controller, %state)
{
   //echo("in_any_altfire( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount1++;
}

function in_any_camtoggle(%controller, %state)
{
   //echo("in_any_camtoggle( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      ServerConnection.setFirstPerson(!ServerConnection.isFirstPerson());
}

function in_any_freelook(%controller, %state)
{
   //echo("in_any_freelook( " @ %controller @ ", " @ %state @ ")");
   $mvFreeLook = %state;
}

$VRMovementSpeed = 1.0;
function OVRUpdatePMove(%pressed, %x, %y)
{
   //echo("OVRUpdateMove(" @ %pressed @ ", " @ %x @ ", " @ %y @ ")");
   if (%pressed)
   {
      $mvRightAction = %x > 0 ? %x * $VRMovementSpeed : 0;
      $mvLeftAction = %x < 0 ? -%x * $VRMovementSpeed : 0;
      $mvForwardAction = %y > 0 ? %y * $VRMovementSpeed : 0;
      $mvBackwardAction = %y < 0 ? -%y * $VRMovementSpeed : 0;
   }
   else
   {
      $mvRightAction = 0;
      $mvLeftAction = 0;
      $mvForwardAction = 0;
      $mvBackwardAction = 0;
   }
}

function OVRUpdatePRot(%pressed, %x, %y)
{
   //echo("OVRUpdateRightPad(" @ %pressed @ ", " @ %x @ ", " @ %y @ ")");
   if (%pressed)
   {
      if ($OpenVR::HMDRotateYawWithMoveActions)
      {
         $mvYawLeftSpeed = %x > 0 ? %x * 0.02 : 0;
         $mvYawRightSpeed = %x < 0 ? -%x * 0.02 : 0;
      }
   }
   else
   {
      $mvYawLeftSpeed = 0;
      $mvYawRightSpeed = 0;
   }
}
