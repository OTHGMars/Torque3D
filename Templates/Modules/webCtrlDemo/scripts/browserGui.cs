
// ----------------------------------------------------------------------------
function GuiWebCtrl::onPopupRequested(%this, %url, %width, %height)
{
   echo("GuiWebCtrl::onPopupRequested(" @ %url @ ", " @ %width @ ", " @ %height @ ")");
   %popup = WebBrowserGui.deepClone();
   %popup-->DemoCEFBrowser.loadURL(%url);
   %popup.canSave = false;
   %popup.isPopup = true;

   if ((%width > 0) && (%height > 0))
   {
      %frame = %popup->browserFrame;
      %frame.canMove = true;
      %minExtents = %frame.minExtent;
      %width = (%width < %minExtents.x) ? %minExtents.x : %width;
      %height = (%height < %minExtents.y) ? %minExtents.y : %height;
      %frame.setExtent(%width, %height);
   }

   %webCanvas = %this.getRoot();
   %webCanvas.pushDialog(%popup);
   return true;
}

function BrowserDlg::onSleep(%this)
{
   if (%this.isPopup)   // Delete cloned guis after they are closed
      %this.schedule(32, "delete");
}

function BrowserSite::onWake(%this)
{
   %this.errorCount = 0;
}

function BrowserSite::onLoadingStateChange(%this, %isLoading, %canGoBack, %canGoForward)
{
   //echo("WebBrowserCtrl::onLoadingStateChange() - called by " @ %this.internalName);
   //echo("   %isLoading = " @ %isLoading @ ", %canGoBack = " @ %canGoBack @ ", %canGoForward = " @ %canGoForward);
   %this.getParent()->BackButton.setActive(%canGoBack);
   %this.getParent()->ForwardButton.setActive(%canGoForward);
   %this.getParent()->StopButton.setActive(%isLoading);
   
   if (!%isLoading)
   {
      %url = %this.getParent()->DemoCEFBrowser.getURL();
      %this.getParent()->AddressBox.setText(%url);
   }
}

function BrowserSite::onLoadError(%this, %errorCode, %errorText, %errorURL)
{
   %this.getParent()->StopButton.setActive(false);

   %text = "BrowserSite::onLoadError() - called by " @ %this.internalName;
   %text = %text @ "\n   errorCode: " @ %errorCode @ ", message: " @ %errorText;
   %text = %text @ "\n   URL: " @ %errorURL;
   warn(%text);

   if (%this.errorCount == 0)
      %this.schedule(32, "pageReload");
   else
      MessageBoxOK("URL error", %text);

   %this.errorCount++;
}

function BrowserAddress::onReturn(%this)
{
   %address = %this.getText();
   %this.getParent()->DemoCEFBrowser.loadURL(%address);
}
