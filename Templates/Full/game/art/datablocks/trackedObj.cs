//----------------------------------------------------------------------------
// Tracked hand and hmd objects
//----------------------------------------------------------------------------

datablock OpenVRTrackedObjectData(VR_HMDData)
{
   shape = "data/ovrModule/art/vrModels/generic_hmd.dae";
   renderOffset = "0 0 0 0 0 1 3.141593";
   disconnectedOffset = "0 0 0 1 0 0 0";
};

// The next two datablocks are for use as OpenVRTrackedObject
datablock OpenVRTrackedObjectData(LeftTrackedObjectData)
{
   shape = "data/ovrModule/art/vrModels/vr_glove_left.dae";

   // renderOffset comes from the wrist node in the base skeleton pose.
   // Use renderOffset to adjust the position of the shape relative to the
   // vr tracked hand pose.
   renderOffset = "-0.041186 -0.150276 0.031897 -0.095531 0.379793 0.920126 3.018123";

   // Where to place the object if there's no tracked device connected.
   // Transform is relative to the tracking zero pose (hmd zero pose).
   disconnectedOffset = "-0.2 0.6 -.4 0 0 1 3.141593";
};

datablock OpenVRTrackedObjectData(RightTrackedObjectData)
{
   shape = "data/ovrModule/art/vrModels/vr_glove_right.dae";
   renderOffset = "0.041186 -0.150276 0.031897  -0.095531 0.379793 0.920126 3.018123";
   disconnectedOffset = "0.2 0.6 -.4 0 0 1 3.141593";
};

// The next two datablocks are for use as OpenVRTrackedHand The only difference
// from above is no render offset. All position and rotation data comes from the
// hand skeleton nodes.
datablock OpenVRTrackedObjectData(LeftHandData)
{
   shape = "data/ovrModule/art/vrModels/vr_glove_left.dae";
   disconnectedOffset = "-0.2 0.6 -.4 0 0 1 3.141593";
};

datablock OpenVRTrackedObjectData(RightHandData)
{
   shape = "data/ovrModule/art/vrModels/vr_glove_right.dae";
   disconnectedOffset = "0.2 0.6 -.4 0 0 1 3.141593";
};
