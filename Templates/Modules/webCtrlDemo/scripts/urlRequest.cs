//==============================================================================
// Default callback handlers for WebURLRequest
//==============================================================================

//------------------------------------------------------------------------------
function WebURLRequest::onRequestComplete(%this, %status, %errorCode)
{
   if (isObject(%this.callbackObject))
      return %this.callbackObject.onRequestComplete(%status, %errorCode);
   else
   {
      if (%status $= "Pending")
         return false;

      if (%status !$= "Success")
      {
         warn("WebURLRequest::onRequestComplete(" @ %status @ ", " @ %errorCode @ ")");

         %rspError = %this.getErrorCode();
         %statusCode = %this.getStatusId();
         %statusText = %this.getStatusText();
         warn("   Error Code " @ %rspError @ ", Status: " @ %statusCode @ " - " @ %statusText);

         %headers = %this.getResponseHeaders();
         warn(%headers @ "\n");
      }
   }

   return true;
}

//------------------------------------------------------------------------------
function WebURLRequest::onUploadProgress(%this, %current, %total)
{
   if (isObject(%this.callbackObject))
      %this.callbackObject.onUploadProgress(%current, %total);
   else
      echo("WebURLRequest::onUploadProgress(" @ %current @ ", " @ %total @ ")");
}

//------------------------------------------------------------------------------
function WebURLRequest::onDownloadProgress(%this, %current, %total)
{
   if (isObject(%this.callbackObject))
      %this.callbackObject.onDownloadProgress(%current, %total);
   else
      echo("WebURLRequest::onDownloadProgress(" @ %current @ ", " @ %total @ ")");
}

//------------------------------------------------------------------------------
function WebURLRequest::onDownloadData(%this, %data, %dataSize)
{
   if (isObject(%this.callbackObject))
      %this.callbackObject.onDownloadData(%data, %dataSize);
   else
      echo("WebURLRequest::onDownloadData(), Size: " @ %dataSize @ ", Data: " @ %data);
}

//------------------------------------------------------------------------------
function WebURLRequest::onLine(%this, %line)
{
   if (isObject(%this.callbackObject))
      %this.callbackObject.onLine(%line);
   else
      echo("WebURLRequest::onLine(): " @ %line);
}

