//-----------------------------------------------------------------------------
// Overlay canvas functions.
//-----------------------------------------------------------------------------

$OVROverlayStep = 0.05;
$OVROverlayStepMin = -3;
$OVROverlayStepMax = -0.6;

// This differs from Canvas::checkCursor because we don't hide/show the hardware
// cursor or lock the mouse. This turns on/off the software cursor (DefaultCursor)
// on the offscreen canvas.
function GuiOffscreenCanvas::checkCursor(%this)
{
   %count = %this.getCount();
   for(%i = 0; %i < %count; %i++)
   {
      %control = %this.getObject(%i);
      if ((%control.noCursor $= "") || !%control.noCursor)
      {
         %this.cursorOn();
         return true;
      }
   }
   // If we get here, every control requested a hidden cursor, so we oblige.
   %this.cursorOff();
   return false;
}

function OVROverlayCanvas::pushDialog(%this, %ctrl, %layer, %center)
{
   //echo("OVERLAY PUSHED DIALOG " @ %ctrl);
   Parent::pushDialog(%this, %ctrl, %layer, %center);
   %this.showCanvas(true);
}

function OVROverlayCanvas::popDialog(%this, %ctrl)
{
   //echo("OVERLAY POPPED DIALOG " @ %ctrl);
   Parent::popDialog(%this, %ctrl);
   %this.showCanvas(false);
}

function OVROverlayCanvas::showCanvas(%this, %show)
{
   if (%show)
   {
      OVROverlayCanvas.showOverlay();
      VRCanvasMap.push();
      if ($InputMap::CurrentActionSet !$= "Menu")
         ovrModule.pushActionSetLayer("Menu");
   }
   else
   {
      OVROverlayCanvas.hideOverlay();
      VRCanvasMap.pop();
      if ($InputMap::CurrentActionSet !$= "Menu")
         ovrModule.popActionSetLayer("Menu");
   }
}

function toggleOVROverlay(%val)
{
   if (%val)
   {
      %wasActive = OVROverlayCanvas.isActiveCanvas();
      OVROverlayCanvas.showCanvas(!%wasActive);
   }
}

//-----------------------------------------------------------------------------
function VRSetupOverlay()
{
   if (!isObject(VROverlay))
   {
      exec("data/ovrModule/scripts/gui/ovrOverlay.gui");
   }

   if (!isObject(OVROverlayCanvas))
   {
      new OpenVROverlay(OVROverlayCanvas) {
         internalName = "torque.vrcanvas";
         targetSize = getWord($pref::OVR::OverlayRes, 0) SPC getWord($pref::OVR::OverlayRes, 2);
         dynamicTarget = true;

         overlayType = $pref::OVR::Dashboard ? "Dashboard" : "Overlay";
         transformType = $pref::OVR::XfmStat ? "Absolute" : "TrackedDeviceRelative";
         transformPosition = "0 -1 " @ ($pref::OVR::Seated ? 0 : -OpenVR::getHMDTrackingHeight());
         transformRotation = "0 0 1 0";

         highQualityOverlay = $pref::OVR::HQOverlay;
         overlayFlags = $pref::OVR::CurvedOverlay ? $OpenVR::OverlayFlags_Curved : $OpenVR::OverlayFlags_None;
         overlayWidth = $pref::OVR::Overlaywidth;
         //inputMethod = $pref::OVR::InputType ? "1" : "0";
         //manualMouseHandling = $pref::OVR::OverlayEvents;
      };
   }

   if ($pref::OVR::HQOverlay && $pref::OVR::CurvedOverlay)
      OVROverlayCanvas.setCurveRange($pref::OVR::CurveMin, $pref::OVR::CurveMax);

   OVROverlayCanvas.setContent(VROverlay);
   OVROverlayCanvas.setCursor(DefaultCursor);
   OVROverlayCanvas.setCursorPos(getWord($pref::OVR::OverlayRes, 0)/2 SPC getWord($pref::OVR::OverlayRes, 2)/2);
   OVROverlayCanvas.hideOverlay();
   $GameCanvas = OVROverlayCanvas;
   
   %buttonText = $pref::OVR::Seated ? "Reset Seated Pose" : "Calibrate Height";
   VROverlay->ResetButton.setText(%buttonText);
}

function OVRMoveOverlay(%val)
{
   %yPos = OVROverlayCanvas.transformPosition.y;
   %yPos += (%val > 0) ? $OVROverlayStep : -$OVROverlayStep;

   if (%yPos < $OVROverlayStepMin)
      %yPos = $OVROverlayStepMin;
   if (%yPos > $OVROverlayStepMax)
      %yPos = $OVROverlayStepMax;

   OVROverlayCanvas.transformPosition.y = %yPos;
}

function rebuildOVRCanvas()
{
   if (isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.showCanvas(false);
      OVROverlayCanvas.delete();
      VRSetupOverlay();
   }
}
