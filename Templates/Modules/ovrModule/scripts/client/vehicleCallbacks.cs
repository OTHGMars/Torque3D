//-----------------------------------------------------------------------------
// Callback functions for Action Set VehicleControls
//-----------------------------------------------------------------------------

function in_vehicle_steer(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_vehicle_steer( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");

   // Analog steering. Map the [-1, 1] input to the steering range for the
   // Cheetah [-0.585, 0.585]
   $mvYaw = %xAxis * 0.585;
   $mvPitch = %yAxis * mPi();

   // Don't let mouse drift interfere
   $mvYawLeftSpeed = 0;
   $mvYawRightSpeed = 0;
   $mvPitchUpSpeed = 0;
   $mvPitchDownSpeed = 0;
}

function in_vehicle_throttle(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_vehicle_throttle( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");
   $mvForwardAction = %xAxis * $movementSpeed;
}

function in_vehicle_reverse(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_vehicle_reverse( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");
   $mvBackwardAction = %xAxis * $movementSpeed;
}

function in_vehicle_brake(%controller, %state)
{
   //echo("in_vehicle_brake( " @ %controller @ ", " @ %state @ ")");
   commandToServer('toggleBrakeLights');
   $mvTriggerCount2++;
}

function in_vehicle_brakelight(%controller, %state)
{
   //echo("in_vehicle_brakelight( " @ %controller @ ", " @ %state @ ")");
   commandToServer('toggleBrakeLights');
}

function in_vehicle_getout(%controller, %state)
{
   //echo("in_vehicle_getout( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      getOut();
}

function in_vehicle_flipcar(%controller, %state)
{
   //echo("in_vehicle_flipcar( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      commandToServer('flipCar');
}

function in_vehicle_menu(%controller, %state)
{
   //echo("in_vehicle_menu( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      OVROverlayCanvas.showCanvas(true);
}
