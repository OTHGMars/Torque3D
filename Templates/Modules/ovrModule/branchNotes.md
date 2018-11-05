### Building T3D with OpenVR
1. Download and extract the [OpenVR 1.0.17 Release](https://github.com/ValveSoftware/openvr/releases)
2. Clone [Torque3D](https://github.com/GarageGames/Torque3D)
3. Pull this branch [OVR_1017f](https://github.com/OTHGMars/Torque3D/tree/OVR_1017f)
4. Configure cmake with your directories and app name.
5. Select `TORQUE_EXTENDED_MOVE` and `TORQUE_OPENVR`
6. Set `TORQUE_OPENVR_SDK_PATH` to the directory where you extracted the openvr sdk.
7. Set the template to 'Full' to install the OpenVR demo scripts.
8. Configure, generate then build the project.

### Installing the runtime
You'll need the latest SteamVR beta runtime installed, but you do not need to be logged into steam when using VR. To install, go to your library/tools folder in Steam, right-click SteamVR, select Properties, click the Beta tab, select SteamVR Beta Update and click close.

### Running the demo scripts
Install the Full template.
Copy the ovrModule directory from Templates/Modules to your game/data directory (you'll need to create the data directory).
Launch the game and click the HMD checkbox on the Choose Level or Join Server screens.

**Empty Terrain** – VR Demo – Use VR controllers to move the player or drive the Cheetah. This is the full template demo updated for VR.

**Empty Room** – Tracked Object Demo – The player is replaced with tracked objects for the HMD and hands (see below for render model requirements). You can also see the animated hand skeleton and controller model when running this level.

### Render models for the tracked object demo
*You can probably skip this step by requesting a zip with the three .dae files in the T3D discord server.* All controller models are loaded through the `IVRRenderModels` interface. The vr glove models are fbx format and must be loaded by the application. These models need converted to collada. We'll also convert the generic hmd model so it can be rendered as a tracked object on clients that are not using an hmd and do not have the vr runtime loaded.

**VR Hands**
1. Open: Steam/steamapps/common/SteamVR/resources/rendermodels /vr_glove/vr_glove_left_model.fbx in Visual Studio.
2. Select the entire model by drawing a box around it.
3. Open the properties window (wrench icon).
4. Expand Phong:models/hands/vr_glove.vmat.
5. Change the Name field to 'vr_glove_mat'.
6. Re-select the entire model.
7. Save in Collada format as: game/data/ovrModule/art/vrModels/vr_glove_left.dae.
8. Repeat 1-7 for the right hand model to create vr_glove_right.dae.

**VR HMD**
1. Open: Steam/steamapps/common/SteamVR/resources/rendermodels/generic_hmd/generic_hmd.obj in Visual Studio.
2. Select the entire model.
3. Open properties and change the scale to 100 100 100.
4. Click View Front to rescale the window.
5. Re-select the entire model.
6. Save in Collada format as:  game/data/ovrModule/art/vrModels/generic_hmd.dae

#### Work Remaining, Issues and other Assorted Ramblings
**Hand Skeletons** – Currently OpenVR needs to be initialized and the runtime needs to be loaded to decompress skeletal bone data in the move packets. The hand skeletons were intentionally made controller agnostic to prevent issues like this, so it is expected to change in future versions of OpenVR. For now, `animateOnServer` should be set false for `OpenVRTrackedHand` objects unless it is known that the server will be a listen-server with an active HMD connected. VR initialization will fail on a dedicated server so decompression will not be available.
During testing, the compressed skeleton data ranged between 41 and 61 Bytes per packet with an average of 49 Bytes (over 200k samples). The `ExtendedMove` class will network this as a variable size binary blob. The bandwidth cost on the local client for networked hand skeletons averages (~49 Bytes * 2 hands * packetrate to server) + (~49 bytes *  2Hands * packetrate to client * number of remote clients). Hand poses and skeleton information can be turned on and off by activating/deactivating the action set that they are a part of. I.E. in the Empty Terrain demo level, the hands are not shown and the Demo action set  is not active, so no position/rotation or skeleton data is networked for either hand.

**OpenGL** – The code works when using opengl, but is very jittery. This is a known issue with a [work-around](https://github.com/ValveSoftware/openvr/blob/v1.0.17/samples/hellovr_opengl/hellovr_opengl_main.cpp#L864-L868). The work-around is for glew, so the equivalent glad fix will need to be found, implemented and tested.

**Linux** – The cmake file has been updated so this branch builds on Linux, but it is not setup properly for [building](https://github.com/ValveSoftware/steam-runtime#building-in-the-runtime) and [running](https://github.com/ValveSoftware/SteamVR-for-Linux/blob/master/README.md#runtime-requirements) within the steam runtime. So, for now (as before) only windows is supported.

**Demo Weapons** - The weapons in the full template are all `firstPerson` with `correctMuzzleVector` set true. Weapons with a true muzzle vector that fire from the muzzle point would be a much better fit for VR. No attempt has been made to update the demo weapons.

**Action Manifests** - Writing an [Action Manifest](https://github.com/ValveSoftware/openvr/wiki/Action-manifest) for `IVRInput` is tedious and error prone (*not that scripting T3D `ActionMap` entries is exactly fun*). They are also very similar, but not identical to Steam Input [In-Game Actions Files](https://partner.steamgames.com/doc/features/steam_controller/iga_file). The problem is compounded during development if you add/remove or change your game actions, you will need to duplicate the change for each input module. T3D would benefit greatly from a unified gui based input tool that managed action sets and actions, automatically updated the `ActionMap` script and generated manifests for all installed input modules, created function stubs with correct parameters for new action entries and managed game default binds. It would also need a corresponding script module to handle activation/deactivation of sets and replace script like [this](https://github.com/OTHGMars/Torque3D/blob/OVR_1017f/Templates/Full/game/scripts/client/client.cs#L158-L160) with a single call that would activate the actions in all active input modules. This would also simplify creating a more full featured mapping gui for keyboard/mouse/sdl input if there was a structured way to represent the actions with their type and callback as sets independent of the `ActionMap` itself.