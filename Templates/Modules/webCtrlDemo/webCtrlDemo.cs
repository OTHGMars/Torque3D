//-----------------------------------------------------------------------------
// Module creation functions.
//-----------------------------------------------------------------------------

function webCtrlDemo::create( %this )
{
   exec("./scripts/customProfiles.cs");
   exec("./scripts/webCursors.cs");
   exec("./scripts/demoGui.cs");
   exec("./scripts/gui/demoGui.gui");
   exec("./scripts/browserGui.cs");
   exec("./scripts/gui/browserGui.gui");

   if (isObject(MainMenuGui))
   {
      %testBtn = new GuiButtonCtrl() {
         text = "Web Demo";
         groupNum = "-1";
         buttonType = "PushButton";
         useMouseEvents = "0";
         position = "0 0";
         extent = "200 40";
         minExtent = "8 8";
         horizSizing = "right";
         vertSizing = "bottom";
         profile = "GuiBlankMenuButtonProfile";
         visible = "1";
         active = "1";
         command = "webCtrlDemo.toggleDemo();";
         tooltipProfile = "GuiToolTipProfile";
         tooltip = "Web demo window";
         isContainer = "0";
         canSave = "0";
         canSaveDynamicFields = "0";
      };

      %browserBtn = new GuiButtonCtrl() {
         text = "Web Browser";
         groupNum = "-1";
         buttonType = "PushButton";
         useMouseEvents = "0";
         position = "0 0";
         extent = "200 40";
         minExtent = "8 8";
         horizSizing = "right";
         vertSizing = "bottom";
         profile = "GuiBlankMenuButtonProfile";
         visible = "1";
         active = "1";
         command = "webCtrlDemo.toggleBrowser();";
         tooltipProfile = "GuiToolTipProfile";
         tooltip = "Web browser window";
         isContainer = "0";
         canSave = "0";
         canSaveDynamicFields = "0";
      };

      if (!isObject(MMTestContainer))
      {
         new GuiDynamicCtrlArrayControl(MMTestContainer) {
            colCount = "0";
            colSize = "200";
            rowCount = "0";
            rowSize = "40";
            rowSpacing = "2";
            colSpacing = "0";
            frozen = "0";
            autoCellSize = "0";
            fillRowFirst = "1";
            dynamicSize = "1";
            padding = "0 0 0 0";
            position = "0 0";
            extent = "200 40";
            minExtent = "8 2";
            horizSizing = "right";
            vertSizing = "bottom";
            profile = "GuiDefaultProfile";
            visible = "1";
            active = "1";
            tooltipProfile = "GuiToolTipProfile";
            hovertime = "1000";
            isContainer = "1";
            canSave = "0";
            canSaveDynamicFields = "0";
         };
         MainMenuGui.add(MMTestContainer);
      }

      MMTestContainer.add(%testBtn);
      MMTestContainer.add(%browserBtn);
   }

   %this.startupCEF();
}

function webCtrlDemo::destroy( %this )
{
   
}

function webCtrlDemo::toggleDemo( %this )
{
   if (WebDemoDlg.isAwake())
      $GameCanvas.popDialog(WebDemoDlg);
   else
      $GameCanvas.pushDialog(WebDemoDlg);
}

function webCtrlDemo::toggleBrowser( %this )
{
   if (WebBrowserGui.isAwake())
      $GameCanvas.popDialog(WebBrowserGui);
   else
      $GameCanvas.pushDialog(WebBrowserGui);
}

function webCtrlDemo::startupCEF( %this )
{
   // Location for web cache files. Default is "cef/webcache". "" will disable
   // cache write. Cef will need write permission to this directory.
   //$Cef::cachePath = getUserPath() @ "/webcache";
   $Cef::cachePath = "cef/webcache";

   // The locale string that will be passed to Blink. If empty the default locale
   // of "en - US" will be used. This value is ignored on Linux where locale is
   // determined using environment variable parsing with the precedence order:
   // LANGUAGE, LC_ALL, LC_MESSAGES and LANG. Locale files are located in 
   // game/cef/locales
   $Cef::localeString = "";

   // The directory and file name to use for the debug log. If empty, the default
   // name of "debug.log" will be used and the file will be written to the
   // application directory.  Cef will need write permission to this file.
   //$Cef::logPath = getUserPath() @ "/cef.log";
   $Cef::logPath = "cef/cef.log";

   // The log severity. Only messages of this severity level or higher will be
   // logged. Options are: Default, Verbose, Info, Warning, Error and None
   $Cef::logSeverity = "Warning";

   // The fully qualified path for the resources directory. If this value is empty
   // the cef.pak and/or devtools_resources.pak files must be located in the module
   // directory on Windows/Linux or the app bundle Resources directory on Mac OS X.
   $Cef::resourcePath = getMainDotCsDir() @ "/cef";

   // The fully qualified path for the locales directory. If this value is empty the
   // locales directory must be located in the module directory. This value is ignored
   // on Mac OS X where pack files are always loaded from the app bundle Resources directory.
   $Cef::localesPath = getMainDotCsDir() @ "/cef/locales";

   // Now that the values are set, start the cef process.
   WebEngine::initializeCEF();

   // Convert our local html path
   WebDemoControl.StartURL = getMainDotCsDir() @ "/data/webCtrlDemo/html/demoTestGui.html";
   
   %this.loadControllerMap();
}

function webCtrlDemo::loadControllerMap( %this )
{  // Map navigation keys to gamepad events for controller navigation
   WebEngine::mapDeviceEvent("gamepad", "btn_r", "tab");
   WebEngine::mapDeviceEvent("gamepad", "btn_l", "shift tab");
   WebEngine::mapDeviceEvent("gamepad", "btn_b", "enter");
   WebEngine::mapDeviceEvent("gamepad", "btn_a", "space");
   WebEngine::mapDeviceEvent("gamepad", "upov", "up");
   WebEngine::mapDeviceEvent("gamepad", "dpov", "down");
   WebEngine::mapDeviceEvent("gamepad", "lpov", "left");
   WebEngine::mapDeviceEvent("gamepad", "rpov", "right");
   WebEngine::mapDeviceEvent("gamepad", "btn_back", "GoBack");
   WebEngine::mapDeviceEvent("gamepad", "btn_start", "GoForward");
}
