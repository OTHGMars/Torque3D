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
#ifndef _GUIWEBCTRL_H_
#define _GUIWEBCTRL_H_
#include "include/cef_render_handler.h"

#ifndef _GUICONTROL_H_
   #include "gui/core/guiControl.h"
#endif

class GuiWebRender;
class GuiWebBrowser;
class GuiOffscreenCanvas;
class GuiWebCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

   CefRefPtr<GuiWebRender> mWebRenderer;
   CefRefPtr<CefBrowser> mBrowser;
   CefRefPtr<GuiWebBrowser> mBrowserClient;

   RectI mSourceRect;
   GuiOffscreenCanvas* mOffScreenCanvas;
   bool mIsRectSet;

   bool createBrowser(const char *startUrl);
   void makeCefMouseEvent(const GuiEvent &event, CefMouseEvent &cefEvent);
   void sendCefKeyEvent(const GuiEvent &event, bool isDown);
   void setInputMode(bool isKeyboard);

protected:
   Point2I mResolution;
   String mStartURL;
   String mSessionPath;
   S32 mFramerate;
   bool mIsTransparent;
   U8 mAlphaCutoff;
   bool mUnloadOnSleep;
   bool mIsPageLoading;
   bool mIsKeyboardMouse;

   bool mForceCenter;

   inline bool hasForcedResolution() { return mResolution.x != 0 && mResolution.y != 0; }

   DECLARE_CALLBACK(void, onLoadingStateChange, (bool isLoading, bool canGoBack, bool canGoForward));
   DECLARE_CALLBACK(void, onLoadError, (S32 errorCode, const char* errorText, const char* failedURL));
   DECLARE_CALLBACK(void, onControlModeChange, (bool isKeyboardMouse));

public:
   DECLARE_CONOBJECT(GuiWebCtrl);

   GuiWebCtrl();
   virtual ~GuiWebCtrl();
   static void initPersistFields();

   virtual bool onAdd();
   virtual void onRemove();

   virtual bool onWake();
   virtual void onSleep();
   virtual void onRender(Point2I offset, const RectI &updateRect);
   virtual bool resize(const Point2I &newPosition, const Point2I &newExtent);

   virtual bool onInputEvent(const InputEventInfo &inputEvent);

   virtual void onMouseUp(const GuiEvent &event);
   virtual void onMouseDown(const GuiEvent &event);
   virtual void onMouseMove(const GuiEvent &event);
   virtual void onMouseDragged(const GuiEvent &event);

   virtual bool onMouseWheelUp(const GuiEvent &event);
   virtual bool onMouseWheelDown(const GuiEvent &event);

   virtual void onRightMouseDown(const GuiEvent &event);
   virtual void onRightMouseUp(const GuiEvent &event);
   virtual void onRightMouseDragged(const GuiEvent &event);

   virtual void onMiddleMouseDown(const GuiEvent &event);
   virtual void onMiddleMouseUp(const GuiEvent &event);
   virtual void onMiddleMouseDragged(const GuiEvent &event);

   virtual bool onKeyDown(const GuiEvent &event);
   virtual bool onKeyUp(const GuiEvent &event);

   virtual void setFirstResponder();
   virtual void onLoseFirstResponder();

   void _onTextureEvent(GFXTexCallbackCode code);

   virtual void markDirty();
   virtual bool getCaretPos(Point2I& pos);

   virtual void getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent);
   virtual void setMouseCursor(U8 type);

   void loadURL(const char* url);
   void loadURLPostRequest(const char* url, const char* reqData);

   void execJavaScript(const String &script, const String &iFrame);
   void touchPage(const Point2I &point);
   bool pageForward();
   bool pageBack();
   void pageReload();
   void onLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward);
   void onLoadError(S32 errorCode, const char* errorText, const char* failedURL);

protected:
   GFXTexHandle mTextureObject;
};

#endif