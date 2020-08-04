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

#include "webApp.h"

WebApp::WebApp() {}

void WebApp::OnContextInitialized()
{
   CEF_REQUIRE_UI_THREAD();

   // Register the custom scheme handler factory.
   CefRegisterSchemeHandlerFactory("t3d", "", new ClientSchemeHandlerFactory());
}

// ClientSchemeHandlerFactory methods:
//------------------------------------------------------------------------------
// Return a new scheme handler instance to handle the request.
CefRefPtr<CefResourceHandler> ClientSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
   CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
   CEF_REQUIRE_IO_THREAD();

   return new ClientSchemeHandler();
}

// ClientSchemeHandler methods:
//------------------------------------------------------------------------------
bool ClientSchemeHandler::ProcessRequest(CefRefPtr<CefRequest> request,
   CefRefPtr<CefCallback> callback)
{
   CEF_REQUIRE_IO_THREAD();

   String filename = request->GetURL().ToString().c_str();
   // Strip t3d:// off the front of the request
   filename.erase(0, 6);
   if (filename.find("..") != String::NPos)
      return false;

   String filepath = Platform::getMainDotCsDir();
   filepath += "/";
   filepath += filename;

   file_path_ = Torque::Path::CompressPath(filepath.c_str());
   bool fileExists = Torque::FS::IsFile(file_path_);
   if (!fileExists)
      return false;

   if (dStristr(file_path_.getExtension().c_str(), "css") != 0)
      mime_type_ = "text/css";
   else if (dStristr(file_path_.getExtension().c_str(), "js") != 0)
      mime_type_ = "text/javascript";
   else if (dStristr(file_path_.getExtension().c_str(), "png") != 0)
      mime_type_ = "image/png";
   else if (dStristr(file_path_.getExtension().c_str(), "jpg") != 0)
      mime_type_ = "image/jpg";
   else
   {
      mime_type_ = "text/html";
   }

   data_size_ = Platform::getFileSize(file_path_.getFullPath());
   offset_ = 0;

   callback->Continue();
   return true;
}

//------------------------------------------------------------------------------
void ClientSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
   int64& response_length, CefString& redirectUrl)
{
   CEF_REQUIRE_IO_THREAD();

   response->SetMimeType(mime_type_);
   response->SetStatus(200);

   // Set the resulting response length.
   response_length = data_size_;
}

//------------------------------------------------------------------------------
bool ClientSchemeHandler::ReadResponse(void* data_out, int bytes_to_read,
   int& bytes_read, CefRefPtr<CefCallback> callback)
{
   CEF_REQUIRE_IO_THREAD();

   bool has_data = false;
   bytes_read = 0;

   Torque::FS::FileRef dataFile = Torque::FS::OpenFile(file_path_, Torque::FS::File::Read);
   if (dataFile != NULL)
   {
      dataFile->setPosition((U32) offset_, Torque::FS::File::Begin);
      bytes_read = dataFile->read(data_out, bytes_to_read);
      dataFile->close();

      offset_ += bytes_read;
      has_data = true;
   }

   return has_data;
}

//------------------------------------------------------------------------------
