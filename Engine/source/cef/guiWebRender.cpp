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

#include "guiWebRender.h"
#include "guiWebCtrl.h"

//------------------------------------------------------------------------------
GuiWebRender::GuiWebRender(GuiWebCtrl* webGui)
{
   mWebGui = webGui;
   mResolution = Point2I::Zero;
   mPopupRect = RectI::Zero;
   mPopupShown = false;
}

//------------------------------------------------------------------------------
GuiWebRender::~GuiWebRender()
{
}

//------------------------------------------------------------------------------
void GuiWebRender::setResolution(const Point2I &resolution, bool forceReset /*= false*/)
{
   if (forceReset)
      mTexture = NULL;

   if (!mTexture || mTexture.getWidth() != resolution.x || mTexture.getHeight() != resolution.y)
   {
      if (resolution.x > 0 && resolution.y > 0)
         mTexture.set(resolution.x, resolution.y, GFXFormatR8G8B8A8_SRGB, &GFXDynamicTextureSRGBProfile,
            String::ToString("GuiWebRender (%s:%i)", __FILE__, __LINE__), 1, 0);
   }
   mResolution = resolution;
}

//------------------------------------------------------------------------------
void GuiWebRender::setPopupResolution(const Point2I &resolution)
{
   if (!mPopupTexture || mPopupTexture.getWidth() != resolution.x || mPopupTexture.getHeight() != resolution.y)
   {
      if (resolution.x > 0 && resolution.y > 0)
         mPopupTexture.set(resolution.x, resolution.y, GFXFormatR8G8B8A8_SRGB, &GFXDynamicTextureSRGBProfile,
         String::ToString("GuiWebRender (%s:%i)", __FILE__, __LINE__), 1, 0);
   }
}

//------------------------------------------------------------------------------
void GuiWebRender::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
   rect = CefRect(0, 0, mTexture.getWidth(), mTexture.getHeight());
   return;
}

//------------------------------------------------------------------------------
void GuiWebRender::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
   mPopupShown = show;
   if (!show)
      clearPopupRect();
}

//------------------------------------------------------------------------------
void GuiWebRender::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
   RectI rc(rect.x, rect.y, rect.width, rect.height);

   // if x or y are negative, move them to 0.
   if (rc.point.x < 0)
      rc.point.x = 0;
   if (rc.point.y < 0)
      rc.point.y = 0;
   // if popup goes outside the view, try to reposition origin
   if (rc.point.x + rc.extent.x > mResolution.x)
      rc.point.x = mResolution.x - rc.extent.y;
   if (rc.point.y + rc.extent.y > mResolution.y)
      rc.point.y = mResolution.y - rc.extent.y;
   // if x or y became negative, move them to 0 again.
   if (rc.point.x < 0)
      rc.point.x = 0;
   if (rc.point.y < 0)
      rc.point.y = 0;

   mPopupRect = rc;
   setPopupResolution(rc.extent);
}

//------------------------------------------------------------------------------
void GuiWebRender::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{
   if (type == PET_POPUP)
   {
      paintPopupTex(buffer, width, height);
      return;
   }
   if (!mTexture)
      return;

   U8* pixBuffer = (U8*)buffer;
   U32 smallestWidth = getMin(mTexture.getWidth(), (U32)width);
   U32 smallestHeight = getMin(mTexture.getHeight(), (U32)height);
   RectI updateRect;

   //const Swizzle<U8, 4> *pSwizzle = GFX->getDeviceSwizzle32();
   for (int i = 0; i < dirtyRects.size(); ++i)
   {
      U32 maxX = getMin((U32)(dirtyRects[i].x + dirtyRects[i].width), smallestWidth);
      U32 maxY = getMin((U32)(dirtyRects[i].y + dirtyRects[i].height), smallestHeight);
      updateRect.set(dirtyRects[i].x, dirtyRects[i].y, maxX - dirtyRects[i].x, maxY - dirtyRects[i].y);

      GFXLockedRect *rect = mTexture.lock(0, &updateRect);

      for (U32 y = dirtyRects[i].y; y < maxY; ++y)
      {
         U32 targetY = y - dirtyRects[i].y;
         for (U32 x = dirtyRects[i].x; x < maxX; ++x)
         {
            U32 targetX = x - dirtyRects[i].x;
            U32 targetIndex = (targetY * rect->pitch) + (targetX * 4);
            U32 sourceIndex = ((y * width) + x) * 4;
            // We're receiving BGRA from CEF, so we need B/R flipped on ogl, but not dx
            //rect->bits[targetIndex] = pixBuffer[sourceIndex];
            //rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
            //rect->bits[targetIndex + 2] = pixBuffer[sourceIndex + 2];
            //rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
            //pSwizzle->InPlace(&rect->bits[targetIndex], sizeof(U32));
            if (GFX->getAdapterType() == OpenGL)
            {
               rect->bits[targetIndex] = pixBuffer[sourceIndex + 2]; //swizzle
               rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
               rect->bits[targetIndex + 2] = pixBuffer[sourceIndex]; //swizzle
               rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
            }
            else
            {
               rect->bits[targetIndex] = pixBuffer[sourceIndex];
               rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
               rect->bits[targetIndex + 2] = pixBuffer[sourceIndex + 2];
               rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
            }
         }
      }

      mTexture.unlock();
   }

   if (mWebGui)
      mWebGui->markDirty();
}

