
// Make sure the demo script is loaded
if (!isObject(ObjCanvasOne))
{
   exec("demo/guiDemo.cs");
   startGuiDemo();
}

singleton Material(CRScreenMat)
{
   mapTo = "CC_Screen1";
   diffuseMap[0] = "#bbcontrolscanvas";
   emissive[0] = "1";
};

singleton Material(CRGuiScreenMat)
{
   mapTo = "Gui_Screen1";
   diffuseMap[0] = "#objectcanvas";
   emissive[0] = "1";
   doubleSided[0] = "1";
   translucent = true;
};

singleton Material(CRGuiBillboardMat)
{
   mapTo = "BB_Screen1";
   diffuseMap[0] = "#objectcanvas_bb";
   emissive[0] = "1";
   doubleSided[0] = "1";
   translucent = true;
};

singleton Material(CRScreenFrame)
{
   mapTo = "MagProp_FlatBlack";
   diffuseColor[0] = "0.02 0.25 0.65 1";
   doubleSided[0] = "1";
};
