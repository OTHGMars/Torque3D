//-----------------------------------------------------------------------------

singleton Material(LaserPointerMat)
{
   mapTo = "laserpointer";
   diffuseMap[0] = "./laserpointer.png";
   specularPower[0] = "1";
   useAnisotropic[0] = "1";
   scrollDir[0] = "0 -0.31";
   scrollSpeed[0] = "4.235";
   translucent = "1";
   translucentBlendOp = "Add";
   showFootprints = "0";
   animFlags[0] = "0x00000001";
   glow[0] = "0";
   emissive[0] = "1";
   specularPower0 = "1";
   doubleSided = 1;
   //diffuseColor[0] = "0.374 0.201 0.001 1";
};

singleton Material(LaserHoverMat)
{
   mapTo = "unmapped_mat";
   diffuseMap[0] = "./laserhovered.png";
   specularPower[0] = "1";
   useAnisotropic[0] = "1";
   scrollDir[0] = "0.1 0.2";
   scrollSpeed[0] = "10";
   translucent = "1";
   translucentBlendOp = "Add";
   showFootprints = "0";
   animFlags[0] = "0x00000001";
   glow[0] = "0";
   emissive[0] = "1";
   specularPower0 = "1";
   doubleSided = 1;
   //diffuseColor[0] = "0.74 0.4 0.2 1";
};

singleton Material(vr_controller_vive_1_51_Mat)
{
   mapTo = "vr_controller_vive_1_51";
   diffuseMap[0] = "art/gui/Torque-3D-logo-w.png";
   translucent = "1";
   //emissive[0] = "1";
};

singleton Material(knuckles_left_logo_Mat)
{
   mapTo = "{knuckles}valve_controller_knu_1_0_left3";
   diffuseMap[0] = "art/gui/Torque-3D-logo-w.png";
   translucent = "1";
   //emissive[0] = "1";
};

singleton Material(knuckles_right_logo_Mat)
{
   mapTo = "{knuckles}valve_controller_knu_1_0_right1";
   diffuseMap[0] = "art/gui/Torque-3D-logo-w.png";
   translucent = "1";
   //emissive[0] = "1";
};
