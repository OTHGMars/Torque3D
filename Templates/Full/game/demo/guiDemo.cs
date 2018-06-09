if( !isObject( GuiBillBoardTextProfile ) )
new GuiControlProfile( GuiBillBoardTextProfile : GuiTextProfile )
{
   justify = "Center";
   fontSize = 54;
   category = "Demo";
};

if( !isObject( GuiGooCheckProfile ) )
new GuiControlProfile( GuiGooCheckProfile : GuiCheckBoxProfile )
{
   fontSize = 24;
   category = "Demo";
};

if( !isObject( GuiBigTextCProfile ) )
new GuiControlProfile( GuiBigTextCProfile : GuiBigTextProfile )
{
   justify = "Center";
   category = "Demo";
};

function setupGuiTargets()
{
   // Music player canvas
   if (!isObject(ObjCanvasOne))
   {
      new GuiOffscreenCanvas(ObjCanvasOne) {
         targetName = "objectcanvas";
         targetSize = "1050 700";
         dynamicTarget = false;
         canInteract = true;
         maxInteractDistance = "3";
      };
   }
   if ( !isObject(GooBackground) )
   {
      exec("demo/GooBack.gui");
      if ( !isObject(GuiMusicPlayer) )
         exec("core/scripts/gui/guiMusicPlayer.cs");
      GooBackground-->FrameCounter.variable = "ObjCanvasOne.renderCount";
      GooBackground-->ContentFrame.add(GuiMusicPlayer);
   }
   ObjCanvasOne.setContent(GooBackground);
   ObjCanvasOne.hideCursor();

   // Billboard canvas
   if (!isObject(ObjCanvasBB))
   {
      new GuiOffscreenCanvas(ObjCanvasBB) {
         targetName = "objectcanvas_bb";
         targetSize = "1050 700";
         dynamicTarget = false;
         canInteract = false;
      };
   }
   if ( !isObject(GuiBillboard) )
   {
      exec("demo/GuiBillboard.gui");
      GuiBillboard-->FrameCounter.variable = "ObjCanvasBB.renderCount";
   }
   ObjCanvasBB.setContent(GuiBillboard);
   ObjCanvasBB.hideCursor();

   // Billboard controls canvas
   if (!isObject(BBControlCanvas))
   {
      new GuiOffscreenCanvas(BBControlCanvas) {
         targetName = "bbcontrolscanvas";
         targetSize = "1050 700";
         dynamicTarget = false;
         canInteract = true;
         maxInteractDistance = "3";
         canActivate = true;
      };
   }
   if ( !isObject(GuiBBControls) )
   {
      exec("demo/GuiBBControl.gui");
      GuiBBControls-->FrameCounter.variable = "BBControlCanvas.renderCount";
   }
   BBControlCanvas.setContent(GuiBBControls);
   BBControlCanvas.hideCursor();

   // Create an actionMap for interacting
   if ( !isObject(GooCanvasMap) )
   {
      new ActionMap(GooCanvasMap);
      GooCanvasMap.bind(mouse0, "button0", OffscreenCanvasClick);
   }

   if (isObject(Reticle))
   {
      Reticle.showCursorAsReticle = true;
      Reticle.maxInteractDistance = 5.0;
   }
}

function startGuiDemo()
{
   // Yes, we really need to set these here. If the missingTexturePath is not
   // defined, the "#targetName" method of selecting a render target as
   // diffuse map in a material will fail...
   $pref::Video::missingTexturePath = "core/art/missingTexture.png";
   $pref::Video::UnAvailableTexturePath = "core/art/unavailable.png";
   $pref::Video::WarningTexturePath = "core/art/warnMat.dds";

   // Make the offscreen canvases
   setupGuiTargets();
}

function OffscreenCanvasClick(%val)
{
   if (isObject($InteractingCanvas))
   {
      if ($InteractingCanvas.canActivate)
      {
         if (%val)
         {
            $InteractingCanvas.setActiveCanvas(true);
            $InteractingCanvas.showCursor();
            $InteractingCanvas.dynamicTarget = true;
            $InteractingCanvas.markDirty();
            GlobalActionMap.bind(mouse0, "button1", RemoveCanvasFocus);
            GooCanvasMap.pop();
            moveMap.pop();
            Reticle.visible = false;
         }
         return;
      }
      if ($InteractingCanvas.canInteract)
      {
         $InteractingCanvas.cursorClick(0, %val);
         $InteractingCanvas.markDirty();
      }
   }
}

function RemoveCanvasFocus(%val)
{
   if (%val)
      return;

   if (isObject($InteractingCanvas))
   {
      $InteractingCanvas.setActiveCanvas(false);
      if (Reticle.showCursorAsReticle)
         $InteractingCanvas.hideCursor();
      $InteractingCanvas.dynamicTarget = false;
   }
   Reticle.visible = true;
   GlobalActionMap.unbind(mouse0, "button1");
   moveMap.push();
   GooCanvasMap.push();
}

function GuiOffscreenCanvas::checkCursor(%this)
{
   if (!Reticle.visible || !Reticle.showCursorAsReticle)
   {
      %count = %this.getCount();
      for(%i = 0; %i < %count; %i++)
      {
         %control = %this.getObject(%i);
         if ((%control.noCursor $= "") || !%control.noCursor)
         {
            %this.showCursor();
            return true;
         }
      }
   }

   // If we get here, every control requested a hidden cursor, so we oblige.
   %this.hideCursor();
   return false;
}

function GuiCrossHairHud::onStartCanvasInteract(%this, %gooCanvasID)
{
   echo("GuiCrossHairHud::onStartCanvasInteract("@%this@", "@%gooCanvasID@")");
   if (isObject(%gooCanvasID))
   {
      $InteractingCanvas = %gooCanvasID;
      if (!Reticle.showCursorAsReticle)
         $InteractingCanvas.showCursor();
      GooCanvasMap.push();
   }
   else
   {
      if (isObject($InteractingCanvas))
      {
         $InteractingCanvas.hideCursor();
         $InteractingCanvas.markDirty();
         $InteractingCanvas = 0;
      }
      GooCanvasMap.pop();
   }
}

function GuiBBControls::updateBackgroundColor(%this, %color)
{
   %this-->ColorSwatch.color = %color;
}

function UpdateBillboard()
{
   %text = GuiBBControls-->TextBox.getText();
   GuiBillboard-->BB_Text.setText(%text);
   GuiBillboard-->SolidBackground.color = GuiBBControls-->ColorSwatch.color;
   ObjCanvasBB.markDirty();
}

function setGameCanvas(%gooCanvas, %isActive)
{
   echo("setGameCanvas("@%gooCanvas@", "@%isActive@")");
   $GameCanvas = %isActive ? %gooCanvas : Canvas;
}

//<shadowcolor:00bb00><shadow:2:3><color:0000bb>Programmer Art FTW!!!
//ObjCanvasOne.dumpTarget("ObjCanvasOne.png");
//exec("demo/guiDemo.cs");startGuiDemo();