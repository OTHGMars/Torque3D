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

#ifndef _WEBURLREQUEST_H_
#define _WEBURLREQUEST_H_

#ifndef _SIMSET_H_
#include "console/simSet.h"
#endif

#ifndef _SCRIPTOBJECTS_H_
#include "console/scriptObjects.h"
#endif

#ifndef _FILESTREAM_H_
#include "core/stream/fileStream.h"
#endif

//#include "platform/types.h"
//#include "include/cef_app.h"
#include "include/cef_urlrequest.h"

typedef cef_urlrequest_status_t URLRequestStatus;
DefineEnumType(URLRequestStatus);

class WebRequestClient;
class WebURLRequest : public ScriptObject
{
public:
   WebURLRequest();
   virtual ~WebURLRequest();

protected:
   typedef ScriptObject Parent;

   U8 *mBuffer;
   size_t mBufferSize;
   bool mSingleLineCallbacks;

   virtual bool processLine(UTF8 *line); // process a complete line of text... default action is to call into script
   void parseLine(U8 *buffer, size_t *start, size_t bufferLen);
   bool finishLastLine();

private:
   CefRefPtr< WebRequestClient > mRequestClient;
   CefRefPtr< CefURLRequest > mRequest;
   FileStream *mOutputStream;

public:
   /// Called when the object is added to the sim.
   virtual bool onAdd();

   /// Called when the object is removed from the sim.
   virtual void onRemove();

   /// Called to send a url request.
   bool makeURLRequest(const char* requestURL, const char* requestMethod, const char* postData, const char* headerMap);

   /// Called to send a url request and save the download data to a file.
   bool makeFileRequest(const char* requestURL, const char* requestMethod, const char* postData, const char* headerMap, const char* filePath);

   /// Called to release a completed request.
   void releaseRequest();

   /// Toggles between single line and chunk data callbacks
   void setLineCallbacks(bool lineCallbacks) { mSingleLineCallbacks = lineCallbacks; }

   /// Error handling details
   S32 getErrorCode();
   S32 getStatusId();
   void getStatusText(String& statusStr);
   void getResponseHeaders(String& headerStr);

   /// Request processing callbacks.
   virtual void onRequestComplete();
   virtual void onUploadProgress(int64 current, int64 total);
   virtual void onDownloadProgress(int64 current, int64 total);
   virtual void onDownloadData(const void* data, size_t data_length);

   DECLARE_CALLBACK(void, onLine, (const char* line));
   DECLARE_CALLBACK(bool, onRequestComplete, (URLRequestStatus requestStatus, S32 errorCode));
   DECLARE_CALLBACK(void, onUploadProgress, (const char* current, const char* total));
   DECLARE_CALLBACK(void, onDownloadProgress, (const char* current, const char* total));
   DECLARE_CALLBACK(void, onDownloadData, (const char* data, const char* data_size));

   DECLARE_CONOBJECT(WebURLRequest);
};

#endif // _WEBURLREQUEST_H_
