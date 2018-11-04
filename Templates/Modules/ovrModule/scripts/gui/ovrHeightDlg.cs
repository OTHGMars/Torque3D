//

function OpenVRHeightDlg::onWake(%this)
{
   %message = "<just:center>";
   if ($pref::OVR::HMDHeight $= "")
      %message = %message @ "Since this is your first time in Standing tracking space, pleae calibrate your hmd height. ";
   %message = %message @ "To reset your HMD height, stand up straight and press \'Set\'.";
   %this-->MessageText.setText(%message);

   %this.lastHeight = OpenVR::getHMDTrackingHeight();
   %this.showHeight();
}

function OpenVRHeightDlg::onSleep(%this)
{
   if (isEventPending(%this.updateSchedule))
      cancel(%this.updateSchedule);
}

function OpenVRHeightDlg::setHMDHeight(%this)
{
   OpenVR::setHMDTrackingHeight(%this.currentHeight);
   $pref::OVR::HMDHeight = %this.currentHeight;
   $GameCanvas.popDialog(%this);
   if (isObject(OVROverlayCanvas))
      OVROverlayCanvas.transformPosition.z = $pref::OVR::Seated ? 0.0 : -%this.currentHeight;
}

function OpenVRHeightDlg::showHeight(%this)
{
   %currentHeight = %this.lastHeight + $mvPosZ0;
   %currentHeight = mRoundColour(%currentHeight, 2);
   %this.currentHeight = %currentHeight;
   %this-->CurrentValue.setText(%currentHeight);

   %this.updateSchedule = %this.schedule(100, "ShowHeight");
}

