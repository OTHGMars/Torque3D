
function HapticEffectBox::onWake(%this)
{
   return;
}

function HapticEffectBox::initFromEffect(%this, %effectObj, %deviceObj)
{
   %this.effectObj = %effectObj;
   %this.deviceObj = %deviceObj;

   %effectStack = %this->EffectStack;
   %header = %effectStack->HeaderLine;

   %this.effectType = %effectObj.getEffectType();
   %header->TypeBox.setText(%this.effectType @ " Effect");
   %this.effectId = %effectObj.getEffectId();
   %header->IdBox.setText("Effect ID: " @ %this.effectId);

   %this.showEffectFields(%effectObj, %deviceObj, %this.effectId);
   %this.updateButtonStates(%effectObj, %deviceObj, %this.effectId);

   %effectStack.updateStack();
   %this.extent.y = %effectStack.extent.y + 8;
   return;
}

function HapticEffectBox::showEffectFields(%this, %effectObj, %deviceObj, %effectId)
{
   if (%effectObj.hasDirection())
   {
      %this.initDirectionLine();
      %this->EffectStack->DirectionLine.setVisible(true);
   }
   else
      %this->EffectStack->DirectionLine.setVisible(false);

   // All effects have the replay line, but delay is disabled for l/r effect
   %replayLine = %this->EffectStack->ReplayLine;
   %replayData = %this.effectObj.getReplayData();
   %replayLine->ReplayLength.setText(getWord(%replayData, 0));
   %replayLine->ReplayDelay.setText(getWord(%replayData, 1));
   %replayLine->ReplayDelay.setActive(!(%this.effectType $= "LeftRight"));

   if (%effectObj.hasEnvelope())
   {
      %this.initEnvelopeLine();
      %this->EffectStack->EnvelopeLine.setVisible(true);
   }
   else
      %this->EffectStack->EnvelopeLine.setVisible(false);

   if (%this.effectType $= "Constant")
   {
      %this.initConstantLine();
      %this->EffectStack->ConstantLine.setVisible(true);
   }
   else
      %this->EffectStack->ConstantLine.setVisible(false);

   if (%this.effectObj.isPeriodicEffect())
   {
      %this.initPeriodicLine();
      %this->EffectStack->PeriodicLine.setVisible(true);
   }
   else
      %this->EffectStack->PeriodicLine.setVisible(false);

   if (%this.effectType $= "Ramp")
   {
      %this.initRampLine();
      %this->EffectStack->RampLine.setVisible(true);
   }
   else
      %this->EffectStack->RampLine.setVisible(false);

   if (%this.effectType $= "LeftRight")
   {
      %this.initLeftRightLine();
      %this->EffectStack->LeftRightLine.setVisible(true);
   }
   else
      %this->EffectStack->LeftRightLine.setVisible(false);

   if (%this.effectObj.isConditionEffect())
   {
      %this.initConditionLine();
      %this->EffectStack->ConditionLine.setVisible(true);
   }
   else
      %this->EffectStack->ConditionLine.setVisible(false);
}

function HapticEffectBox::initDirectionLine(%this)
{
   %dirCtrl = %this->EffectStack->DirectionLine;
   %coordType = %this.effectObj.getDirectionCoordType();
   %coordData = %this.effectObj.getDirectionCoordData();
   //echo("Coordinate Type: " @ %coordType);
   //echo("Coordinate Data: " @ %coordData);

   %popup = %dirCtrl->CoordPopup;
   %popup.notifyControl = %this;
   %popup.add("Polar Coordinates", 0);
   %popup.add("Cartesian Coordinates", 1);
   %popup.add("Spherical Coordinates", 2);
   %popup.lastCoordType = 0;
   if (%coordType $= "Cartesian")
      %popup.lastCoordType = 1;
   else if (%coordType $= "Spherical")
      %popup.lastCoordType = 2;

   %dirCtrl->DirParam1.setText(%coordData.x);
   %dirCtrl->DirParam2.setText(%coordData.y);
   %dirCtrl->DirParam3.setText(%coordData.z);

   %popup.setSelected(%popup.lastCoordType, true);
}

function HapticEffectBox::initEnvelopeLine(%this)
{
   %envBox = %this->EffectStack->EnvelopeLine;
   %envData = %this.effectObj.getEnvelopeData();
   //echo("Envelope Data: " @ %envData);

   %envBox->AttackLength.setText(getWord(%envData, 0));
   %envBox->AttackStrength.setValue(getWord(%envData, 1));
   %envBox->FadeLength.setText(getWord(%envData, 2));
   %envBox->FadeStrength.setValue(getWord(%envData, 3));
}

