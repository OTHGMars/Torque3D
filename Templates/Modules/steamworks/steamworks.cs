//-----------------------------------------------------------------------------
// Module creation functions.
//-----------------------------------------------------------------------------

function steamworks::create( %this )
{
   if (SteamAPI::isSteamRunning())
   {
      SteamAPI::setOverlayNotificationPosition("Top Left");
      SteamAPI::setOverlayNotificationInset(8, 8);

      exec("./scripts/achievements.cs");
      %this.init();

      if (isFunction("isSteamInputInitialized") && isSteamInputInitialized())
      {
         exec("./scripts/steamInput.cs");
         exec("./scripts/menuCallbacks.cs");
         exec("./scripts/playerCallbacks.cs");
         exec("./scripts/vehicleCallbacks.cs");
         %this.initInput();
      }
   }
}

function steamworks::destroy( %this )
{
   
}

function steamworks::init( %this )
{
   SteamAPI::requestCurrentStats();
}

// Everything below this point are just helpers for testing/learning the input system
function steamworks::listAllUserBinds(%this)
{
   %numControllers = SteamInput::getNumControllers();
   for (%i = 0; %i < %numControllers; %i++)
   {
      %inputType = SteamInput::getInputTypeForDevice(%i);
      %inputStr = SteamInput::getInputStringForType(%inputType);
      %deviceHandle = SteamInput::getControllerHandle(%i);
      echo("\nController " @ %i @ ", Handle: " @ %deviceHandle @ ", Type: " @ %inputType @ " (" @ %inputStr @ ")");

      for (%setIndex = 0; %setIndex < %this.actionSets; %setIndex++)
      {
         %setTag = %this.setTag[%setIndex];
         echo("   Action Set " @ %setTag);

         // Analog actions
         for (%actionIndex = 0; %actionIndex < %this.analogActions[%setTag]; %actionIndex++)
         {
            %actionTag = %this.analogTag[%setTag, %actionIndex];
            %actId = %this.analogAction[%setTag, %actionTag];
            %origins = SteamInput::getAnalogActionOrigins(%i, %setIndex, %actId);
            %bindings = steamworks.getBindsFromOrigins(%origins);
            %glyphs = steamworks.getGlyphsFromOrigins(%origins);
            %bindStr = (%bindings $= "") ? "None" : (%bindings @ "; " @ %glyphs);
            echo("      Analog Action: " @ %actionTag @ " - " @ %bindStr);
         }

         // Digital actions
         for (%actionIndex = 0; %actionIndex < %this.digitalActions[%setTag]; %actionIndex++)
         {
            %actionTag = %this.digitalTag[%setTag, %actionIndex];
            %actId = %this.digitalAction[%setTag, %actionTag];
            %origins = SteamInput::getDigitalActionOrigins(%i, %setIndex, %actId);
            %bindings = steamworks.getBindsFromOrigins(%origins);
            %glyphs = steamworks.getGlyphsFromOrigins(%origins);
            %bindStr = (%bindings $= "") ? "None" : (%bindings @ "; " @ %glyphs);
            echo("      Digital Action: " @ %actionTag @ " - " @ %bindStr);
         }
      }
   }
}

function listAllSteamGlyphs()
{
   for (%i = 1; %i < 196; %i++)
   {
      echo(%i @ ": " @ SteamInput::getStringForActionOrigin(%i) @ ", " @ SteamInput::getGlyphForActionOrigin(%i));
   }
}

//SteamInput::showBindingPanel(0);
//SteamInput::getCurrentActionSet(0);
//SteamInput::triggerVibration(0, 10000, 10000);
//SteamAPI::isOverlayEnabled();
//SteamAPI::isSteamInBigPictureMode();
//SteamAPI::isSteamRunningInVR();

//steamworks.listAllUserBinds();
//listAllSteamGlyphs();
//steamworks.getBindsForAction("Digital", 0, "Menu", "menu_lmb");
//steamworks.getGlyphsForAction("Digital", 0, "Menu", "menu_lmb");
//steamworks.getBindsForAction("Analog", 0, "Menu", "menu_mouse");
//steamworks.getGlyphsForAction("Analog", 0, "Menu", "menu_mouse");
//steamworks.getBindsForAction("Digital", 0, "Player", "player_fire");
//steamworks.getGlyphsForAction("Digital", 0, "Player", "player_fire");

//
//SteamAPI::isSteamRunning();
//SteamAPI::getUserId();
//SteamAPI::getPersonaName();
//SteamAPI::requestCurrentStats();
//SteamAPI::areStatsLoaded();
//SteamAPI::getNumAchievements();
//SteamAPI::getAchievementName(0);
//SteamAPI::getAchievement("ACH_TRAVEL_FAR_ACCUM");
//SteamAPI::setAchievement("ACH_TRAVEL_FAR_ACCUM");
//SteamAPI::setAchievement("ACH_TRAVEL_FAR_SINGLE");
//SteamAPI::clearAchievement("ACH_TRAVEL_FAR_ACCUM");
//SteamAPI::resetAllStats(1);
//SteamAPI::storeStats();
//SteamAPI::getAchievementDisplayAttribute("ACH_TRAVEL_FAR_ACCUM", "Name");
//SteamAPI::getAchievementDisplayAttribute("ACH_TRAVEL_FAR_ACCUM", "desc");
//SteamAPI::getAchievementDisplayAttribute("ACH_TRAVEL_FAR_ACCUM", "hidden");
//SteamAPI::setOverlayNotificationPosition("Top Right");
//SteamAPI::setOverlayNotificationInset(4, 4);
//SteamAPI::showGamepadTextInput("Normal", "Single", "Testing Description", 12, "Starter");
