//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "webEngine.h"
#include "webURLRequest.h"
#include "webRequestClient.h"
#include "core/strings/stringUnit.h"

#include "include/cef_base.h"
#include "include/cef_urlrequest.h"

//  CefURLRequest
IMPLEMENT_CONOBJECT(WebURLRequest);

ConsoleDocClass(WebURLRequest,
   "@brief Script interface to a CefURLRequest object.\n\n"
   "@ingroup CEF\n"
);

ImplementEnumType(URLRequestStatus,
   "The the possible status codes for a URL request.\n\n"
   "@ingroup CEF\n")
{ UR_UNKNOWN, "Unknown"},
{ UR_SUCCESS, "Success" },
{ UR_IO_PENDING, "Pending" },
{ UR_CANCELED, "Canceled" },
{ UR_FAILED, "Failed" },
EndImplementEnumType;

IMPLEMENT_CALLBACK(WebURLRequest, onLine, void, (const char* line), (line),
   "@brief Called whenever a line of data is received by the request handler.\n\n"
   "This callback is called when the received data contains a newline @\\n character, or "
   "the request has completed and the object's buffer is flushed.\n"
   "@param line Data sent from the server.\n");

IMPLEMENT_CALLBACK(WebURLRequest, onRequestComplete, bool, (URLRequestStatus requestStatus, S32 errorCode), (requestStatus, errorCode),
   "Called whenever a request has completed processing. Return true from the callback handler "
   "to release the request. Return false to retain it. If the request is retained, "
   "releaseRequest() must be called before another request can be made using this "
   "WebURLRequest object.\n\n"
   "@param requestStatus The status code for the request (Unknown, Success, Pending, Canceled or Failed).\n"
   "@param errorCode - Will be a non-zero value if the status is \"Canceled\" or \"Failed\".\n"
   "@ingroup CEF\n");

IMPLEMENT_CALLBACK(WebURLRequest, onUploadProgress, void, (const char* current, const char* total), (current, total),
   "Notifies the client of upload progress.\n\n"
   "@param current The number of bytes sent so far.\n"
   "@param total - The total size of uploading data (or -1 if chunked upload is enabled).\n"
   "@ingroup CEF\n");

IMPLEMENT_CALLBACK(WebURLRequest, onDownloadProgress, void, (const char* current, const char* total), (current, total),
   "Notifies the client of download progress.\n\n"
   "@param current The number of bytes received up to this call.\n"
   "@param total - The expected total size of the response (or -1 if not determined).\n"
   "@ingroup CEF\n");

IMPLEMENT_CALLBACK(WebURLRequest, onDownloadData, void, (const char* data, const char* data_size), (data, data_size),
   "Called when some part of the response is read. This method will not be called if the "
   "UR_FLAG_NO_DOWNLOAD_DATA flag is set on the request.\n\n"
   "@param data - Contains the current bytes received since the last call.\n"
   "@param data_size - The number of bytes in data.\n"
   "@ingroup CEF\n");

//------------------------------------------------------------------------------
WebURLRequest::WebURLRequest()
:  mBuffer(nullptr),
   mBufferSize(0),
   mSingleLineCallbacks(false),
   mRequestClient(nullptr),
   mRequest(nullptr),
   mOutputStream(nullptr)
{
}

WebURLRequest::~WebURLRequest()
{
   dFree(mBuffer);
}

bool WebURLRequest::processLine(UTF8 * line)
{
   onLine_callback(line);
   return true;
}

void WebURLRequest::parseLine(U8* buffer, size_t * start, size_t bufferLen)
{
   // find the first \n in buffer
   size_t i;
   U8 *line = buffer + *start;

   for (i = *start; i < bufferLen; i++)
      if (buffer[i] == '\n' || buffer[i] == 0)
         break;
   U32 len = i - *start;

   if (i == bufferLen || mBuffer)
   {
      // we've hit the end with no newline
      mBuffer = (U8 *)dRealloc(mBuffer, mBufferSize + len + 1);
      dMemcpy(mBuffer + mBufferSize, line, len);
      mBufferSize += len;
      *start = i;

      // process the line
      if (i != bufferLen)
      {
         mBuffer[mBufferSize] = 0;
         if (mBufferSize && mBuffer[mBufferSize - 1] == '\r')
            mBuffer[mBufferSize - 1] = 0;
         U8 *temp = mBuffer;
         mBuffer = 0;
         mBufferSize = 0;

         processLine((UTF8*)temp);
         dFree(temp);
      }
   }
   else if (i != bufferLen)
   {
      line[len] = 0;
      if (len && line[len - 1] == '\r')
         line[len - 1] = 0;
      processLine((UTF8*)line);
   }
   if (i != bufferLen)
      *start = i + 1;
}

