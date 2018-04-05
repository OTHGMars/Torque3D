new GuiControlProfile (GuiHapticBackgroundProfile)
{
   justify = "left";
   fontSize = 18;
   fontColor = "225 225 225 255";

   opaque = true;
   fillColor = "20 20 20 240";
};

new GuiControlProfile (GuiHapticBorderProfile)
{
   opaque = false;
   border = 1;
   borderColor   = "225 225 225 255"; 
};

new GuiControlProfile (GuiHapticTextProfile)
{
   justify = "left";
   fontSize = 18;
   fontColor = "225 225 225 255";
};

new GuiControlProfile (GuiHapticCheckboxProfile : GuiCheckBoxProfile)
{
   fontSize = 18;
   fontColor = "225 225 225 255";
   fontColorHL ="141 204 249 255";
};
