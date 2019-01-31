//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// initializeCanvas
// Constructs and initializes the default canvas window.
//---------------------------------------------------------------------------------------------
$canvasCreated = false;
$Video::ModeTags = "Windowed Borderless Fullscreen";

function configureCanvas()
{
   if (($pref::Video::deviceId $= "") ||
         ($pref::Video::deviceId >= Canvas.getMonitorCount()))
      $pref::Video::deviceId = 0;  // Monitor 0

   if ($pref::Video::deviceMode $= "")
      $pref::Video::deviceMode = 1; // Borderless

   // Default to borderless at desktop resolution if there is no saved pref
   if ($pref::Video::Resolution $= "")
   {
      $pref::Video::mode = Canvas.getBestCanvasRes($pref::Video::deviceId, $pref::Video::deviceMode);
      Canvas.modeStrToPrefs($pref::Video::mode);
   }

   if ($pref::Video::deviceMode < 2)
      $pref::Video::FullScreen = false;
   %modeStr = Canvas.prefsToModeStr();

   echo("--------------");
   echo("Attempting to set resolution to \"" @ %modeStr @ "\"");

   // Make sure we are running at a valid resolution
   if (!Canvas.checkCanvasRes(%modeStr, $pref::Video::deviceId, $pref::Video::deviceMode, true))
   {
      %modeStr = Canvas.getBestCanvasRes($pref::Video::deviceId, $pref::Video::deviceMode);
      Canvas.modeStrToPrefs(%modeStr);
   }

   %fsLabel = getWord($Video::ModeTags, $pref::Video::deviceMode);
   %resX = $pref::Video::Resolution.x;
   %resY = $pref::Video::Resolution.y;
   %bpp  = $pref::Video::BitDepth;
   %rate = $pref::Video::RefreshRate;
   %fsaa = $pref::Video::AA;
   %fs = ($pref::Video::deviceMode == 2);

   echo("Accepted Mode: " NL
      "--Resolution     : " @  %resX SPC %resY NL
      "--Screen Mode    : " @ %fsLabel NL
      "--Bits Per Pixel : " @ %bpp NL
      "--Refresh Rate   : " @ %rate NL
      "--FSAA Level     : " @ %fsaa NL
      "--------------");

   // Actually set the new video mode
   Canvas.setVideoMode(%resX, %resY, %fs, %bpp, %rate, %fsaa);

   // FXAA piggybacks on the FSAA setting in $pref::Video::mode.
   if ( isObject( FXAA_PostEffect ) )
      FXAA_PostEffect.isEnabled = ( %fsaa > 0 ) ? true : false;

   //if ( $pref::Video::autoDetect )
   //   GraphicsQualityAutodetect();
}

function initializeCanvas()
{
   // Don't duplicate the canvas.
   if($canvasCreated)
   {
      error("Cannot instantiate more than one canvas!");
      return;
   }

   if (!createCanvas())
   {
      error("Canvas creation failed. Shutting down.");
      quit();
   }

   $canvasCreated = true;
}

//---------------------------------------------------------------------------------------------
// resetCanvas
// Forces the canvas to redraw itself.
//---------------------------------------------------------------------------------------------
function resetCanvas()
{
   if (isObject(Canvas))
      Canvas.repaint();
}

//---------------------------------------------------------------------------------------------
// Callbacks for window events.
//---------------------------------------------------------------------------------------------

function GuiCanvas::onLoseFocus(%this)
{
}

//---------------------------------------------------------------------------------------------
// Full screen handling
//---------------------------------------------------------------------------------------------

function GuiCanvas::attemptFullscreenToggle(%this)
{
   // If the Editor is running then we cannot enter full screen mode
   if ( EditorIsActive() && !%this.isFullscreen() )
   {
      MessageBoxOK("Windowed Mode Required", "Please exit the Mission Editor to switch to full screen.");
      return;
   }

   // If the GUI Editor is running then we cannot enter full screen mode
   if ( GuiEditorIsActive() && !%this.isFullscreen() )
   {
      MessageBoxOK("Windowed Mode Required", "Please exit the GUI Editor to switch to full screen.");
      return;
   }

   %this.toggleFullscreen();
}