bool WebURLRequest::finishLastLine()
{
   bool hadBuffer = false;

   if (mBufferSize)
   {
      mBuffer[mBufferSize] = 0;
      processLine((UTF8*)mBuffer);
      dFree(mBuffer);
      mBuffer = 0;
      mBufferSize = 0;

      hadBuffer = true;
   }

   if (mOutputStream)
   {
      SAFE_DELETE(mOutputStream);
   }

   return hadBuffer;
}

bool WebURLRequest::onAdd()
{
   mRequestClient = new WebRequestClient(this);
   return (mRequestClient != NULL) && Parent::onAdd();
}

void WebURLRequest::onRemove()
{
   if (mRequestClient)
   {
      mRequestClient->releaseCallbackObject();
      mRequestClient = nullptr;
   }

   finishLastLine();
   if (mRequest)
      mRequest = nullptr;

   Parent::onRemove();
}

bool WebURLRequest::makeURLRequest(const char* requestURL, const char* requestMethod,
   const char* postData, const char* headerMap)
{
   if (!requestURL || !requestMethod || mRequest)
      return false;

   CefRefPtr<CefRequest> request = CefRequest::Create();
   request->SetURL(requestURL);
   request->SetMethod(requestMethod);
   if (Con::getBoolVariable("DebugHttp"))
      Con::printf("Sending %s request to: %s", requestMethod, requestURL);

   if (postData && postData[0] != '\0')
   {
      CefRefPtr<CefPostData> requestData = CefPostData::Create();
      CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
      element->SetToBytes(dStrlen(postData), postData);
      requestData->AddElement(element);
      request->SetPostData(requestData);
 
      if (Con::getBoolVariable("DebugHttp"))
         Con::printf("PostData: %s", postData);
   }

   if (headerMap && headerMap[0] != '\0')
   {  // Break the headers into individual units.
      char tmpBufferPair[2048], tmpBufferValue[2048], tmpBufferHeader[256];
      CefRequest::HeaderMap requestMap;
      U32 headerUnits = StringUnit::getUnitCount(headerMap, "\t\n");
      for (U32 i = 0; i < headerUnits; ++i)
      {
         const char* headerPair = StringUnit::getUnit(headerMap, i, "\t\n", tmpBufferPair, sizeof(tmpBufferPair));
         const char* headerName = StringUnit::getUnit(headerPair, 0, ":\n", tmpBufferHeader, sizeof(tmpBufferHeader));
         const char* headerValue = StringUnit::getUnit(headerPair, 1, ":\n", tmpBufferValue, sizeof(tmpBufferValue));
         requestMap.insert(std::make_pair(headerName, headerValue));
      }
      if (headerUnits > 0)
         request->SetHeaderMap(requestMap);
   }

   request->SetFlags(UR_FLAG_ALLOW_STORED_CREDENTIALS);
   mRequest = CefURLRequest::Create(request, mRequestClient.get(), nullptr);
   if (mRequest)
      return true;

   return false;
}

bool WebURLRequest::makeFileRequest(const char* requestURL, const char* requestMethod,
   const char* postData, const char* headerMap, const char* filePath)
{
   char buffer[1024];
   Con::expandScriptFilename(buffer, sizeof(buffer), filePath);

   if (!buffer[0])
      return false;

   if ((mOutputStream = FileStream::createAndOpen(buffer, Torque::FS::File::Write)) == NULL)
      return false;

   mOutputStream->setPosition(0);

   return makeURLRequest(requestURL, requestMethod, postData, headerMap);
}

void WebURLRequest::releaseRequest()
{
   finishLastLine();
   mRequest = nullptr;
}

S32 WebURLRequest::getErrorCode()
{
   if (!mRequest)
      return 0;

   CefRefPtr< CefResponse > response = mRequest->GetResponse();
   if (!response)
      return 0;

   return (S32) response->GetError();
}

S32 WebURLRequest::getStatusId()
{
   if (!mRequest)
      return 0;

   CefRefPtr< CefResponse > response = mRequest->GetResponse();
   if (!response)
      return 0;

   return response->GetStatus();
}

void WebURLRequest::getStatusText(String& statusStr)
{
   if (!mRequest)
      return;

   CefRefPtr< CefResponse > response = mRequest->GetResponse();
   if (!response)
      return;

   statusStr = response->GetStatusText().ToString().c_str();
}

void WebURLRequest::getResponseHeaders(String& headerStr)
{
   if (!mRequest)
      return;

   CefRefPtr< CefResponse > response = mRequest->GetResponse();
   if (!response)
      return;

   CefRequest::HeaderMap headermap;
   response->GetHeaderMap(headermap);
   for (std::multimap<CefString, CefString>::iterator it = headermap.begin(); it != headermap.end(); it++)
   {
      headerStr += String::ToString("%s:%s\n", it->first.ToString().c_str(), it->second.ToString().c_str());
   }
}

