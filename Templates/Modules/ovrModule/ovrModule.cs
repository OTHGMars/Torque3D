//-----------------------------------------------------------------------------
// Module creation functions.
//-----------------------------------------------------------------------------

$Video::VREnabled = false;
function ovrModule::create( %this )
{
   // The action set file that defines all of your game actions and types
   $OpenVR::inputManifestPath = "data/ovrModule/manifest/t3d_actions.json";

   // Cached textures for VR render models will get saved here for faster
   // loading. It needs write permission to this path.
   $OpenVR::cachePath = "data/ovrModule/cache/";

   // Setup Guis
   exec("./scripts/gui/ovrSettingsDlg.cs");
   exec("./scripts/gui/ovrSettings.gui");
   exec("./scripts/gui/ovrHeightDlg.cs");
   exec("./scripts/gui/ovrHeight.gui");

   // Game/Demo specific controller actions and callbacks
   exec("./scripts/client/actionSets.cs");
   exec("./scripts/client/demoCallbacks.cs");
   exec("./scripts/client/menuCallbacks.cs");
   exec("./scripts/client/playerCallbacks.cs");
   exec("./scripts/client/vehicleCallbacks.cs");

   // Core system scripts
   exec("./scripts/client/openvr.cs");
   exec("./scripts/client/devices.cs");
   exec("./scripts/client/overlay.cs");
   exec("./scripts/client/inputSetup.cs");
}

function ovrModule::destroy( %this )
{
   
}

// System event callbacks
function onOVRInputReady()
{
   ovrModule.initActionSets();
}

function onOVRDeviceActivated(%deviceIdx)
{
   echo("OVR device " @ %deviceIdx @ " is now active");
   identifyOVRDevice(%deviceIdx);
   if (OpenVR::getDeviceClass(%deviceIdx) $= "Controller")
      setupOVRControllers();
}

function onOVRDeviceRoleChanged()
{
   echo("OVR devices have changed roles.");
   setupOVRControllers();
}
