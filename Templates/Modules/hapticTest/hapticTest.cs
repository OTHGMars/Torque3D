//-----------------------------------------------------------------------------
// Module creation functions.
//-----------------------------------------------------------------------------

function hapticTest::create( %this )
{
   exec("./scripts/customProfiles.cs");
   exec("./scripts/effectBox.cs");
   exec("./scripts/gui/effectBox.gui");
   exec("./scripts/hapticTest.cs");
   exec("./scripts/gui/hapticTest.gui");

   if (isObject(MainMenuGui))
   {
      %testBtn = new GuiButtonCtrl() {
         text = "Haptic Playground";
         groupNum = "-1";
         buttonType = "PushButton";
         useMouseEvents = "0";
         position = "0 40";
         extent = "200 40";
         minExtent = "8 8";
         horizSizing = "right";
         vertSizing = "bottom";
         profile = "GuiBlankMenuButtonProfile";
         visible = "1";
         active = "1";
         command = "Canvas.pushDialog(HapticTestDlg);";
         tooltipProfile = "GuiToolTipProfile";
         isContainer = "0";
         canSave = "0";
         canSaveDynamicFields = "0";
      };
      MainMenuGui.add(%testBtn);
   }
}

function hapticTest::destroy( %this )
{
   
}
