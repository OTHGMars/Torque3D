
function WebURLGui::onWake(%this)
{
   if (%this.requestType $= "")
   {
      %this-->GET.performClick();
      %this.responseHeader = "";
      %this.response = "";
      %this.status = "";
   }
}

function WebURLGui::onSleep(%this)
{
   return;
}

function WebURLGui::onSaveAs(%this)
{
   %fileName = %this-->FileNameBox.getText();
   %filePath = collapseFilename(filePath(%fileName));
   %dlg = new SaveFileDialog()
   {
      Filters        = "All Files (*.*)|*.*|";
      DefaultPath    = %filePath;
      ChangePath     = false;
      OverwritePrompt   = true;
   };

   %ret = %dlg.Execute();
   if(%ret)
   {
      %fileName = %dlg.FileName;
      %fullPath = makeFullPath(%fileName);
      %this-->FileNameBox.setText(%fullPath);
   }
   
   %dlg.delete();
}

function WebURLGui::sendRequest(%this)
{
   %url = %this-->URLBox.getText();
   if (%url $= "")
   {
      MessageBoxOK("Error", "Request requires a URL!");
      return;
   }

   %isFileDownload = %this-->FileReqCheck.getValue();
   %filePath = %this-->FileNameBox.getText();
   if (%isFileDownload && (%filePath $= ""))
   {
      MessageBoxOK("Error", "File requests require a file path!");
      return;
   }

   %postData = "";
   if (%this.requestType $= "POST")
      %postData = %this-->PostDataBox.getText();

   %headers = "";
   %headerCount = %this-->HeaderStack.getCount();
   for (%i = 0; %i < %headerCount; %i++)
   {
      %headerCtrl = %this-->HeaderStack.getObject(%i);
      %headerText = %headerCtrl->HeaderBox.getText();
      %valueText = %headerCtrl->ValueBox.getText();
      if (%headerText !$= "")
         %headers = %headers @ %headerText @ ":" @ %valueText @ "\t";
   }

   if (!isObject(DemoURLObject))
   {
      %this.urlObject = new WebURLRequest(DemoURLObject)
      {
         // Redirect callbacks directly to this gui object
         callbackObject = %this;
      };
   }
   DemoURLObject.setLineCallbacks(true); // Receive data every line instead of every chunk.

   if (%isFileDownload)
      DemoURLObject.makeFileRequest(%url, %this.requestType, %postData, %headers, %filePath);
   else
      DemoURLObject.makeRequest(%url, %this.requestType, %postData, %headers);

   %this-->StatusText.setText("Sending");
   %this-->DLProgress.setValue(0);
   %this-->DisplayText.setText("");
   %this.responseHeader = "";
   %this.response = "";
   %this.status = "Sending";
   return;
}

function WebURLGui::onFileCheck(%this)
{
   %isFileDownload = %this-->FileReqCheck.getValue();
   %this-->FileNameBox.setActive(%isFileDownload);
   %this-->FileNameBtn.setActive(%isFileDownload);
}

function WebURLGui::onTypeButton(%this, %radioCtrl)
{
   %this.requestType = strupr(%radioCtrl.internalName);
   %this-->PostDataBox.setActive(%this.requestType $= "POST");
}

function WebURLGui::onAddHeader(%this)
{
   %header = HeaderLineTemplate.deepClone();
   %header.canSave = false;
   %header.visible = true;
   %this-->HeaderStack.add(%header);
}

function WebURLGui::removeHeader(%this, %headerBtn)
{
   %header = %headerBtn.getParent();
   %header.delete();
   %this-->HeaderStack.updateStack();
}

function WebURLGui::onHeaderCheck(%this)
{
   %showResponseheader = %this-->ShowHeaderCheck.getValue();
   if (%showResponseheader)
      %outPut = %this.responseHeader NL %this.response;
   else
      %outPut = %this.response;

   %this-->DisplayText.setText(%outPut);
}

//------------------------------------------------------------------------------
function WebURLGui::onRequestComplete(%this, %status, %errorCode)
{
   %this.status = %status;
   if (%status !$= "Success")
   {
      %rspError = %this.urlObject.getErrorCode();
      %statusCode = %this.urlObject.getStatusId();
      %statusText = %this.urlObject.getStatusText();
      %displayStatus = %status @ ", " @ %statusCode @ " - " @ %statusText;
      warn("   Error Code " @ %rspError @ ", Status: " @ %status @ ", " @ %statusCode @ " - " @ %statusText);
   }
   else
      %displayStatus = %status @ " - " @ %errorCode;

   %this-->StatusText.setText(%displayStatus);

   %this.responseHeader = %this.urlObject.getResponseHeaders();
   %showResponseheader = %this-->ShowHeaderCheck.getValue();
   if (%showResponseheader)
   {
      %outPut = %this.responseHeader NL %this.response;
      %this-->DisplayText.setText(%outPut);
   }

   if (%status $= "Pending")
      return false;

   return true;
}

//------------------------------------------------------------------------------
function WebURLGui::onUploadProgress(%this, %current, %total)
{
   %this.status = "Uploading";
   if (%total > 0)
   {
      %progress = %current / %total;
      %this-->DLProgress.setValue(%progress);
      %this-->StatusText.setText(%this.status);
   }
   else
      %this-->StatusText.setText(%this.status SPC %current @ "/?");
}

//------------------------------------------------------------------------------
function WebURLGui::onDownloadProgress(%this, %current, %total)
{
   %this.status = "Downloading";
   if (%total > 0)
   {
      %progress = %current / %total;
      %this-->DLProgress.setValue(%progress);
      %this-->StatusText.setText(%this.status);
   }
   else
      %this-->StatusText.setText(%this.status SPC %current @ "/?");
}

//------------------------------------------------------------------------------
function WebURLGui::onDownloadData(%this, %data, %dataSize)
{
   %this.response = %this.response @ %data;
   %this-->DisplayText.setText(%this.response);
}

//------------------------------------------------------------------------------
function WebURLGui::onLine(%this, %line)
{
   if (%this.response $= "")
      %this.response = %line;
   else
      %this.response = %this.response NL %line;
   %this-->DisplayText.setText(%this.response);
}

//exec("data/webCtrlDemo/scripts/urlTest.cs");