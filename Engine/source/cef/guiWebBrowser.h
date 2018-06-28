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
#ifndef _GUIWEBBROWSER_H_
#define _GUIWEBBROWSER_H_
#include "include/cef_app.h"
#include "include/cef_client.h"

class GuiControl;
class GuiWebRender;
class GuiWebBrowser : public CefClient, public CefLoadHandler
{
private:
   enum Constants {
      MaxJSArgs = 8, // Maximum arguments in a JS->TS call including function name as first arg.
   };

   // The gui control this browser will forward JS->TS calls and load messages to.
   GuiControl* mBrowserGui;

public:
   GuiWebBrowser(GuiControl* browserGui, GuiWebRender *renderHandler);
   ~GuiWebBrowser();

   CefRefPtr<CefRenderHandler> m_renderHandler;

   // CefClient methods:
   virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return m_renderHandler; }
   CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }

   bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) OVERRIDE;

   // CefLoadHandler methods:
   ///
   // Called when the loading state has changed. This callback will be executed
   // twice -- once when loading is initiated either programmatically or by user
   // action, and once when loading is terminated due to completion, cancellation
   // of failure. It will be called before any calls to OnLoadStart and after all
   // calls to OnLoadError and/or OnLoadEnd.
   ///
   void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading,
      bool canGoBack, bool canGoForward) OVERRIDE;

   ///
   // Called when a navigation fails or is canceled. This method may be called
   // by itself if before commit or in combination with OnLoadStart/OnLoadEnd if
   // after commit. |errorCode| is the error code number, |errorText| is the
   // error text and |failedUrl| is the URL that failed to load.
   // See net\base\net_error_list.h for complete descriptions of the error codes.
   ///
   /*--cef(optional_param=errorText)--*/
   void OnLoadError(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      ErrorCode errorCode,
      const CefString& errorText,
      const CefString& failedUrl) OVERRIDE;

   IMPLEMENT_REFCOUNTING(GuiWebBrowser);
};

#endif // _GUIWEBBROWSER_H_

