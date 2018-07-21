
function WebDemoControl::onWake(%this)
{
   return;
}

function WebDemoControl::JS_DemoBtnClick(%this, %arg1, %arg2, %arg3, %arg4, %arg5)
{
   echo("WebDemoControl::JS_DemoBtnClick() - called by " @ %this.internalName);
   echo("The text entered is: " @ %arg1);
   echo("The slider is at: " @ %arg2);
   echo("The checkbox value is: " @ %arg4);
   echo("The number is: " @ %arg3);
   echo("The user type is: " @ %arg5);
   return;
}

function WebDemoControl::onControlModeChange(%this, %isKeyboardMouse)
{
   %jsStr = "window.controllerNavigation=";
   %jsStr = %jsStr @ (%isKeyboardMouse ? "false;" : "true;");
   %this.execJavaScript(%jsStr);
   %this.isControllerMode = !%isKeyboardMouse;
}

function WebDemoControl::onLoadingStateChange(%this, %isLoading, %canGoBack, %canGoForward)
{
   echo("WebDemoControl::onLoadingStateChange() - called by " @ %this.internalName);
   echo("   %isLoading = " @ %isLoading @ ", %canGoBack = " @ %canGoBack @ ", %canGoForward = " @ %canGoForward);

   // Insert the virtual keyboard if there's a controller or joystick connected
   if (!%isLoading && isMethod("SDLInputManager", "numJoysticks") && (SDLInputManager::numJoysticks() > 0))
   {
      %jsStr = "var head=document.getElementsByTagName('head')[0];";
      %jsStr = %jsStr @ "var cssLink=document.createElement('link');";
      %jsStr = %jsStr @ "cssLink.rel='stylesheet';";
      %jsStr = %jsStr @ "cssLink.type='text/css';";
      %jsStr = %jsStr @ "cssLink.href = 't3d://data/webCtrlDemo/html/keyboard.css';";
      %jsStr = %jsStr @ "head.appendChild(cssLink);";

      %jsStr = %jsStr @ "var script = document.createElement('script');";
      %jsStr = %jsStr @ "script.type = 'text/javascript';";
      %jsStr = %jsStr @ "script.src = 't3d://data/webCtrlDemo/html/numpad.js';";
      %jsStr = %jsStr @ "script.charset='UTF - 8';";
      %jsStr = %jsStr @ "head.appendChild(script);";

      %jsStr = %jsStr @ "script = document.createElement('script');";
      %jsStr = %jsStr @ "script.type = 'text/javascript';";
      %jsStr = %jsStr @ "script.src = 't3d://data/webCtrlDemo/html/keyboard.js';";
      %jsStr = %jsStr @ "script.charset='UTF - 8';";
      %jsStr = %jsStr @ "head.appendChild(script);";

      if (($pref::cef::virtualKeySize < 1) || ($pref::cef::virtualKeySize > 5))
         $pref::cef::virtualKeySize = 3;
      %jsStr = %jsStr @ "window.virtualKeySize=" @ $pref::cef::virtualKeySize @ ";";
      if (%this.isControllerMode)
         %jsStr = %jsStr @ "window.controllerNavigation=true;";
      %this.execJavaScript(%jsStr);
   }
}

function GuiWebCtrl::onLoadError(%this, %errorCode, %errorText, %errorURL)
{
   echo("GuiWebCtrl::onLoadError() - called by " @ %this.internalName);
   echo("   errorCode: " @ %errorCode @ ", message: " @ %errorText);
   echo("   URL: " @ %errorURL);
}

function playDemoVid(%volume, %video)
{
   WebDemoControl.execJavaScript("StartVideo(" @ %volume @ ", \"" @ %video @ "\");");
}

//playDemoVid(0.2, "portal.webm");
//WebDemoControl.execJavaScript("HideVideoCtrl();");
//WebDemoControl.loadURL("www.torque3d.org");
//WebDemoControl.pageBack();
//WebDemoControl.urlPost("https://www.yourdomain.com/login", "userIdHint=XXXXXXXXXXXXXXXXX");
