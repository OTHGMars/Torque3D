//-----------------------------------------------------------------------------
// Controller functions.
//-----------------------------------------------------------------------------

function setupDefaultOpenVRBinds(%moveMap)
{
   if (!$OVRDevicesListed)
   {  // List the connected devices in the console
      for (%i = 0; %i < 16; %i++)
      {
         identifyOVRDevice(%i);
      }
   }
   %moveMap.bindCmd( keyboard, o, "resetOpenVRSensors();", "");
   %moveMap.bindCmd( gamepad0, "btn_y", "resetOpenVRSensors();", "");
   %moveMap.bind(keyboard, "h", toggleOVROverlay);

   if ( isObject(VRCanvasMap) )
      return;

   new ActionMap(VRCanvasMap);
   VRCanvasMap.bind(mouse0, "zaxis", OVRMoveOverlay);
}

function identifyOVRDevice(%deviceIdx)
{
   %deviceClass = OpenVR::getDeviceClass(%deviceIdx);
   if (%deviceClass !$= "Invalid")
   {
      echo("\nOpenVR device: " @ %deviceIdx SPC %deviceClass);

      // From ETrackedDeviceProperty
      //Prop_TrackingSystemName_String = 1000
      //Prop_ModelNumber_String        = 1001
      //Prop_ManufacturerName_String   = 1005
      echo(" Manufacturer: " @ OpenVR::getDevicePropertyString(%deviceIdx, 1005));
      echo("       System: " @ OpenVR::getDevicePropertyString(%deviceIdx, 1000));
      echo("        Model: " @ OpenVR::getDevicePropertyString(%deviceIdx, 1001));
   }

   if (%deviceClass $= "Controller")
   {
      // Properties that are unique to TrackedDeviceClass_Controller
      //Prop_AttachedDeviceId_String = 3000,
      //Prop_SupportedButtons_Uint64 = 3001,
      //Prop_Axis0Type_Int32         = 3002, // Return value is of type EVRControllerAxisType
      //Prop_Axis1Type_Int32         = 3003, // Return value is of type EVRControllerAxisType
      //Prop_Axis2Type_Int32         = 3004, // Return value is of type EVRControllerAxisType
      //Prop_Axis3Type_Int32         = 3005, // Return value is of type EVRControllerAxisType
      //Prop_Axis4Type_Int32         = 3006, // Return value is of type EVRControllerAxisType
      //Prop_ControllerRoleHint_Int32= 3007, // Return value is of type ETrackedControllerRole
      echo("    Device ID: " @ OpenVR::getDevicePropertyString(%deviceIdx, 3000));
      echo("      Buttons: " @ OpenVR::getDevicePropertyUInt(%deviceIdx, 3001));
      for (%i = 0; %i < 5; %i++)
         echo("  Axis "@ %i @" Type: " @ OpenVR::getControllerAxisType(%deviceIdx, %i));
      echo("         Role: " @ OpenVR::getControllerRole(%deviceIdx));
   }

   $OVRDevicesListed = true;
}

function setupOVRControllers()
{
   for (%i = 0; %i < 16; %i++)
   {
      if (OpenVR::getDeviceClass(%i) $= "Controller")
      {
         if (OpenVR::getControllerRole(%i) $= "LeftHand")
            %leftIdx = %i;
         else
            %rightIdx = %i;
      }
      if ((%leftIdx !$= "") && (%rightIdx !$= ""))
         break;
   }

   echo("OVR Controllers - Left: " @ %leftIdx @ ", Right: " @ %rightIdx);
   ovrModule.assignHands(%leftIdx, %rightIdx);
}

// Right hand client side tracked object for demo
function CreateVR_RHTracker(%deviceIndex, %controllerModel)
{
   if (!isObject(OVR_RHTracker))
   {
      echo("Creating right hand tracker with render model: " @ OpenVR::getDevicePropertyString(%deviceIndex, 1003));

      if (!isObject(LaserPointerMat))
         exec("data/ovrModule/art/vrModels/ovrMats.cs");

      if (getWord(%controllerModel, 0) $= "oculus")
         %offset = "0 -.04 -.02 1 0 0 0.66";
      else if (getWord(%controllerModel, 0) $= "vive")
         %offset = "0 0 -0.003 1 0 0 0";
      else if (getWord(%controllerModel, 0) $= "Knuckles")
         %offset = "-0.005 -0.03 -0.02 1 0 0 0.33";
      else
         %offset = "0 0 0 1 0 0 0";

      new OpenVRClientTObj(OVR_RHTracker)
      {
         inputSource = "/user/hand/right";
         poseAction = "/actions/demo/in/hand_right";
         skeletonAction = "/actions/demo/in/rh_anim";
         shapeName = "data/ovrModule/art/vrModels/vr_glove_right.dae";
         rangeWithController = true;
         renderHand = false;
         renderOVRModel = true;
         
         startRadius = "0.003";
         laserSides = "16";
         maxPointerLength = 5;
         laserMaterial = "LaserPointerMat";
         laserHoverMaterial = "LaserHoverMat";
         laserOffset = %offset;
      };
   }
   else
   {
      OVR_RHTracker.resetDeviceIndex();
   }
}

// Left hand client side tracked object for demo
function CreateVR_LHTracker(%deviceIndex, %controllerModel)
{
   if (!isObject(OVR_LHTracker))
   {
      echo("Creating left hand tracker with render model: " @ OpenVR::getDevicePropertyString(%deviceIndex, 1003));

      if (!isObject(LaserPointerMat))
         exec("data/ovrModule/art/vrModels/ovrMats.cs");

      if (getWord(%controllerModel, 0) $= "oculus")
         %offset = "0 -.04 -.02 1 0 0 0.66";
      else if (getWord(%controllerModel, 0) $= "vive")
         %offset = "0 0 -0.003 1 0 0 0";
      else if (getWord(%controllerModel, 0) $= "Knuckles")
         %offset = "0.005 -0.03 -0.02 1 0 0 0.33";
      else
         %offset = "0 0 0 1 0 0 0";


      new OpenVRClientTObj(OVR_LHTracker)
      {
         inputSource = "/user/hand/left";
         poseAction = "/actions/demo/in/hand_left";
         skeletonAction = "/actions/demo/in/lh_anim";
         shapeName = "data/ovrModule/art/vrModels/vr_glove_left.dae";
         rangeWithController = true;
         renderHand = false;
         renderOVRModel = true;
         
         startRadius = "0.003";
         laserSides = "16";
         maxPointerLength = 5;
         laserMaterial = "LaserPointerMat";
         laserHoverMaterial = "LaserHoverMat";
         laserOffset = %offset;
      };
   }
   else
   {
      OVR_LHTracker.resetDeviceIndex();
   }
}
