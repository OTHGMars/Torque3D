//
//if( !isObject( GuiVRRadioProfile ) )
//new GuiControlProfile( GuiVRRadioProfile )
//{
   //fontSize = 14;
   //fillColor = "232 232 232";
   //fontColor = "20 20 20";
   //fontColorHL = "80 80 80";
   //fixedExtent = true;
   //bitmap = "core/art/gui/images/radioButton";
   //hasBitmapArray = true;
   //tab = true;
   //canKeyFocus = true;   
   //category = "OVR";
//};
if( !isObject( GuiVRTextEditProfile ) )
new GuiControlProfile( GuiVRTextEditProfile )
{
   opaque = true;
   bitmap = "core/art/gui/images/textEdit";
   hasBitmapArray = true; 
   border = -2; // fix to display textEdit img
   fillColor = "242 241 240 0";
   fillColorHL = "255 255 255";
   fontColor = "0 0 0";
   fontColorHL = "255 255 255";
   fontColorSEL = "98 100 137";
   fontColorNA = "200 200 200";
   textOffset = "4 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   justify = "left";
   //tab = true;
   canKeyFocus = true;   
   category = "OVR";
};


function OpenVRSetupDlg::onWake(%this)
{
   // Save starting values that require a separate apply
   %this.startRes = $pref::OVR::OverlayRes;
   %this.curveMin = $pref::OVR::CurveMin;
   %this.curveMax = $pref::OVR::CurveMax;

   // Setup available resolutions. Add to this list to test a res in the overlay
   %this.overlaySizes = "1920 x 1080\t1600 x 900\t1400 x 1050\t1280 x 720\t1024 x 1024\t1024 x 768\t800 x 600\t512 x 512";
   %this.fillResList();
   %this.setHeightText();
   %this.setDashboardOptions();

   %this-->CurveMin.setText($pref::OVR::CurveMin);
   %this-->CurveMax.setText($pref::OVR::CurveMax);
   %this-->WidthText.setText($pref::OVR::Overlaywidth);
   %this-->ApplyBtn.setVisible($Video::VREnabled && isObject(OVROverlayCanvas));

   %this.setDashboardOptions();
}

function OpenVRSetupDlg::fillResList(%this)
{
   // Add all preset sizes
   %this-->ResList.clear();
   %count = 0;
   %resCount = getFieldCount(%this.overlaySizes);
   for (%i = 0; %i < %resCount; %i++)
   {
      %resString = getField(%this.overlaySizes, %i);
      %this-->ResList.add(%resString, %i);
   }
   %this-->ResList.setText($pref::OVR::OverlayRes);
}

function OpenVRSetupDlg::setHeightText(%this)
{  // Calculate and display the overlay height
   %aspectRatio = getWord($pref::OVR::OverlayRes, 2) / getWord($pref::OVR::OverlayRes, 0);
   %height = $pref::OVR::Overlaywidth * %aspectRatio;
   %height = mRound( %height * 100 ) * 0.01;
   %this-->WidthResult.text = "= Height " @ %height @ " meter(s)";
}

function OpenVRSetupDlg::setDashboardOptions(%this)
{
   // If a dashboard type overlay is selected disable fields that it excludes
   if ($pref::OVR::Dashboard)
      $pref::OVR::HQOverlay = false;
   %this-->HQCheck.setActive(!$pref::OVR::Dashboard);
   %this.setHQOptions();
}

function OpenVRSetupDlg::setHQOptions(%this)
{
   // If it's not the "high quality" overlay, none of the curve options can be used
   if (!$pref::OVR::HQOverlay)
      $pref::OVR::CurvedOverlay = false;
   %this-->CurveCheck.setActive($pref::OVR::HQOverlay);
   %this-->CurveMin.setActive($pref::OVR::HQOverlay);
   %this-->CurveMax.setActive($pref::OVR::HQOverlay);
   %this.checkApply();

   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.highQualityOverlay = $pref::OVR::HQOverlay;
   }
}

