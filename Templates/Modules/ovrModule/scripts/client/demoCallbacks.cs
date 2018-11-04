//-----------------------------------------------------------------------------
// Callback functions for Action Set DemoControls
//-----------------------------------------------------------------------------

function in_demo_toggle_left(%controller, %state)
{
   //echo("in_demo_toggle_left( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      return;

   if ($OVRLeftToggle $= "")
      $OVRLeftToggle = 0;
   $OVRLeftToggle++;
   ovrModule.setSkeletonMode("Demo", "lh_anim", (($OVRLeftToggle % 3) < 2));

   if (isObject(OVR_LHTracker))
   {
      OVR_LHTracker.renderOVRModel = (($OVRLeftToggle % 3) < 1);
      //OVR_LHTracker.renderHand = !(($OVRLeftToggle % 4) == 1);
      OVR_LHTracker.rangeWithController = (($OVRLeftToggle % 3) < 2);
      OVR_LHTracker.activeLaser = OVR_LHTracker.activeLaser && (($OVRLeftToggle % 3) < 1);
   }
}

function in_demo_toggle_right(%controller, %state)
{
   //echo("in_demo_toggle_right( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      return;

   if ($OVRRightToggle $= "")
      $OVRRightToggle = 0;
   $OVRRightToggle++;
   ovrModule.setSkeletonMode("Demo", "rh_anim", (($OVRRightToggle % 3) < 2));

   if (isObject(OVR_RHTracker))
   {
      OVR_RHTracker.renderOVRModel = (($OVRRightToggle % 3) < 1);
      //OVR_RHTracker.renderHand = !(($OVRRightToggle % 4) == 1);
      OVR_RHTracker.rangeWithController = (($OVRRightToggle % 3) < 2);
      OVR_RHTracker.activeLaser = OVR_RHTracker.activeLaser && (($OVRRightToggle % 3) < 1);
   }
}

function in_demo_laser_left(%controller, %state)
{
   //echo("in_demo_laser_left( " @ %controller @ ", " @ %state @ ")");
   if (%state && isObject(OVR_LHTracker))
      OVR_LHTracker.activeLaser = !OVR_LHTracker.activeLaser && (($OVRLeftToggle % 3) < 1);
}

function in_demo_laser_right(%controller, %state)
{
   //echo("in_demo_laser_right( " @ %controller @ ", " @ %state @ ")");
   if (%state && isObject(OVR_RHTracker))
      OVR_RHTracker.activeLaser = !OVR_RHTracker.activeLaser && (($OVRRightToggle % 3) < 1);
}

function in_demo_menu(%controller, %state)
{
   //echo("in_demo_menu( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      OVROverlayCanvas.showCanvas(true);
}

// Callback templates for pose and velocity callbacks
/*
function in_demo_left_pose(%controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot)
{
   //echo("in_demo_left_pose( " @ %controller @ ", " @ %xPos @ ", " @ %yPos @ ", " @ %zPos @ ", " @ %xRot @ ", " @ %yRot @ ", " @ %zRot @ ", " @ %wRot @ ")");
}

function in_demo_right_pose(%controller, %xPos, %yPos, %zPos, %xRot, %yRot, %zRot, %wRot)
{
   //echo("in_demo_right_pose( " @ %controller @ ", " @ %xPos @ ", " @ %yPos @ ", " @ %zPos @ ", " @ %xRot @ ", " @ %yRot @ ", " @ %zRot @ ", " @ %wRot @ ")");
}

function in_demo_left_pose_vel(%controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel)
{
   //echo("in_demo_left_pose_vel( " @ %controller @ ", " @ %xLinVel @ ", " @ %yLinVel @ ", " @ %zLinVel @ ", " @ %xAngVel @ ", " @ %yAngVel @ ", " @ %zAngVel @ ")");
}

function in_demo_right_pose_vel(%controller, %xLinVel, %yLinVel, %zLinVel, %xAngVel, %yAngVel, %zAngVel)
{
   //echo("in_demo_right_pose_vel( " @ %controller @ ", " @ %xLinVel @ ", " @ %yLinVel @ ", " @ %zLinVel @ ", " @ %xAngVel @ ", " @ %yAngVel @ ", " @ %zAngVel @ ")");
}
*/