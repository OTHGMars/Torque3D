//-----------------------------------------------------------------------------
// Load the IVRInput Action Sets
// This file is the game specific action map setup for openvr.
// The equivalent of default.binds.cs
//-----------------------------------------------------------------------------

function ovrModule::initActionSets( %this )
{  // All of the data here must match what is set in your action manifest .json
   // file. These commands cause the action and action set handles to be loaded.

   %this.actionSets = 0;
   %this.setLayers = 0;
   %this.addActionSet("Demo", "/actions/demo");
   %this.addActionSet("Menu", "/actions/menu");
   %this.addActionSet("Player", "/actions/player");
   %this.addActionSet("Vehicle", "/actions/vehicle");

   // Load the Demo actions
   %this.addDigitalAction("Demo", "toggle_left", "in_demo_toggle_left");
   %this.addDigitalAction("Demo", "toggle_right", "in_demo_toggle_right");
   %this.addDigitalAction("Demo", "laser_left", "in_demo_laser_left");
   %this.addDigitalAction("Demo", "laser_right", "in_demo_laser_right");
   %this.addDigitalAction("Demo", "demo_menu", "in_demo_menu");

   // No callbacks for the pose actions. They go directly to ExtendedMoveManager
   // at the index assigned here.
   %this.addPoseAction("Demo", "hand_right", "", "", 2);
   %this.addPoseAction("Demo", "hand_left", "", "", 1);

   // Attach the compresed hand skeleton data to the move manager when the Demo
   // set is active.
   %this.addSkeletalAction("Demo", "rh_anim", 2);
   %this.addSkeletalAction("Demo", "lh_anim", 1);

   // Both hands start in "with controller" pose
   %this.setSkeletonMode("Demo", "rh_anim", true);
   %this.setSkeletonMode("Demo", "lh_anim", true);

   // Load the menu actions
   %this.addAnalogAction("Menu", "menu_mouse", "in_menu_mouse");
   %this.addDigitalAction("Menu", "menu_lmb", "in_menu_lmb");
   %this.addDigitalAction("Menu", "menu_rmb", "in_menu_rmb");
   %this.addDigitalAction("Menu", "menu_up", "in_menu_up");
   %this.addDigitalAction("Menu", "menu_down", "in_menu_down");
   %this.addDigitalAction("Menu", "menu_left", "in_menu_left");
   %this.addDigitalAction("Menu", "menu_right", "in_menu_right");
   %this.addDigitalAction("Menu", "menu_next", "in_menu_next");
   %this.addDigitalAction("Menu", "menu_last", "in_menu_last");
   %this.addDigitalAction("Menu", "menu_back", "in_menu_back");
   %this.addDigitalAction("Menu", "menu_select", "in_menu_select");
   %this.addDigitalAction("Menu", "menu_submit", "in_menu_submit");
   %this.addDigitalAction("Menu", "menu_cancel", "in_menu_cancel");
   %this.addDigitalAction("Menu", "menu_menu", "in_pause_menu");
   %this.addDigitalAction("Menu", "menu_closer", "in_overlay_closer");
   %this.addDigitalAction("Menu", "menu_farther", "in_overlay_farther");

   // Load the player actions
   %this.addAnalogAction("Player", "player_move_joy", "in_player_move_joy");
   %this.addAnalogAction("Player", "player_move_touch", "in_player_move_touch");
   %this.addAnalogAction("Player", "player_rot_joy", "in_player_rot_joy");
   %this.addAnalogAction("Player", "player_rot_touch", "in_player_rot_touch");
   %this.addDigitalAction("Player", "player_move_press", "in_player_move_press");
   %this.addDigitalAction("Player", "player_rot_press", "in_player_rot_press");
   %this.addDigitalAction("Player", "player_fire", "in_any_fire");
   %this.addDigitalAction("Player", "player_altfire", "in_any_altfire");
   %this.addDigitalAction("Player", "player_jump", "in_player_jump");
   %this.addDigitalAction("Player", "player_crouch", "in_player_crouch");
   %this.addDigitalAction("Player", "player_sprint", "in_player_sprint");
   %this.addDigitalAction("Player", "player_weapup", "in_player_weapup");
   %this.addDigitalAction("Player", "player_weapdown", "in_player_weapdown");
   %this.addDigitalAction("Player", "player_weapreload", "in_player_weapreload");
   %this.addDigitalAction("Player", "player_tossweapon", "in_player_tossweapon");
   %this.addDigitalAction("Player", "player_tossammo", "in_player_tossammo");
   %this.addDigitalAction("Player", "player_carjack", "in_player_carjack");
   //%this.addDigitalAction("Player", "player_camtoggle", "in_any_camtoggle");
   //%this.addDigitalAction("Player", "player_freelook", "in_any_freelook");
   //%this.addDigitalAction("Player", "player_zoom", "in_player_zoom");
   %this.addDigitalAction("Player", "player_interact", "in_player_interact");
   %this.addDigitalAction("Player", "player_menu", "in_player_menu");

   // Load the vehicle actions
   %this.addAnalogAction("Vehicle", "vehicle_steer", "in_vehicle_steer");
   %this.addAnalogAction("Vehicle", "vehicle_throttle", "in_vehicle_throttle");
   %this.addAnalogAction("Vehicle", "vehicle_reverse", "in_vehicle_reverse");
   %this.addDigitalAction("Vehicle", "vehicle_fire", "in_any_fire");
   %this.addDigitalAction("Vehicle", "vehicle_altfire", "in_any_altfire");
   %this.addDigitalAction("Vehicle", "vehicle_brake", "in_vehicle_brake");
   %this.addDigitalAction("Vehicle", "vehicle_brakelight", "in_vehicle_brakelight");
   %this.addDigitalAction("Vehicle", "vehicle_getout", "in_vehicle_getout");
   %this.addDigitalAction("Vehicle", "vehicle_flipcar", "in_vehicle_flipcar");
   //%this.addDigitalAction("Vehicle", "vehicle_camtoggle", "in_any_camtoggle");
   //%this.addDigitalAction("Vehicle", "vehicle_freelook", "in_any_freelook");
   %this.addDigitalAction("Vehicle", "vehicle_menu", "in_vehicle_menu");

   // Load the haptic events
   %this.addHapticEvent("Player", "player_recoil");
   %this.addHapticEvent("Vehicle", "vehicle_collision");

   %this.activateActionSet("Menu");
}
