//-----------------------------------------------------------------------------
// Callback functions for Action Set MenuControls
//-----------------------------------------------------------------------------
$MouseXNudge = 0;
$MouseYNudge = 0;
function in_menu_mouse(%controller, %xAxis, %yAxis, %zAxis)
{
   //echo("in_menu_mouse( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ", " @ %zAxis @ ")");
   //%xAxis = (%xAxis > 0) ? %xAxis + 1 : %xAxis - 1;
   //%yAxis = (%yAxis > 0) ? %yAxis + 1 : %yAxis - 1;
   $MouseXNudge += %xAxis;
   $MouseYNudge += %yAxis;
   
   %xNudge = 0;
   while (mAbs($MouseXNudge) >= 1)
   {
      %xNudge += ($MouseXNudge > 1) ? 1 : -1;
      $MouseXNudge -= %xNudge;
   }

   %yNudge = 0;
   while (mAbs($MouseYNudge) >= 1)
   {
      %yNudge += ($MouseYNudge > 1) ? 1 : -1;
      $MouseYNudge -= %yNudge;
   }

   if ((%xNudge != 0) || (%yNudge != 0))
   {
      $GameCanvas.cursorNudge(%xNudge, -%yNudge);
   }
}

function in_menu_lmb(%controller, %state)
{
   //echo("in_menu_lmb( " @ %controller @ ", " @ %state @ ")");
   $GameCanvas.cursorClick(0, %state);
}

function in_menu_rmb(%controller, %state)
{
   //echo("in_menu_rmb( " @ %controller @ ", " @ %state @ ")");
   $GameCanvas.cursorClick(1, %state);
}

function in_overlay_closer(%controller, %state)
{
   //echo("in_overlay_closer( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      OVRMoveOverlay(1);
}

function in_overlay_farther(%controller, %state)
{
   //echo("in_overlay_farther( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      OVRMoveOverlay(-1);
}

function in_menu_up(%controller, %state)
{
   echo("in_menu_up( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_down(%controller, %state)
{
   echo("in_menu_down( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_left(%controller, %state)
{
   echo("in_menu_left( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_right(%controller, %state)
{
   echo("in_menu_right( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_next(%controller, %state)
{
   //echo("in_menu_next( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_last(%controller, %state)
{
   echo("in_menu_last( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_back(%controller, %state)
{
   echo("in_menu_back( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_select(%controller, %state)
{
   echo("in_menu_select( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_submit(%controller, %state)
{
   echo("in_menu_submit( " @ %controller @ ", " @ %state @ ")");
}

function in_menu_cancel(%controller, %state)
{
   echo("in_menu_cancel( " @ %controller @ ", " @ %state @ ")");
   if (!%state)
      OVROverlayCanvas.showCanvas(false);
}

function in_pause_menu(%controller, %state)
{
   echo("in_pause_menu( " @ %controller @ ", " @ %state @ ")");
}