function HapticEffectBox::initConstantLine(%this)
{
   %constBox = %this->EffectStack->ConstantLine;
   %effectLevel = %this.effectObj.getConstantEffectData();
   echo("Constant Data: " @ %effectLevel);

   %constBox->EffectLevel.setValue(%effectLevel);
}

function HapticEffectBox::initPeriodicLine(%this)
{
   %periodicBox = %this->EffectStack->PeriodicLine;
   %periodicData = %this.effectObj.getPeriodicEffectData();
   echo("Periodic Data: " @ %periodicData);

   %periodicBox->Period.setText(getWord(%periodicData, 0));
   %periodicBox->Magnitude.setValue(getWord(%periodicData, 1));
   %periodicBox->Offset.setValue(getWord(%periodicData, 2));
   %periodicBox->Phase.setText(getWord(%periodicData, 3));
}

function HapticEffectBox::initRampLine(%this)
{
   %rampBox = %this->EffectStack->RampLine;
   %rampData = %this.effectObj.getRampEffectData();
   echo("Ramp Data: " @ %rampData);

   %rampBox->StartLevel.setValue(getWord(%rampData, 0));
   %rampBox->EndLevel.setValue(getWord(%rampData, 1));
}

function HapticEffectBox::initLeftRightLine(%this)
{
   %lrBox = %this->EffectStack->LeftRightLine;
   %lrData = %this.effectObj.getLeftRightEffectData();
   echo("LeftRight Data: " @ %lrData);

   %lrBox->LargeMag.setValue(getWord(%lrData, 0));
   %lrBox->SmallMag.setValue(getWord(%lrData, 1));
}

function HapticEffectBox::initConditionLine(%this)
{
   %condBox = %this->EffectStack->ConditionLine;

   %condData = %this.effectObj.getConditionEffectRSat();
   %condBox->RSatX.setValue(%condData.x);
   %condBox->RSatY.setValue(%condData.y);
   %condBox->RSatZ.setValue(%condData.z);

   %condData = %this.effectObj.getConditionEffectLSat();
   %condBox->LSatX.setValue(%condData.x);
   %condBox->LSatY.setValue(%condData.y);
   %condBox->LSatZ.setValue(%condData.z);

   %condData = %this.effectObj.getConditionEffectRCoeff();
   %condBox->RCoeffX.setValue(%condData.x);
   %condBox->RCoeffY.setValue(%condData.y);
   %condBox->RCoeffZ.setValue(%condData.z);

   %condData = %this.effectObj.getConditionEffectLCoeff();
   %condBox->LCoeffX.setValue(%condData.x);
   %condBox->LCoeffY.setValue(%condData.y);
   %condBox->LCoeffZ.setValue(%condData.z);

   %condData = %this.effectObj.getConditionEffectDeadzone();
   %condBox->DeadzoneX.setValue(%condData.x);
   %condBox->DeadzoneY.setValue(%condData.y);
   %condBox->DeadzoneZ.setValue(%condData.z);

   %condData = %this.effectObj.getConditionEffectCenter();
   %condBox->CenterX.setText(%condData.x);
   %condBox->CenterY.setText(%condData.y);
   %condBox->CenterZ.setText(%condData.z);
}

function HapticEffectBox::updateButtonStates(%this, %effectObj, %deviceObj, %effectId)
{
   %buttons = %this->EffectStack->ButtonLine;
   %buttons->LoadButton.setText((%effectId < 0) ? "Upload To Device" : "Unload From Device");
   %buttons->LoadButton.command = %this @ ".onLoadButton();";
   %buttons->UpdateButton.command = %this @ ".onUpdateButton();";
   %buttons->PlayButton.command = %this @ ".onPlayButton();";
   %buttons->StopButton.command = %this @ ".onStopButton();";
   %buttons->DeleteButton.command = %this @ ".onDeleteButton();";

   if ((%effectId > -1) && (%deviceObj.query() & $SDLTypeMasks::HapticStatus))
      %effectPlaying = (%deviceObj.getEffectStatus(%effectId) > 0);

   %buttons->UpdateButton.setActive(%effectId > -1);
   %buttons->PlayButton.setActive((%effectId > -1) && !%effectPlaying);
   %buttons->StopButton.setActive((%effectId > -1) && %effectPlaying);
}

