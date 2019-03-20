// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/cef_app.h"
#include "cefsimple/simple_app.h"

// Entry point function for sub-processes.
int main(int argc, char* argv[]) {
  // Provide CEF with command-line arguments.
  CefMainArgs main_args(argc, argv);

  // SimpleApp implements application-level callbacks for the render process.
  // It will bind the JS to TS function in OnContextCreated() after CEF has initialized.
  CefRefPtr<SimpleApp> app(new SimpleApp);

  // Execute the sub-process.
  return CefExecuteProcess(main_args, app.get(), NULL);
}
