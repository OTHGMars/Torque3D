//-----------------------------------------------------------------------------
// Callback functions for Action Set MenuControls
//-----------------------------------------------------------------------------

function si_menu_mouse(%controller, %xAxis, %yAxis)
{
   //echo("si_menu_mouse( " @ %controller @ ", " @ %xAxis @ ", " @ %yAxis @ ")");
}

function si_menu_lmb(%controller, %state)
{
   //echo("si_menu_lmb( " @ %controller @ ", " @ %state @ ")");
   $GameCanvas.cursorClick(0, %state);
}

function si_menu_rmb(%controller, %state)
{
   //echo("si_menu_rmb( " @ %controller @ ", " @ %state @ ")");
   $GameCanvas.cursorClick(1, %state);
}

function si_menu_up(%controller, %state)
{
   echo("si_menu_up( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_down(%controller, %state)
{
   echo("si_menu_down( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_left(%controller, %state)
{
   echo("si_menu_left( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_right(%controller, %state)
{
   echo("si_menu_right( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_next(%controller, %state)
{
   echo("si_menu_next( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_last(%controller, %state)
{
   echo("si_menu_last( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_back(%controller, %state)
{
   echo("si_menu_back( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_select(%controller, %state)
{
   echo("si_menu_select( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_submit(%controller, %state)
{
   echo("si_menu_submit( " @ %controller @ ", " @ %state @ ")");
}

function si_menu_cancel(%controller, %state)
{
   echo("si_menu_cancel( " @ %controller @ ", " @ %state @ ")");
}

function si_pause_menu(%controller, %state)
{
   //echo("si_pause_menu( " @ %controller @ ", " @ %state @ ")");
   if (%state)
      SteamInput::showBindingPanel(%controller);
}