void WebURLRequest::onRequestComplete()
{
   if (!mRequest)
      return;

   finishLastLine();

   cef_errorcode_t errCode = ERR_NONE;
   cef_urlrequest_status_t requestStatus = mRequest->GetRequestStatus();
   if (requestStatus == UR_FAILED || requestStatus == UR_CANCELED)
   {
      errCode = mRequest->GetRequestError();
   }

   if (onRequestComplete_callback(requestStatus, errCode))
      mRequest = nullptr;
}

void WebURLRequest::onUploadProgress(int64 current, int64 total)
{
   if (!mRequest)
      return;

   onUploadProgress_callback(String::ToString("%I64d", current).c_str(), String::ToString("%I64d", total).c_str());
}

void WebURLRequest::onDownloadProgress(int64 current, int64 total)
{
   if (!mRequest)
      return;

   onDownloadProgress_callback(String::ToString("%I64d", current).c_str(), String::ToString("%I64d", total).c_str());
}

void WebURLRequest::onDownloadData(const void* data, size_t data_length)
{
   if (!mRequest)
      return;

   if (mOutputStream)
   {
      mOutputStream->write(data_length, data);
   }
   else
   {
      if (mSingleLineCallbacks)
      {
         size_t start = 0;
         while (start < data_length)
            parseLine((U8 *)data, &start, data_length);
      }
      else
      {
         char* dataCopy = (char*)dMalloc(data_length + 1);
         dMemcpy(dataCopy, data, data_length);
         dataCopy[data_length] = '\0';
         onDownloadData_callback(dataCopy, String::ToString("%I64d", data_length).c_str());
         dFree(dataCopy);
      }
   }
}

//------------------------------------------------------------------------------
DefineEngineMethod(WebURLRequest, makeRequest, void, (const char* requestURL,
   const char* requestMethod, const char* postData, const char* headerMap), ("", ""),
   "Creates and dispatches a CefURLRequest.\n\n"

   "@param requestURL - The fully qualified URL to the request endpoint. "
   "(e.g. \"https://master.cloud.enjin.dev/graphql\", \"http://www.garagegames.com/feeds/rss/threads\")\n"
   "@param requestMethod The request method type (GET, POST, HEAD, DELETE or PUT).\n"
   "@param postData Data to be sent with POST requests. Empty for other request types. Default: \"\"\n"
   "@param headerMap A list of header:value pairs as a string of fields. "
   "header1:value1\\theader2:value2\\t\n"
   "Default: \"\".\n"

   "@tsexample\n"
   "@endtsexample\n\n")
{
   object->makeURLRequest(requestURL, requestMethod, postData, headerMap);
}

DefineEngineMethod(WebURLRequest, makeFileRequest, void, (const char* requestURL,
   const char* requestMethod, const char* postData, const char* headerMap, const char* filePath), ,
   "Creates and dispatches a CefURLRequest and saves the downloaded data into the named file.\n\n"

   "@param requestURL - The fully qualified URL to the request endpoint. "
   "(e.g. \"https://master.cloud.enjin.dev/graphql\", \"http://www.garagegames.com/feeds/rss/threads\")\n"
   "@param requestMethod The request method type (GET, POST, HEAD, DELETE or PUT).\n"
   "@param postData Data to be sent with POST requests. Empty for other request types.\n"
   "@param headerMap A list of header:value pairs as a string of fields. "
   "header1:value1\\theader2:value2\\t\n"
   "@param filePath The path and name for the downloaded file.\n"

   "@tsexample\n"
   "@endtsexample\n\n")
{
   object->makeFileRequest(requestURL, requestMethod, postData, headerMap, filePath);
}

DefineEngineMethod(WebURLRequest, releaseRequest, void, ( ), ,
   "Releases any existing CefURLRequest.\n\n")
{
   object->releaseRequest();
}

DefineEngineMethod(WebURLRequest, getErrorCode, S32, ( ), ,
   "Get the response error code. Returns ERR_NONE if there was no error. "
   "Error codes are enumerated in cef_errorcode_t.\n\n")
{
   return object->getErrorCode();
}

DefineEngineMethod(WebURLRequest, getStatusId, S32, ( ), ,
   "Get the HTTP response status code.\n\n")
{
   return object->getStatusId();
}

DefineEngineMethod(WebURLRequest, getStatusText, String, ( ), ,
   "Get the response status text.\n\n")
{
   String outStr;
   object->getStatusText(outStr);
   return outStr;
}

DefineEngineMethod(WebURLRequest, getResponseHeaders, String, ( ), ,
   "Get the response header map as a set of 'header:value' text fields.\n\n")
{
   String outStr;
   object->getResponseHeaders(outStr);
   return outStr;
}

DefineEngineMethod(WebURLRequest, setLineCallbacks, void, (bool lineCallbacks), (true),
   "Toggles between single line and chunk data callbacks. Pass true to receive "
   "onLine(%line) calls for each line of text recieved. Pass false to receive a "
   "onDownloadData(%data, %size) callback with each complete response chunk.\n\n")
{
   object->setLineCallbacks(lineCallbacks);
}
