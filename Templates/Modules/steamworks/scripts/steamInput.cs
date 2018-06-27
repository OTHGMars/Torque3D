//-----------------------------------------------------------------------------
// SteamInput setup and callbacks
//-----------------------------------------------------------------------------

function steamworks::initInput( %this )
{  // The strings used for action sets and action names must match those set in
   // the In-Game Actions file, game_actions_x.vdf.

   %this.actionSets = 0;
   %this.setLayers = 0;
   %this.addActionSet("Menu", "MenuControls");
   %this.addActionSet("Player", "PlayerControls");
   %this.addActionSet("Vehicle", "VehicleControls");

   // Load the menu actions
   %this.addAnalogAction("Menu", "menu_mouse", "si_menu_mouse");
   %this.addDigitalAction("Menu", "menu_lmb", "si_menu_lmb");
   %this.addDigitalAction("Menu", "menu_rmb", "si_menu_rmb");
   %this.addDigitalAction("Menu", "menu_up", "si_menu_up");
   %this.addDigitalAction("Menu", "menu_down", "si_menu_down");
   %this.addDigitalAction("Menu", "menu_left", "si_menu_left");
   %this.addDigitalAction("Menu", "menu_right", "si_menu_right");
   %this.addDigitalAction("Menu", "menu_next", "si_menu_next");
   %this.addDigitalAction("Menu", "menu_last", "si_menu_last");
   %this.addDigitalAction("Menu", "menu_back", "si_menu_back");
   %this.addDigitalAction("Menu", "menu_select", "si_menu_select");
   %this.addDigitalAction("Menu", "menu_submit", "si_menu_submit");
   %this.addDigitalAction("Menu", "menu_cancel", "si_menu_cancel");
   %this.addDigitalAction("Menu", "menu_menu", "si_pause_menu");

   // Load the player actions
   %this.addAnalogAction("Player", "player_move", "si_player_move");
   %this.addAnalogAction("Player", "player_camera", "si_player_camera");
   %this.addDigitalAction("Player", "player_fire", "si_any_fire");
   %this.addDigitalAction("Player", "player_altfire", "si_any_altfire");
   %this.addDigitalAction("Player", "player_jump", "si_player_jump");
   %this.addDigitalAction("Player", "player_crouch", "si_player_crouch");
   %this.addDigitalAction("Player", "player_sprint", "si_player_sprint");
   %this.addDigitalAction("Player", "player_weapup", "si_player_weapup");
   %this.addDigitalAction("Player", "player_weapdown", "si_player_weapdown");
   %this.addDigitalAction("Player", "player_weapreload", "si_player_weapreload");
   %this.addDigitalAction("Player", "player_tossweapon", "si_player_tossweapon");
   %this.addDigitalAction("Player", "player_tossammo", "si_player_tossammo");
   %this.addDigitalAction("Player", "player_carjack", "si_player_carjack");
   %this.addDigitalAction("Player", "player_camtoggle", "si_any_camtoggle");
   %this.addDigitalAction("Player", "player_freelook", "si_any_freelook");
   %this.addDigitalAction("Player", "player_zoom", "si_player_zoom");
   %this.addDigitalAction("Player", "player_interact", "si_player_interact");
   %this.addDigitalAction("Player", "player_menu", "si_player_menu");

   // Load the vehicle actions
   %this.addAnalogAction("Vehicle", "vehicle_steer", "si_vehicle_steer");
   %this.addAnalogAction("Vehicle", "vehicle_throttle", "si_vehicle_throttle");
   %this.addAnalogAction("Vehicle", "vehicle_reverse", "si_vehicle_reverse");
   %this.addDigitalAction("Vehicle", "vehicle_fire", "si_any_fire");
   %this.addDigitalAction("Vehicle", "vehicle_altfire", "si_any_altfire");
   %this.addDigitalAction("Vehicle", "vehicle_brake", "si_vehicle_brake");
   %this.addDigitalAction("Vehicle", "vehicle_brakelight", "si_vehicle_brakelight");
   %this.addDigitalAction("Vehicle", "vehicle_getout", "si_vehicle_getout");
   %this.addDigitalAction("Vehicle", "vehicle_flipcar", "si_vehicle_flipcar");
   %this.addDigitalAction("Vehicle", "vehicle_camtoggle", "si_any_camtoggle");
   %this.addDigitalAction("Vehicle", "vehicle_freelook", "si_any_freelook");
   %this.addDigitalAction("Vehicle", "vehicle_menu", "si_vehicle_menu");

   %this.activateActionSet("Menu");
   $Steam::CurrentActionSet = "Menu";
}

