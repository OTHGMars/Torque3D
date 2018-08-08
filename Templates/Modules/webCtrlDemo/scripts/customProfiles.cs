//------------------------------------------------------------------------------
if (!isObject(GuiWebCtrlProfile))
   singleton GuiControlProfile( GuiWebCtrlProfile )
   {
      tab = true;
      canKeyFocus = true;
      modal = true;
      category = "Core";
   };
new GuiControlProfile (GuiWebDlgBackgroundProfile)
{
   opaque = true;
   fillColor = "20 20 20 225";
};