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

#ifndef _WEBREQUESTCLIENT_H_
#define _WEBREQUESTCLIENT_H_
#include "include/cef_urlrequest.h"

class WebURLRequest;
class WebRequestClient : public  CefURLRequestClient
{
private:

   // The script object to forward callbacks to.
   WebURLRequest* mCallbackObject;

public:
   WebRequestClient(WebURLRequest* callbackObj);
   ~WebRequestClient();

   ///
   // Clears the callback object so it can be safely deleted.
   ///
   void releaseCallbackObject() { mCallbackObject = nullptr; }

   ///
   // Notifies the client that the request has completed. Use the
   // CefURLRequest::GetRequestStatus method to determine if the request was
   // successful or not.
   ///
   /*--cef()--*/
   virtual void OnRequestComplete(CefRefPtr<CefURLRequest> request);

   ///
   // Notifies the client of upload progress. |current| denotes the number of
   // bytes sent so far and |total| is the total size of uploading data (or -1 if
   // chunked upload is enabled). This method will only be called if the
   // UR_FLAG_REPORT_UPLOAD_PROGRESS flag is set on the request.
   ///
   /*--cef()--*/
   virtual void OnUploadProgress(CefRefPtr<CefURLRequest> request,
      int64 current,
      int64 total);

   ///
   // Notifies the client of download progress. |current| denotes the number of
   // bytes received up to the call and |total| is the expected total size of the
   // response (or -1 if not determined).
   ///
   /*--cef()--*/
   virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
      int64 current,
      int64 total);

   ///
   // Called when some part of the response is read. |data| contains the current
   // bytes received since the last call. This method will not be called if the
   // UR_FLAG_NO_DOWNLOAD_DATA flag is set on the request.
   ///
   /*--cef()--*/
   virtual void OnDownloadData(CefRefPtr<CefURLRequest> request,
      const void* data,
      size_t data_length);

   ///
   // Called on the IO thread when the browser needs credentials from the user.
   // |isProxy| indicates whether the host is a proxy server. |host| contains the
   // hostname and |port| contains the port number. Return true to continue the
   // request and call CefAuthCallback::Continue() when the authentication
   // information is available. Return false to cancel the request. This method
   // will only be called for requests initiated from the browser process.
   ///
   /*--cef(optional_param=realm)--*/
   virtual bool GetAuthCredentials(bool isProxy,
      const CefString& host,
      int port,
      const CefString& realm,
      const CefString& scheme,
      CefRefPtr<CefAuthCallback> callback);

   IMPLEMENT_REFCOUNTING(WebRequestClient);
};

#endif // _WEBREQUESTCLIENT_H_

