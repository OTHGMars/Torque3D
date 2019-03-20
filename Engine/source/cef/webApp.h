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

#ifndef _WEBAPP_H_
#define _WEBAPP_H_

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "console/console.h"
#include "core/stream/fileStream.h"

// Implementation of the factory for creating scheme handlers.
class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
   ClientSchemeHandlerFactory() {}

   // Return a new scheme handler instance to handle the request.
   CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& scheme_name,
      CefRefPtr<CefRequest> request) OVERRIDE;

private:
   IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
   DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandlerFactory);
};

// Implement application-level callbacks for the browser process.
class WebApp : public CefApp, public CefBrowserProcessHandler
{
public:
   WebApp();

   // CefApp methods:
   ///
   // Provides an opportunity to register custom schemes. Do not keep a reference
   // to the |registrar| object. This method is called on the main thread for
   // each process and the registered schemes should be the same across all
   // processes.
   ///
   /*--cef()--*/
   void OnRegisterCustomSchemes(
   CefRawPtr<CefSchemeRegistrar> registrar)  OVERRIDE
   {
      // Register the t3d custom scheme as standard and secure.
      // Must be the same implementation in all processes.
      registrar->AddCustomScheme("t3d", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE);
   }
    
   virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }

   // CefBrowserProcessHandler methods:
   virtual void OnContextInitialized() OVERRIDE;

private:
   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(WebApp);
   DISALLOW_COPY_AND_ASSIGN(WebApp);
};

// Implementation of the scheme handler for t3d:// requests.
class ClientSchemeHandler : public CefResourceHandler {
public:
   ClientSchemeHandler() : offset_(0) {}

   bool ProcessRequest(CefRefPtr<CefRequest> request,
      CefRefPtr<CefCallback> callback) OVERRIDE;

   void GetResponseHeaders(CefRefPtr<CefResponse> response,
      int64& response_length,
      CefString& redirectUrl) OVERRIDE;

   void Cancel() OVERRIDE { CEF_REQUIRE_IO_THREAD(); }

   bool ReadResponse(void* data_out,
      int bytes_to_read,
      int& bytes_read,
      CefRefPtr<CefCallback> callback) OVERRIDE;

private:
   Torque::Path file_path_;
   std::string mime_type_;
   size_t offset_;
   size_t data_size_;

   IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
   DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandler);
};

#endif  // _WEBAPP_H_