//------------------------------------------------------------------------------
void GuiWebRender::paintPopupTex(const void * buffer, int width, int height)
{
   U8* pixBuffer = (U8*)buffer;
   U32 smallestWidth = getMin(mPopupTexture.getWidth(), (U32)width);
   U32 smallestHeight = getMin(mPopupTexture.getHeight(), (U32)height);
   RectI updateRect;

   if (!mPopupTexture)
      return;

   //const Swizzle<U8, 4> *pSwizzle = GFX->getDeviceSwizzle32();
   GFXLockedRect *rect = mPopupTexture.lock();

   for (U32 y = 0; y < smallestHeight; ++y)
   {
      for (U32 x = 0; x < smallestWidth; ++x)
      {
         U32 targetIndex = (y * rect->pitch) + (x * 4);
         U32 sourceIndex = ((y * width) + x) * 4;
         // We're receiving BGRA from CEF, so we need B/R flipped on ogl, but not dx
         //rect->bits[targetIndex] = pixBuffer[sourceIndex];
         //rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
         //rect->bits[targetIndex + 2] = pixBuffer[sourceIndex + 2];
         //rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
         //pSwizzle->InPlace(&rect->bits[targetIndex], sizeof(U32));
         if (GFX->getAdapterType() == OpenGL)
         {
            rect->bits[targetIndex] = pixBuffer[sourceIndex + 2]; //swizzle
            rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
            rect->bits[targetIndex + 2] = pixBuffer[sourceIndex]; //swizzle
            rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
         }
         else
         {
            rect->bits[targetIndex] = pixBuffer[sourceIndex];
            rect->bits[targetIndex + 1] = pixBuffer[sourceIndex + 1];
            rect->bits[targetIndex + 2] = pixBuffer[sourceIndex + 2];
            rect->bits[targetIndex + 3] = pixBuffer[sourceIndex + 3];
         }
      }
   }

   mPopupTexture.unlock();

   if (mWebGui)
      mWebGui->markDirty();
}

//------------------------------------------------------------------------------
void GuiWebRender::clearPopupRect()
{
   mPopupRect = RectI::Zero;
   mPopupTexture = NULL;
}

//------------------------------------------------------------------------------
bool GuiWebRender::onDeviceEvent(GFXDevice::GFXDeviceEventType evt)
{
   if (mResolution.x > 0 && mResolution.y > 0)
   {
      mTexture.set(mResolution.x, mResolution.y, GFXFormatR8G8B8A8_SRGB, &GFXDynamicTextureSRGBProfile,
         String::ToString("GuiWebRender (%s:%i)", __FILE__, __LINE__), 1, 0);
   }

   if (mPopupRect.extent.x > 0 && mPopupRect.extent.y > 0)
   {
      mPopupTexture.set(mPopupRect.extent.x, mPopupRect.extent.y, GFXFormatR8G8B8A8_SRGB, &GFXDynamicTextureSRGBProfile,
         String::ToString("GuiWebRender (%s:%i)", __FILE__, __LINE__), 1, 0);
   }
   return true;
}

//------------------------------------------------------------------------------
void GuiWebRender::OnCursorChange(CefRefPtr<CefBrowser> browser,
   CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info)
{
   if (mWebGui)
      mWebGui->setMouseCursor((U8)type);
}

//------------------------------------------------------------------------------
void GuiWebRender::getPopupRect(RectI &rect)
{
   rect = mPopupRect;
}

//------------------------------------------------------------------------------