function OpenVRSetupDlg::checkApply(%this)
{
   if (%this-->ApplyBtn.isVisible())
   {
      %isActive = ($pref::OVR::OverlayRes !$= %this.startRes) ||
         ($pref::OVR::CurveMin !$= %this.curveMin) || ($pref::OVR::CurveMax !$= %this.curveMax);
      %this-->ApplyBtn.setActive(%isActive);
   }
}

function OpenVRSetupDlg::onRenderStyle(%this)
{
   %newStyle = $pref::Video::StereoSEP ? "stereo separate" : "stereo side by side";
   if (PlayGui.isAwake() && PlayGui.renderStyle !$= "standard")
      PlayGui.renderStyle = %newStyle;
}

function OpenVRSetupDlg::onResolutionChange(%this)
{
   $pref::OVR::OverlayRes = %this-->ResList.getText();
   %this.setHeightText();
   %this.checkApply();
}

function OpenVRSetupDlg::onUniverseChange(%this)
{
   $OpenVR::HMDRotateYawWithMoveActions = $pref::OVR::Seated;
   if ($Video::VREnabled)
   {
      OpenVR::setRoomTracking($pref::OVR::Standing);
      if (isObject(OVROverlayCanvas))
      {
         OVROverlayCanvas.trackingOrigin = $pref::OVR::Seated ? "Seated" : "Standing";
         OVROverlayCanvas.transformPosition.z = $pref::OVR::Seated ? 0.0 : -OpenVR::getHMDTrackingHeight();
      }
   }
}

function OpenVRSetupDlg::onOverlayChange(%this)
{
   %this.setDashboardOptions();
   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.overlayType = $pref::OVR::Dashboard ? "Dashboard" : "Overlay";
   }
}

function OpenVRSetupDlg::setInputType(%this)
{
   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.inputMethod = $pref::OVR::InputType ? "1" : "0";
   }
}

function OpenVRSetupDlg::setOverlayEvents(%this)
{
   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.manualMouseHandling = $pref::OVR::OverlayEvents;
   }
}

function OpenVRSetupDlg::onXformChange(%this)
{
   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.transformType = $pref::OVR::XfmStat ? "Absolute" : "TrackedDeviceRelative";
   }
}

function OpenVRSetupDlg::onWidthChange(%this)
{
   %width = %this-->WidthText.getText();
   if ( %width > 0 )
   {
      $pref::OVR::Overlaywidth = %width;
      %this.setHeightText();
   }
}

function OpenVRSetupDlg::onCurveOverlay(%this)
{
   if ($Video::VREnabled && isObject(OVROverlayCanvas))
   {
      OVROverlayCanvas.overlayFlags = $pref::OVR::CurvedOverlay ? $OpenVR::OverlayFlags_Curved : $OpenVR::OverlayFlags_None;
   }
}

function OpenVRSetupDlg::onCurveDistance(%this)
{
   $pref::OVR::CurveMin = %this-->CurveMin.getText();
   $pref::OVR::CurveMax = %this-->CurveMax.getText();
   %this.checkApply();
}

function OpenVRSetupDlg::onApply(%this)
{
   // If the render target resolution is changing, we need to do a full teardown
   // of the offscreen canvas, but we can't do it from this callback.
   if ($pref::OVR::OverlayRes !$= %this.startRes)
   {
      $GameCanvas.popDialog(%this);
      schedule(32, 0, "rebuildOVRCanvas");
   }
   else
   {  // Just set the updated curve parameters.
      if (($pref::OVR::CurveMin !$= %this.curveMin) || ($pref::OVR::CurveMax !$= %this.curveMax))
      {
         %this.curveMin = $pref::OVR::CurveMin;
         %this.curveMax = $pref::OVR::CurveMax;
         OVROverlayCanvas.setCurveRange($pref::OVR::CurveMin, $pref::OVR::CurveMax);
      }
      %this.checkApply();
   }
}