function HapticEffectBox::updateEffectData(%this)
{
   if (%this.effectObj.hasDirection())
   {
      %dirCtrl = %this->EffectStack->DirectionLine;
      %dirSystem = %dirCtrl->CoordPopup.getText();
      %dirCoords = %dirCtrl->DirParam1.getText() SPC %dirCtrl->DirParam2.getText() SPC %dirCtrl->DirParam3.getText();
      %this.effectObj.setDirectionCoordType(getword(%dirSystem, 0));
      %this.effectObj.setDirectionCoordData(%dirCoords);
   }

   %replayLine = %this->EffectStack->ReplayLine;
   %length = %replayLine->ReplayLength.getText();
   %delay = %replayLine->ReplayDelay.getText();
   %this.effectObj.setReplaydata(%length, %delay);

   if (%this.effectObj.hasEnvelope())
      %this.updateEnvelopeData();

   if (%this.effectType $= "Constant")
      %this.updateConstantData();

   if (%this.effectObj.isPeriodicEffect())
      %this.updatePeriodicData();

   if (%this.effectType $= "Ramp")
      %this.updateRampData();

   if (%this.effectType $= "LeftRight")
      %this.updateLeftRightData();

   if (%this.effectObj.isConditionEffect())
      %this.updateConditionData();
}

function HapticEffectBox::updateEnvelopeData(%this)
{
   %envBox = %this->EffectStack->EnvelopeLine;
   %attackLength = %envBox->AttackLength.getText();
   %attackLevel = mRound(%envBox->AttackStrength.getValue());
   %fadeLength = %envBox->FadeLength.getText();
   %fadeLevel = mRound(%envBox->FadeStrength.getValue());

   %this.effectObj.setEnvelopeData(%attackLength, %attackLevel, %fadeLength, %fadeLevel);
}

function HapticEffectBox::updateConstantData(%this)
{
   %constBox = %this->EffectStack->ConstantLine;
   %level = mRound(%constBox->EffectLevel.getValue());
   %this.effectObj.setConstantEffectData(%level);
}

function HapticEffectBox::updatePeriodicData(%this)
{
   %periodicBox = %this->EffectStack->PeriodicLine;
   %period = %periodicBox->Period.getText();
   %magnitude = mRound(%periodicBox->Magnitude.getValue());
   %offset = mRound(%periodicBox->Offset.getValue());
   %phase = %periodicBox->Phase.getText();
   %this.effectObj.setPeriodicEffectData(%period, %magnitude, %offset, %phase);
}

function HapticEffectBox::updateRampData(%this)
{
   %rampBox = %this->EffectStack->RampLine;
   %rampStart = mRound(%rampBox->StartLevel.getValue());
   %rampEnd = mRound(%rampBox->EndLevel.getValue());
   %this.effectObj.setRampEffectData(%rampStart, %rampEnd);
}

function HapticEffectBox::updateLeftRightData(%this)
{
   %lrBox = %this->EffectStack->LeftRightLine;
   %largeMag = mRound(%lrBox->LargeMag.getValue());
   %smallMag = mRound(%lrBox->SmallMag.getValue());
   %this.effectObj.setLeftRightEffectData(%largeMag, %smallMag);
}

function HapticEffectBox::updateConditionData(%this)
{
   %condBox = %this->EffectStack->ConditionLine;

   %condData = mRound(%condBox->RSatX.getValue()) SPC
      mRound(%condBox->RSatY.getValue()) SPC mRound(%condBox->RSatZ.getValue());
   %this.effectObj.setConditionEffectRSat(%condData);

   %condData = mRound(%condBox->LSatX.getValue()) SPC
      mRound(%condBox->LSatY.getValue()) SPC mRound(%condBox->LSatZ.getValue());
   %this.effectObj.setConditionEffectLSat(%condData);

   %condData = mRound(%condBox->RCoeffX.getValue()) SPC
      mRound(%condBox->RCoeffY.getValue()) SPC mRound(%condBox->RCoeffZ.getValue());
   %this.effectObj.setConditionEffectRCoeff(%condData);

   %condData = mRound(%condBox->LCoeffX.getValue()) SPC
      mRound(%condBox->LCoeffY.getValue()) SPC mRound(%condBox->LCoeffZ.getValue());
   %this.effectObj.setConditionEffectLCoeff(%condData);

   %condData = mRound(%condBox->DeadzoneX.getValue()) SPC
      mRound(%condBox->DeadzoneY.getValue()) SPC mRound(%condBox->DeadzoneZ.getValue());
   %this.effectObj.setConditionEffectDeadzone(%condData);

   %condData = %condBox->CenterX.getText() SPC %condBox->CenterY.getText() SPC %condBox->CenterZ.getText();
   %this.effectObj.setConditionEffectCenter(%condData);
}

