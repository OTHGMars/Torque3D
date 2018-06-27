//-----------------------------------------------------------------------------
// Callback functions for Action Set VehicleControls
//-----------------------------------------------------------------------------

function si_vehicle_steer(%controller, %xAxis, %yAxis)
{
   //echo("si_vehicle_steer( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");
   // based on a default camera FOV of 90'
   // ($cameraFov / 90) * 0.01 * 10 == ($cameraFov / 900) ~= ($cameraFov * 0.001111)
   %xAxis *= ($cameraFov * 0.001111);
   %yAxis *= ($cameraFov * 0.001111);

   if(%xAxis > 0)
   {
      $mvYawLeftSpeed = %xAxis;
      $mvYawRightSpeed = 0;
   }
   else
   {
      $mvYawLeftSpeed = 0;
      $mvYawRightSpeed = -%xAxis;
   }

   if(%yAxis > 0)
   {
      $mvPitchDownSpeed = %yAxis;
      $mvPitchUpSpeed = 0;
   }
   else
   {
      $mvPitchDownSpeed = 0;
      $mvPitchUpSpeed = -%yAxis;
   }
}

function si_vehicle_throttle(%controller, %xAxis, %yAxis)
{
   //echo("si_vehicle_throttle( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");
   $mvForwardAction = %xAxis * $movementSpeed;
}

function si_vehicle_reverse(%controller, %xAxis, %yAxis)
{
   //echo("si_vehicle_reverse( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");
   $mvBackwardAction = %xAxis * $movementSpeed;
}

function si_vehicle_brake(%controller, %state)
{
   //echo("si_vehicle_brake( " @ %controller @ ", " @ %state @ ")");
   commandToServer('toggleBrakeLights');
   $mvTriggerCount2++;
}

function si_vehicle_brakelight(%controller, %state)
{
   //echo("si_vehicle_brakelight( " @ %controller @ ", " @ %state @ ")");
   commandToServer('toggleBrakeLights');
}

function si_vehicle_getout(%controller, %state)
{
   //echo("si_vehicle_getout( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      getOut();
}

function si_vehicle_flipcar(%controller, %state)
{
   //echo("si_vehicle_flipcar( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('flipCar');
}

function si_vehicle_menu(%controller, %state)
{
   //echo("si_vehicle_menu( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      handleEscape();
}