/// addActionSet - Looks up the set handle from steam, saves the handle and a
/// tag for reference from script when activating or adding actions.
/// %setTag - The tag string that will be used to identify the set in script calls.
/// %setName - The name exactly as it appears in the In Game Actions (IGA) file. 
function steamworks::addActionSet(%this, %setTag, %setName)
{
   %this.ActionSet[%setTag] = SteamInput::addActionSet(%setName);
   %this.analogActions[%setTag] = 0;
   %this.digitalActions[%setTag] = 0;

   %this.setTag[%this.actionSets] = %setTag;
   %this.actionSets++;

   if (%this.ActionSet[%setTag] == -1)
      warn("Action Set: " @ %setName @ " Not found!");
}

/// addActionSetLayer - Looks up the set handle from steam, saves the handle and a
/// tag for reference from script when activating or adding actions.
/// %layerTag - The tag string that will be used to identify the set in script calls.
/// %layerName - The name exactly as it appears in the In Game Actions (IGA) file. 
function steamworks::addActionSetLayer(%this, %layerTag, %layerName)
{
   %this.ActionSetLayer[%layerTag] = SteamInput::addActionSetLayer(%layerName);
   %this.analogActions[%layerTag] = 0;
   %this.digitalActions[%layerTag] = 0;

   %this.layerTag[%this.setLayers] = %layerTag;
   %this.setLayers++;

   if (%this.ActionSetLayer[%layerTag] == -1)
      warn("Action Set Layer: " @ %layerName @ " Not found!");
}

/// addAnalogAction - Looks up the analog action and attaches the callback
/// %setTag - The action set that the action is listed with
/// %actionName - The name that appears in the IGA file for this action.
/// %callback - The script function that will be called when this action is
/// active and has input. Analog callbacks get the following parameters:
/// %controllerIndex, %xAxis, %yAxis.
function steamworks::addAnalogAction(%this, %setTag, %actionName, %callback)
{
   %this.analogAction[%setTag, %actionName] =
         SteamInput::addAnalogAction(%actionName, %callback);

   %this.analogTag[%setTag, %this.analogActions[%setTag]] = %actionName;
   %this.analogActions[%setTag]++;

   if (%this.analogAction[%setTag, %actionName] == -1)
      warn("Analog Action: " @ %actionName @ " Not found!");
   if (!isFunction(%callback))
      warn("Callback function: " @ %callback @ " For " @ %setTag SPC %actionName @ " Not found!");
}

/// addDigitalAction - Looks up the digital action and attaches the callback
/// %setTag - The action set that the action is listed with
/// %actionName - The name that appears in the IGA file for this action.
/// %callback - The script function that will be called when this action is
/// active and has input. Digital callbacks get the following parameters:
/// %controllerIndex, %state bool (pressed/unpressed).
function steamworks::addDigitalAction(%this, %setTag, %actionName, %callback)
{
   %this.digitalAction[%setTag, %actionName] =
         SteamInput::addDigitalAction(%actionName, %callback);

   %this.digitalTag[%setTag, %this.digitalActions[%setTag]] = %actionName;
   %this.digitalActions[%setTag]++;

   if (%this.digitalAction[%setTag, %actionName] == -1)
      warn("Digital Action: " @ %actionName @ " Not found!");
   if (!isFunction(%callback))
      warn("Callback function: " @ %callback @ " For " @ %setTag SPC %actionName @ " Not found!");
}

/// activateActionSet - Activates the action set on the controller.
/// %setTag - The action set tag that was passed to addActionSet.
/// %controller - The controller to activate the input on. Pass an empty string
/// or -1 to have the set activated on all controllers.
function steamworks::activateActionSet(%this, %setTag, %controller)
{
   if (%controller $= "")
      %controller = -1; // All controllers
   clearAllMoves();
   SteamInput::activateActionSet(%controller, %this.ActionSet[%setTag]);
}