//---------------------------------------------------------------------------------------------
// Editor Checking
// Needs to be outside of the tools directory so these work in non-tools builds
//---------------------------------------------------------------------------------------------

function EditorIsActive()
{
   return ( isObject(EditorGui) && Canvas.getContent() == EditorGui.getId() );
}

function GuiEditorIsActive()
{
   return ( isObject(GuiEditorGui) && Canvas.getContent() == GuiEditorGui.getId() );
}

function GuiCanvas::modeStrToPrefs(%this, %modeStr)
{
   $pref::Video::Resolution = %modeStr.x SPC %modeStr.y;
   $pref::Video::FullScreen = getWord(%modeStr, $WORD::FULLSCREEN);
   $pref::Video::BitDepth = getWord(%modeStr, $WORD::BITDEPTH);
   $pref::Video::RefreshRate = getWord(%modeStr, $WORD::REFRESH);
   $pref::Video::AA = getWord(%modeStr, $WORD::AA);
}

function GuiCanvas::prefsToModeStr(%this)
{
   %modeStr = $pref::Video::Resolution SPC $pref::Video::FullScreen SPC
      $pref::Video::BitDepth SPC $pref::Video::RefreshRate SPC $pref::Video::AA;

   return %modeStr;
}

function GuiCanvas::checkCanvasRes(%this, %mode, %deviceId, %deviceMode, %startup)
{
   %deviceRect = getWords(%this.getMonitorRect(%deviceId), 2);
   %resX = getWord(%mode, $WORD::RES_X);
   %resY = getWord(%mode, $WORD::RES_Y);

   if ((%resX > %deviceRect.x) || (%resY > %deviceRect.y))
      return false;  // Bigger than monitor is bad

   if (%deviceMode == 0)
   {  // Windowed must be smaller than the device for title bar and border
      if ((%resY > (%deviceRect.y - 60)) || (%resX > (%deviceRect.x - 2)))
         return false;
      return true;
   }
   else if (%deviceMode == 1)
   {  // Borderless must be at the device res
      if ((%resX != %deviceRect.x) || (%resY != %deviceRect.y))
         return false;

      return true;
   }

   if (!%startup)
      return true;

   // Checking saved prefs, make sure the mode still exists
   %bpp = getWord(%mode, $WORD::BITDEPTH);
   %rate = getWord(%mode, $WORD::REFRESH);

   %resCount = %this.getMonitorModeCount(%deviceId);
   for (%i = (%resCount - 1); %i >= 0; %i--)
   {
      %testRes = %this.getMonitorMode(%deviceId, %i);
      %testResX = getWord(%testRes, $WORD::RES_X);
      %testResY = getWord(%testRes, $WORD::RES_Y);
      %testBPP  = getWord(%testRes, $WORD::BITDEPTH);
      %testRate = getWord(%testRes, $WORD::REFRESH);

      if ((%testResX == %resX) && (%testResY == %resY) &&
            (%testBPP == %bpp) && (%testRate == %rate))
         return true;
   }

   return false;
}

// Find the best video mode setting for the device and display mode
function GuiCanvas::getBestCanvasRes(%this, %deviceId, %deviceMode)
{
   %deviceRect = getWords(%this.getMonitorRect(%deviceId), 2);

   %resCount = %this.getMonitorModeCount(%deviceId);
   for (%i = 0; %i < %resCount; %i++)
   {
      %testRes = %this.getMonitorMode(%deviceId, %i);
      %resX = getWord(%testRes, $WORD::RES_X);
      %resY = getWord(%testRes, $WORD::RES_Y);

      if ((%resX > %deviceRect.x) || (%resY > %deviceRect.y))
         continue;
      else if ((%deviceMode == 0) && (%resY == %deviceRect.y))
         continue;
      else if ((%deviceMode == 1) && ((%resX != %deviceRect.x) || (%resY != %deviceRect.y)))
         continue;  // Borderless must be at the device res

      return %testRes;
   }

   // Nothing found? return first mode
   return %this.getMonitorMode(%deviceId, 0);
}