
singleton Material(vr_glove_model_vmat)
{
   mapTo = "vr_glove_mat";
   diffuseColor[0] = "0 0.4 0.5 0.55";
   specular[0] = "0.1 0.1 0.1 1";
   specularPower[0] = "5";
   translucentBlendOp = "Sub";
   translucent = "1";
   translucentZWrite = "1";
   castDynamicShadows = true;
};

singleton Material(generic_hmd_lambert2SG_mat)
{
   mapTo = "generic_hmd_lambert2SG";
   diffuseMap[0] = "./generic_hmd.png";
   specular[0] = "0.1 0.1 0.1 1";
   specularPower[0] = "5";
   translucentBlendOp = "Sub";
   translucent = "1";
   translucentZWrite = "1";
   castDynamicShadows = true;
};
