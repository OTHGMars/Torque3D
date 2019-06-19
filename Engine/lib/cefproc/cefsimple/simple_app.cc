// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefsimple/simple_app.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

SimpleApp::SimpleApp()
{
}

void SimpleApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
   CefRefPtr<CefFrame> frame,
   CefRefPtr<CefV8Context> context)
{
   // Retrieve the context's window object.
   CefRefPtr<CefV8Value> object = context->GetGlobal();

   // Create an instance of my CefV8Handler object.
   CefRefPtr<CefV8Handler> handler = new TorqueV8Handler(browser, frame);

   // JavaScript to TorqueScript bridge handler.
   // Create the "execTS" function.
   CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("execTS", handler);

   // Bind the "execTS" function to the "window" object.
   object->SetValue("execTS", func, V8_PROPERTY_ATTRIBUTE_NONE);

   //LOG(WARNING) << "SimpleApp::OnContextCreated().";
}