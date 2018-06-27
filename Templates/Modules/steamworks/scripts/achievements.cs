//-----------------------------------------------------------------------------
// Achievement/Stats functions and callbacks.
//-----------------------------------------------------------------------------

function onSteamUserStatsReceived(%success)
{
   %text = "The user stats load " @ (%success ? "Succeeded" : "Failed");
   echo(%text);
   if (%success)
      listSteamAchievements();
}

function onSteamGameOverlayActivated(%isActive)
{
   %text = "The steam game overlay is " @ (%isActive ? "Active" : "Inactive");
   echo(%text);
}

function onSteamAchievementStored(%unlocked, %name, %curProgress, %totalProgress)
{
   echo("onSteamAchievementStored(" @ %unlocked @ ", " @ %name @ ", " @ %curProgress @ ", " @ %totalProgress @ ") - Called!");
}

function onSteamUserStatsStored(%success)
{
   if (%success)
      echo("The user stats have been stored!");
   else
      echo("The user stats FAILED to store!");
}

function listSteamAchievements()
{
   if (!SteamAPI::isSteamRunning())
   {
      echo("Steam is not running");
      return;
   }

   echo("Steam User: " @ SteamAPI::getPersonaName() @ ", Steam ID: " @
      SteamAPI::getUserId());

   if (!SteamAPI::areStatsLoaded())
   {
      echo("No Stats Loaded Yet!");
      return;
   }

   %numAchievements = SteamAPI::getNumAchievements();
   for (%i = 0; %i < %numAchievements; %i++)
   {
      %apiName = SteamAPI::getAchievementName(%i);
      %achieved = SteamAPI::getAchievement(%apiName);
      %dispname = SteamAPI::getAchievementDisplayAttribute(%apiName, "name");
      %dispdesc = SteamAPI::getAchievementDisplayAttribute(%apiName, "desc");
      %hidden = SteamAPI::getAchievementDisplayAttribute(%apiName, "hidden");
      %achievedText = %achieved ? ", Unlocked" : ", Locked";
      %hiddenText = %hidden ? ", Hidden" : "";
      echo(%i SPC %apiName @ ": " @ %dispname @ ", " @ %dispdesc @ %hiddenText @ %achievedText);
   }
}
