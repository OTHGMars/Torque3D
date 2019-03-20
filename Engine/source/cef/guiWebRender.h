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
#ifndef _GUIWEBRENDER_H_
#define _GUIWEBRENDER_H_
#include "include/cef_render_handler.h"
#include "gfx/gfxDevice.h"

class GFXTexHandle;
class GuiWebCtrl;
class GuiWebRender : public CefRenderHandler
{
private:
   GFXTexHandle mTexture;        // Texture for main browser view
   GFXTexHandle mPopupTexture;   // Texture for popup window while shown
   GuiWebCtrl *mWebGui;          // The gui control that owns and receives callbacks from this view
   Point2I mResolution;          // Resolution of the main browser view
   RectI mPopupRect;             // Display rectangle for the popup
   bool mPopupShown;             // Should the popup texture be rendered

   void setPopupResolution(const Point2I &resolution);
   void paintPopupTex(const void *buffer, int width, int height);
   void clearPopupRect();

public:
   GuiWebRender(GuiWebCtrl *webGui);
   ~GuiWebRender();

   GFXTexHandle getTexture() { return mTexture; }
   void setResolution(const Point2I &resolution, bool forceReset = false);

   inline bool isPopupVisible() { return mPopupShown; }
   GFXTexHandle getPopupTexture() { return mPopupTexture; }
   void getPopupRect(RectI &rect);
   bool onDeviceEvent(GFXDevice::GFXDeviceEventType evt);

   void releaseGuiOwner() { mWebGui = NULL; }

   // CefRenderHandler interface
public:
   void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
   void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
   void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;
   void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) OVERRIDE;

   virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
      CefCursorHandle cursor,
      CursorType type,
      const CefCursorInfo& custom_cursor_info) OVERRIDE;

   virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDragData> drag_data,
      DragOperationsMask allowed_ops,
      int x,
      int y) {
      return true;
   }


   // CefBase interface
public:
   IMPLEMENT_REFCOUNTING(GuiWebRender);
};

#endif // _GUIWEBRENDER_H_