function HapticEffectBox::onLoadButton(%this)
{
   echo("onLoadButton(" @ %this @ ")");
   %effectStack = %this->EffectStack;
   if (%this.effectId < 0)
   {
      %this.updateEffectData();
      %effectId = %this.deviceObj.uploadEffect(%this.effectObj);
      if (%effectId < 0)
         return;  //Failed to upload

      %this.effectId = %effectId;
      %effectStack->HeaderLine->IdBox.setText("Effect ID: " @ %this.effectId);
      %this.updateButtonStates(%this.effectObj, %this.deviceObj, %effectId);
   }
   else
   {
      %retVal = %this.deviceObj.unloadEffect(%this.effectObj);
      %this.effectId = -1;
      %effectStack->HeaderLine->IdBox.setText("Effect ID: -1");
      %this.updateButtonStates(%this.effectObj, %this.deviceObj, -1);
   }
   return;
}

function HapticEffectBox::onUpdateButton(%this)
{
   echo("onUpdateButton(" @ %this @ ")");
   %this.updateEffectData();
   %this.deviceObj.updateEffect(%this.effectObj);
   return;
}

function HapticEffectBox::onPlayButton(%this)
{
   echo("onPlayButton(" @ %this @ ")");
   %this.updateEffectData();
   %this.deviceObj.updateEffect(%this.effectObj);

   // Get the iterations count
   %buttons = %this->EffectStack->ButtonLine;
   %iterations = %buttons->Iterations.getText();
   if (%iterations $= "Loop")
      %iterations = -1;
   else
   {
      %iterations = mRound(%iterations);
      if (%iterations < 1)
         %iterations = 1;
   }

   %retVal = %this.deviceObj.runEffect(%this.effectObj, %iterations);
   if (%retVal)
   {
      %buttons->PlayButton.setActive(false);
      %buttons->StopButton.setActive(true);

      %replayData = %this.effectObj.getReplayData();
      %runTime = %replayData.x + %replayData.y;
      if (%iterations > 0)
      {
         %runTime *= %iterations;
         %this.runSched = %this.schedule(%runTime, "updateButtonStates", %this.effectObj, %this.deviceObj, %this.effectId);
      }
   }
   return;
}

function HapticEffectBox::onStopButton(%this)
{
   echo("onStopButton(" @ %this @ ")");
   if (isEventPending(%this.runSched))
      cancel(%this.runSched);

   %this.deviceObj.stopEffect(%this.effectObj);
   %buttons = %this->EffectStack->ButtonLine;
   %buttons->PlayButton.setActive(true);
   %buttons->StopButton.setActive(false);
   return;
}

function HapticEffectBox::onDeleteButton(%this)
{
   echo("onDeleteButton(" @ %this @ ")");
   %this.deviceObj.removeEffect(%this.effectObj);
   HapticTestDlg.schedule(32, "initFromDevice", HapticTestDlg.selectedDeviceId);
}

function HapticEffectBox::onCoordTypeChange(%this, %coordPopup, %typeId)
{
   %dirCtrl = %this->EffectStack->DirectionLine;
   %dirCtrl->DirParam2.setActive(%typeId > 0);
   %dirCtrl->DirParam3.setActive(%typeId == 1);

   if (%typeId != %coordPopup.lastCoordType)
   {  // Convert the coordinates between systems
      %oldCoords = %dirCtrl->DirParam1.getText() SPC %dirCtrl->DirParam2.getText() SPC %dirCtrl->DirParam3.getText();
      %newCoords = %this.effectObj.convertCoords(%coordPopup.lastCoordType, %typeId, %oldCoords);
      %dirCtrl->DirParam1.setText(%newCoords.x);
      %dirCtrl->DirParam2.setText(%newCoords.y);
      %dirCtrl->DirParam3.setText(%newCoords.z);
   }
   %coordPopup.lastCoordType = %typeId;
}

function SDLCoordTypePopup::onSelect(%this, %idVal, %name)
{
   echo("Selected #" @ %idVal @ ": " @ %name);
   %this.notifyControl.onCoordTypeChange(%this, %idVal);
}
