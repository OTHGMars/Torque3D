
function HapticTestDlg::onWake(%this)
{
   if (%this.selectedDeviceId $= "")
      %this.selectedDeviceId = 0;
   if (%this.rumbleStrength $= "")
      %this.rumbleStrength = 0.5;
   if (%this.rumbleLength $= "")
      %this.rumbleLength = 2000;

   %this.updateStatus();
}

function HapticTestDlg::onDeviceButton(%this)
{
   %isOpen = SDLHaptic::isOpen(%this.selectedDeviceId);
   if (%isOpen)
   {
      SDLHaptic::close(%this.selectedDeviceId);
   }
   else
   {
      %obj = SDLHaptic::open(%this.selectedDeviceId);
   }
   %this.schedule(100, "initFromDevice", %this.selectedDeviceId);
}

function HapticTestDlg::updateStatus(%this)
{
   %hapticDevices = SDLHaptic::numHaptics();
   %hapticMouse = SDLHaptic::mouseIsHaptic();

   %text = %hapticDevices @ " Haptic device(s) found.";
   if (%hapticMouse)
      %text = %text @ " Mouse is haptic.";
   %this-->devicesLine.setText(%text);

//%hapticDevices = 0;

   HapticDeviceList.clear();
   if (%hapticDevices > 0)
   {
      for (%i = 0; %i < %hapticDevices; %i++)
      {
         %name = SDLHaptic::getName(%i);
         HapticDeviceList.add(%name, %i);
      }
      if ((%this.selectedDeviceId < 0) || (%this.selectedDeviceId >= %hapticDevices))
         %this.selectedDeviceId = 0;
      HapticDeviceList.setSelected(%this.selectedDeviceId, true);
   }
   else
   {
      HapticDeviceList.setActive(false);
      %this-->DeviceButton.setActive(false);

      %this-->NoDevLine.setText("No Devices Found...Nothing to do here.");
      %this-->NoDevLine.setVisible(true);
      %this-->OptionsBox.setVisible(false);
      %this.selectedDevice = -1;
   }
}

function HapticTestDlg::initFromDevice(%this, %deviceId)
{
   %this.selectedDeviceId = %deviceId;
   %this-->DeviceButton.setActive(true);

   %isOpen = SDLHaptic::isOpen(%deviceId);
   if (%isOpen)
      %hapticObj = SDLHaptic::getDevice(%deviceId);

   if (!%isOpen || !isObject(%hapticObj))
   {
      %this-->DeviceButton.setText("Open Device");

      %this-->NoDevLine.setText("Open device to view settings.");
      %this-->NoDevLine.setVisible(true);
      HapticTestScroll.setVisible(false);
      return;
   }

   %this-->DeviceButton.setText("Close Device");
   %this-->NoDevLine.setVisible(false);
   %this.clearEffectBoxes();
   HapticTestScroll.setVisible(true);

   %hasRumble = %hapticObj.rumbleSupported();
   %deviceCaps = %hapticObj.query();
   %numEffects = %hapticObj.numEffects();
   %numSimultaneous = %hapticObj.numEffectsPlaying();
   %effectCount = %hapticObj.getEffectCount();
   
   %text = "Device can store " @ %numEffects @ " effect(s) and play ";
   if (%numSimultaneous < 2)
      %text = %text @ "one at a time.";
   else
      %text = %text @ "up to " @ %numSimultaneous @ " at a time.";
   HapticFeatures->DeviceCaps.setText(%text);

   %text = "";
   if (%hasRumble)
      %text = %text @ "Rumble";
   if (%deviceCaps & $SDLTypeMasks::HapticConstant)
      %text = %text @ (%text !$= "" ? ", Constant Effect" : "Constant Effect");
   if (%deviceCaps & $SDLTypeMasks::HapticLeftRight)
      %text = %text @ (%text !$= "" ? ", Left/Right Effect" : "Left/Right Effect");
   if ((%deviceCaps & $SDLTypeMasks::HapticSine) || (%deviceCaps & $SDLTypeMasks::HapticTriangle) ||
      (%deviceCaps & $SDLTypeMasks::HapticSawToothUp) || (%deviceCaps & $SDLTypeMasks::HapticSawToothDown))
      %text = %text @ (%text !$= "" ? ", Periodic Effect" : "Periodic Effect");
   if ((%deviceCaps & $SDLTypeMasks::HapticSpring) || (%deviceCaps & $SDLTypeMasks::HapticDamper) ||
      (%deviceCaps & $SDLTypeMasks::HapticInertia) || (%deviceCaps & $SDLTypeMasks::HapticFriction))
      %text = %text @ (%text !$= "" ? ", Condition Effect" : "Condition Effect");
   if (%deviceCaps & $SDLTypeMasks::HapticRamp)
      %text = %text @ (%text !$= "" ? ", Ramp Effect" : "Ramp Effect");
   if (%deviceCaps & $SDLTypeMasks::HapticCustom)
      %text = %text @ (%text !$= "" ? ", Custom Effects" : "Custom Effects");
   if (%deviceCaps & $SDLTypeMasks::HapticGain)
      %text = %text @ (%text !$= "" ? ", Gain Adjust" : "Gain Adjust");
   if (%deviceCaps & $SDLTypeMasks::HapticAutocenter)
      %text = %text @ (%text !$= "" ? ", Autocenter" : "Autocenter");
   if (%deviceCaps & $SDLTypeMasks::HapticStatus)
      %text = %text @ (%text !$= "" ? ", Status Query" : "Status Query");
   if (%deviceCaps & $SDLTypeMasks::HapticPause)
      %text = %text @ (%text !$= "" ? ", Pause" : "Pause");

   HapticFeatures->DeviceFeatures.setText("Features Supported: " @ %text);

   HapticTestRumble.setVisible(%hasRumble);

   %featureStack = HapticTestScroll->FeatureStack;
   %featureStack->GainBox.setVisible(%deviceCaps & $SDLTypeMasks::HapticGain);
   %this.gainValue = %hapticObj.getGain();
   %featureStack->GainBox->valueSlider.setValue(%this.gainValue);

   %featureStack->AutocenterBox.setVisible(%deviceCaps & $SDLTypeMasks::HapticAutocenter);
   %this.autocenterValue = %hapticObj.getAutocenter();
   %featureStack->AutocenterBox->valueSlider.setValue(%this.autocenterValue);
   
   %this.initEffectList(%deviceCaps);

   for (%i = 0; %i < %effectCount; %i++)
   {
      %effectobj = %hapticObj.getEffect(%i);
      if (isObject(%effectobj))
         %this.addEffect(%effectObj, %hapticObj);
   }
}

