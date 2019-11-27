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

#include "guiWebBrowser.h"
#include "guiWebRender.h"
#include "guiWebCtrl.h"
#include "gui/core/guiControl.h"
#include "include/wrapper/cef_helpers.h"


//------------------------------------------------------------------------------
GuiWebBrowser::GuiWebBrowser(GuiControl* browserGui, GuiWebRender *renderHandler)
{
   mBrowserGui = browserGui;
   m_renderHandler = renderHandler;
}

//------------------------------------------------------------------------------
GuiWebBrowser::~GuiWebBrowser()
{
}

// CefClient methods:
//------------------------------------------------------------------------------
bool GuiWebBrowser::OnProcessMessageReceived(
   CefRefPtr<CefBrowser> browser,
   CefRefPtr<CefFrame> frame,
   CefProcessId source_process,
   CefRefPtr<CefProcessMessage> message) {
   CEF_REQUIRE_UI_THREAD();

   // Check for messages from the client render process (JS).
   std::string message_name = message->GetName();
   if (message_name == "TorqueScript")
   {
      CefRefPtr<CefListValue> args = message->GetArgumentList();

      // The first argument must be the function name
      if ((args->GetSize() == 0) || (args->GetType(0) != VTYPE_STRING))
         return false;

      String tsArgs[MaxJSArgs];
      // Prepend "JS_" to the function name to prevent generic engine calls
      tsArgs[0] = "JS_" + String(args->GetString(0).c_str());

      int argc = 1;
      for (int i = 1; i < args->GetSize() && i < MaxJSArgs; ++i)
      {
         if (args->GetType(i) == VTYPE_STRING)
         {
            tsArgs[argc++] = args->GetString(i).c_str();
         }
         else if (args->GetType(i) == VTYPE_INT)
         {
            int test = args->GetInt(i);
            tsArgs[argc++] = String::ToString(test);
         }
         else if (args->GetType(i) == VTYPE_DOUBLE)
         {
            double test = args->GetDouble(i);
            tsArgs[argc++] = String::ToString(test);
         }
         else if (args->GetType(i) == VTYPE_BOOL)
         {
            bool test = args->GetBool(i);
            tsArgs[argc++] = String::ToString(test);
         }
      }

      for (int i = argc; i < MaxJSArgs; ++i)
         tsArgs[i].clear();

      Con::executef(mBrowserGui, tsArgs[0].c_str(), tsArgs[1].c_str(), tsArgs[2].c_str(), tsArgs[3].c_str(), tsArgs[4].c_str(),
         tsArgs[5].c_str(), tsArgs[6].c_str(), tsArgs[7].c_str());
      return true;
   }

   return false;
}

// CefLoadHandler methods:
//------------------------------------------------------------------------------
void GuiWebBrowser::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading,
   bool canGoBack, bool canGoForward)
{
   if (mBrowserGui)
      ((GuiWebCtrl*) mBrowserGui)->onLoadingStateChange(isLoading, canGoBack, canGoForward);
}

//------------------------------------------------------------------------------
void GuiWebBrowser::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
      ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
   String errorStr = errorText.c_str();
   String errorURL = failedUrl.c_str();
   if (mBrowserGui)
      ((GuiWebCtrl*)mBrowserGui)->onLoadError((S32) errorCode, errorStr.c_str(), errorURL.c_str());
}

// CefLifeSpanHandler methods:
//------------------------------------------------------------------------------
bool GuiWebBrowser::OnBeforePopup(CefRefPtr<CefBrowser> browser,
   CefRefPtr<CefFrame> frame,
   const CefString& target_url,
   const CefString& target_frame_name,
   WindowOpenDisposition target_disposition,
   bool user_gesture,
   const CefPopupFeatures& popupFeatures,
   CefWindowInfo& windowInfo,
   CefRefPtr<CefClient>& client,
   CefBrowserSettings& settings,
   CefRefPtr<CefDictionaryValue>& extra_info,
   bool* no_javascript_access)
{
   String URL = target_url.c_str();
   if (mBrowserGui)
   {
      S32 width = popupFeatures.widthSet ? popupFeatures.width : 0;
      S32 height = popupFeatures.heightSet ? popupFeatures.height : 0;
      return ((GuiWebCtrl*)mBrowserGui)->onPopupRequest(URL.c_str(), width, height);
   }

   return false;
}
