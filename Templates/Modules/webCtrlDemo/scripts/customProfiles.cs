//------------------------------------------------------------------------------
if (!isObject(GuiWebCtrlProfile))
   singleton GuiControlProfile( GuiWebCtrlProfile )
   {
      tab = true;
      canKeyFocus = true;
      modal = true;
      category = "Core";
   };
if (!isObject(GuiWebDlgBackgroundProfile))
   singleton GuiControlProfile (GuiWebDlgBackgroundProfile)
   {
      opaque = true;
      fillColor = "20 20 20 225";
   };

if( !isObject( GuiTextCenterProfile ) )
   singleton GuiControlProfile (GuiTextCenterProfile : GuiTextProfile)
   {
      justify = "center";
      category = "Core";
   };