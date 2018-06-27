//-----------------------------------------------------------------------------
// Callback functions for Action Set PlayerControls
//-----------------------------------------------------------------------------

function si_player_move(%controller, %xAxis, %yAxis)
{
   //echo("si_player_move( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");
   if(%xAxis > 0)
   {
      $mvRightAction = %xAxis * $movementSpeed;
      $mvLeftAction = 0;
   }
   else
   {
      $mvRightAction = 0;
      $mvLeftAction = -%xAxis * $movementSpeed;
   }

   if(%yAxis > 0)
   {
      $mvForwardAction = %yAxis * $movementSpeed;
      $mvBackwardAction = 0;
   }
   else
   {
      $mvForwardAction = 0;
      $mvBackwardAction = -%yAxis * $movementSpeed;
   }
}

function si_player_camera(%controller, %xAxis, %yAxis)
{
   //echo("si_player_camera( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");

   // Mouse-like input on player camera
   // based on a default camera FOV of 90'
   // ($cameraFov / 90) * 0.01 == ($cameraFov / 9000) ~= ($cameraFov * 0.000111)
   %xAxis *= ($cameraFov * 0.000111);
   %yAxis *= ($cameraFov * 0.000111);

   if(ServerConnection.isControlObjectRotDampedCamera())
   {  // Clamp and scale
      %xAxis = mClamp(%xAxis, -m2Pi()+0.01, m2Pi()-0.01);
      %xAxis *= 0.5;
      %yAxis = mClamp(%yAxis, -m2Pi()+0.01, m2Pi()-0.01);
      %yAxis *= 0.5;
   }
   $mvYaw += %xAxis;
   $mvPitch += %yAxis;
}

function si_player_jump(%controller, %state)
{
   //echo("si_player_jump( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount2++;
}

function si_player_crouch(%controller, %state)
{
   //echo("si_player_crouch( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount3++;
}

function si_player_sprint(%controller, %state)
{
   //echo("si_player_sprint( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount5++;
}

function si_player_weapup(%controller, %state)
{
   //echo("si_player_weapup( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('cycleWeapon', "next");
}

function si_player_weapdown(%controller, %state)
{
   //echo("si_player_weapdown( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('cycleWeapon', "prev");
}

function si_player_weapreload(%controller, %state)
{
   //echo("si_player_weapreload( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('reloadWeapon');
}

function si_player_tossweapon(%controller, %state)
{
   //echo("si_player_tossweapon( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('Throw', "Weapon");
}

function si_player_tossammo(%controller, %state)
{
   //echo("si_player_tossammo( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('Throw', "Ammo");
}

function si_player_carjack(%controller, %state)
{
   //echo("si_player_carjack( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      carjack();
}

function si_player_zoom(%controller, %state)
{
   //echo("si_player_zoom( " @ %controller @ ", " @ %state @ ")");
   toggleZoom(%state);
}

function si_player_interact(%controller, %state)
{
   echo("si_player_interact( " @ %controller @ ", " @ %state @ ")");
}

function si_player_menu(%controller, %state)
{
   //echo("si_player_menu( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      handleEscape();
}

function si_any_fire(%controller, %state)
{
   //echo("si_any_fire( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount0++;
}

function si_any_altfire(%controller, %state)
{
   //echo("si_any_altfire( " @ %controller @ ", " @ %state @ ")");
   $mvTriggerCount1++;
}

function si_any_camtoggle(%controller, %state)
{
   //echo("si_any_camtoggle( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      ServerConnection.setFirstPerson(!ServerConnection.isFirstPerson());
}

function si_any_freelook(%controller, %state)
{
   //echo("si_any_freelook( " @ %controller @ ", " @ %state @ ")");
   $mvFreeLook = %state;
}
