
function WebBrowserCtrl::onLoadingStateChange(%this, %isLoading, %canGoBack, %canGoForward)
{
   //echo("WebBrowserCtrl::onLoadingStateChange() - called by " @ %this.internalName);
   //echo("   %isLoading = " @ %isLoading @ ", %canGoBack = " @ %canGoBack @ ", %canGoForward = " @ %canGoForward);
   WebBrowserGui-->BackButton.setActive(%canGoBack);
   WebBrowserGui-->ForwardButton.setActive(%canGoForward);
   WebBrowserGui-->StopButton.setActive(%isLoading);
   
   if (!%isLoading)
   {
      %url = WebBrowserCtrl.getURL();
      BrowserAddress.setText(%url);
   }
}

function WebBrowserCtrl::onLoadError(%this, %errorCode, %errorText, %errorURL)
{
   WebBrowserGui-->StopButton.setActive(false);

   %text = "WebBrowserCtrl::onLoadError() - called by " @ %this.internalName;
   %text = %text @ "\n   errorCode: " @ %errorCode @ ", message: " @ %errorText;
   %text = %text @ "\n   URL: " @ %errorURL;
   echo(%text);
   MessageBoxOK("URL error", %text);
}

function BrowserAddress::onReturn(%this)
{
   %address = BrowserAddress.getText();
   WebBrowserCtrl.loadURL(%address);
}
