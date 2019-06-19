// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp, public CefRenderProcessHandler {
public:
   SimpleApp();

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

   virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE { return this; }

   // CefRenderProcessHandler methods:
   virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefV8Context> context) OVERRIDE;

private:
   // Include the default reference counting implementation.
   IMPLEMENT_REFCOUNTING(SimpleApp);
};

class TorqueV8Handler : public CefV8Handler {
private:
   CefRefPtr<CefBrowser> browser;
   CefRefPtr<CefFrame> frame;

public:
   TorqueV8Handler(CefRefPtr<CefBrowser> initB, CefRefPtr<CefFrame> initF) { browser = initB; frame = initF; }

   virtual bool Execute(const CefString& name,
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments,
      CefRefPtr<CefV8Value>& retval,
      CefString& exception) OVERRIDE
   {
      if (name == "execTS") {

         CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("TorqueScript");

         // Retrieve the argument list object.
         CefRefPtr<CefListValue> args = msg->GetArgumentList();
         for (int i = 0; i < arguments.size(); ++i)
         {
            if (arguments[i]->IsString())
               args->SetString(i, arguments[i]->GetStringValue());
            else if (arguments[i]->IsInt())
               args->SetInt(i, arguments[i]->GetIntValue());
            else if (arguments[i]->IsDouble())
               args->SetDouble(i, arguments[i]->GetDoubleValue());
            else if (arguments[i]->IsBool())
               args->SetBool(i, arguments[i]->GetBoolValue());
         }

         frame->SendProcessMessage(PID_BROWSER, msg);
         // Return true to JS.
         retval = CefV8Value::CreateBool(true);
         //LOG(WARNING) << "TorqueV8Handler::Execute() returning true.";
         return true;
      }

      // Function does not exist.
      LOG(WARNING) << "TorqueV8Handler::Execute() returning false.";
      return false;
   }

   // Provide the reference counting implementation for this class.
   IMPLEMENT_REFCOUNTING(TorqueV8Handler);
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