/// getBindsForAction - returns a list of localized binding descriptions for
/// a single bound action.
/// %type - 'Analog' or 'Digital' action.
/// %controller - The controller to get binding for.
/// %setTag - Tag name passed to addActionSet when the set was added.
/// %actionName - The action name.
function steamworks::getBindsForAction(%this, %type, %controller, %setTag, %actionName)
{
   %setId = %this.ActionSet[%setTag];
   if (%type $= "analog")
   {
      %actionId = %this.analogAction[%setTag, %actionName];
      %origins = SteamInput::getAnalogActionOrigins(%i, %setId, %actionId);
   }
   else
   {
      %actionId = %this.digitalAction[%setTag, %actionName];
      %origins = SteamInput::getDigitalActionOrigins(%i, %setId, %actionId);
   }

   %bindings = %this.getBindsFromOrigins(%origins);
   return %bindings;
}

/// getGlyphsForAction - Returns a list of glyph file paths for a single bound action.
/// %type - 'Analog' or 'Digital' action.
/// %controller - The controller to get binding for.
/// %setTag - Tag name passed to addActionSet when the set was added.
/// %actionName - The action name.
function steamworks::getGlyphsForAction(%this, %type, %controller, %setTag, %actionName)
{
   %setId = %this.ActionSet[%setTag];
   if (%type $= "analog")
   {
      %actionId = %this.analogAction[%setTag, %actionName];
      %origins = SteamInput::getAnalogActionOrigins(%i, %setId, %actionId);
   }
   else
   {
      %actionId = %this.digitalAction[%setTag, %actionName];
      %origins = SteamInput::getDigitalActionOrigins(%i, %setId, %actionId);
   }

   %glyphs = %this.getGlyphsFromOrigins(%origins);
   return %glyphs;
}

/// getBindsFromOrigins - returns a list of localized binding descriptions from
/// the passed origins string.
/// %origins - The origins list returned from getAnalogActionOrigins or
/// getDigitalActionOrigins.
function steamworks::getBindsFromOrigins(%this, %origins)
{
   %numOrigins = getFieldCount(%origins);
   %bindings = "";
   for (%i = 0; %i < %numOrigins; %i++)
   {
      %idVal = getField(%origins, %i);
      if (%bindings !$= "")
         %bindings = %bindings @ ", ";
      %bindings = %bindings @ SteamInput::getStringForActionOrigin(%idVal);
   }

   return %bindings;
}

/// getGlyphsFromOrigins - returns a list of glyph file paths from the passed
/// origins string.
/// %origins - The origins list returned from getAnalogActionOrigins or
/// getDigitalActionOrigins.
function steamworks::getGlyphsFromOrigins(%this, %origins)
{
   %numOrigins = getFieldCount(%origins);
   %glyphs = "";
   for (%i = 0; %i < %numOrigins; %i++)
   {
      %idVal = getField(%origins, %i);
      if (%glyphs !$= "")
         %glyphs = %glyphs @ "\t";
      %glyphs = %glyphs @ SteamInput::getGlyphForActionOrigin(%idVal);
   }

   return %glyphs;
}

// Callback that fires when steam initially detects the controllers and again
// any time a controller is connected or disconnected.
function onSteamControllerChange(%numControllers)
{
   if (%numControllers > 0)
      steamworks.activateActionSet($Steam::CurrentActionSet);

   echo("onSteamControllerChange(): " @ %numControllers @ " connected controller(s).");
   for (%i = 0; %i < %numControllers; %i++)
   {
      %inputType = SteamInput::getInputTypeForDevice(%i);
      %inputStr = SteamInput::getInputStringForType(%inputType);
      %deviceHandle = SteamInput::getControllerHandle(%i);
      echo("   " @ %i+1 @ ", Handle: " @ %deviceHandle @ ", Type: " @ %inputType @ " (" @ %inputStr @ ")");
   }
}

function clearAllMoves()
{
   $mvForwardAction = 0;
   $mvBackwardAction = 0;
   $mvYawLeftSpeed = 0;
   $mvYawRightSpeed = 0;
   $mvPitchDownSpeed = 0;
   $mvPitchUpSpeed = 0;
}
