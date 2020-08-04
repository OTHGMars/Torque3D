# CEF T3D Plugin Installation Instructions

Download and extract the CEF 'Standard Distribution' for your target platform from: (http://opensource.spotify.com/cefbuilds/index.html).

Clone [Torque3D](https://github.com/GarageGames/Torque3D).

Pull https://github.com/OTHGMars/Torque3D/tree/CEFPlugin into your local branch.

## Windows:
1. Build the cef wrapper binary. (This step is only needed to build the dll wrapper. All other required binaries are provided in the distribution).
   * Use cmake to open CMakeLists.txt in the folder you extracted to. Set output to the same directory.
   * Configure and generate the cef project for the same VS version that you will be using to build T3D.
   * Open cef.sln in VS and create Release and Debug builds of the libcef_dll_wrapper project.

2. Build Torque3D.
   * Open your t3d cmake project. Set TORQUE_CEF_WEB on. Set TORQUE_CEF_PATH to the directory where you extracted the binary distribution. Set TorqueTemplate to 'BaseGame'.
   * Configure and generate your project.
   * Build the solution in VS. Be sure to also build the INSTALL project, as it will copy the binaries and resources needed by cef into your game directory.

3. Start using html guis. The install scripts copied the webCtrlDemo Module to your game/data directory. The demo provides gui based examples of interaction with the GuiWebCtrl and WebURLRequest objects.


## Linux
Coming Soon...