function HapticTestDlg::initEffectList(%this, %deviceCaps)
{
   HapticEffectList.clear();
   %numEffects = 0;
   %firstEffect = "";
   for (%mask = $SDLTypeMasks::HapticConstant; %mask < $SDLTypeMasks::HapticCustom; %mask *= 2)
   {
      if (%deviceCaps & %mask)
      {
         %name = SDLHaptic::getEffectName(%mask);
         HapticEffectList.add(%name SPC "Effect", %mask);
         %numEffects++;
         if (%firstEffect $= "")
            %firstEffect = %mask;
      }
   }
   if (%numEffects > 0)
   {
      HapticEffectList.setSelected(%firstEffect, false);
      HapticTestScroll-->CreateEffectBox.setVisible(true);
   }
   else
      HapticTestScroll-->CreateEffectBox.setVisible(false);
}

function HapticTestDlg::playRumble(%this)
{
   %hapticObj = SDLHaptic::getDevice(%this.selectedDevice);
   if (isObject(%hapticObj))
      %hapticObj.rumblePlay(%this.rumbleStrength, %this.rumbleLength);
   HapticTestRumble->RumbleStrength.setActive(false);
   HapticTestRumble->RumbleLength.setActive(false);
   HapticTestRumble->PlayButton.setActive(false);
   HapticTestRumble->StopButton.setActive(true);
   %this.rumbleSched = %this.schedule(%this.rumbleLength, "rumbleTimeout");
}

function HapticTestDlg::stopRumble(%this)
{
   if (isEventPending(%this.rumbleSched))
      cancel(%this.rumbleSched);

   %hapticObj = SDLHaptic::getDevice(%this.selectedDevice);
   if (isObject(%hapticObj))
      %hapticObj.rumbleStop();
   %this.rumbleTimeout();
}

function HapticTestDlg::rumbleTimeout(%this)
{
   HapticTestRumble->RumbleStrength.setActive(true);
   HapticTestRumble->RumbleLength.setActive(true);
   HapticTestRumble->PlayButton.setActive(true);
   HapticTestRumble->StopButton.setActive(false);
}

function HapticTestDlg::applyGain(%this)
{
   //echo("Applying Gain: " @ %this.gainValue);
   %hapticObj = SDLHaptic::getDevice(%this.selectedDevice);
   if (isObject(%hapticObj))
      %hapticObj.setGain(%this.gainValue);
}

function HapticTestDlg::applyAutocenter(%this)
{
   //echo("Applying Autocenter: " @ %this.autocenterValue);
   %hapticObj = SDLHaptic::getDevice(%this.selectedDevice);
   if (isObject(%hapticObj))
      %hapticObj.setAutocenter(%this.autocenterValue);
}

function HapticTestDlg::clearEffectBoxes(%this)
{
   %effectBox = HapticTestScroll->FeatureStack.findObjectByInternalName("TempEffectBox");
   while (isObject(%effectBox))
   {
      %effectBox.delete();
      %effectBox = HapticTestScroll->FeatureStack.findObjectByInternalName("TempEffectBox");
   }
}

function HapticTestDlg::onNewEffectButton(%this)
{
   %effectType = HapticEffectList.getSelected();
   %hapticObj = SDLHaptic::getDevice(%this.selectedDevice);
   if (isObject(%hapticObj))
   {
      %effectObj = %hapticObj.createEffect(%effectType);
      if (isObject(%effectObj))
      {
         %this.addEffect(%effectObj, %hapticObj);
         echo("Effect Created: " @ %effectObj @ "! Type = " @ %effectType);
      }
   }
}

function HapticTestDlg::addEffect(%this, %effectObj, %hapticObj)
{
   %guiCtrl = HapticEffectPrototype.deepClone();
   %guiCtrl.canSave = false;
   %guiCtrl.visible = true;
   %guiCtrl.initFromEffect(%effectObj, %hapticObj);
   HapticTestScroll->FeatureStack.addGuiControl(%guiCtrl);
}

function HapticDeviceList::onSelect(%this, %idVal, %name)
{
   //echo("Selected #" @ %idVal @ ": " @ %name);
   HapticTestDlg.initFromDevice(%idVal);
}

function HapticFeatureList::onResize(%this, %width, %height)
{
   //echo("HapticFeatureList::onResize(" @ %width @ ", " @ %height @ ")");
   HapticFeatures.setExtent(616, %height + 34);
   HapticTestScroll->FeatureStack.updateStack();
}

