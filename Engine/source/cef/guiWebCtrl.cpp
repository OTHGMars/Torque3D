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

#include "guiWebCtrl.h"
#include "guiWebRender.h"
#include "guiWebBrowser.h"
#include "webEngine.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxTextureManager.h"
#include "platform/platformInput.h"
#include "windowManager/sdl/sdlWindow.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiOffscreenCanvas.h"
#include "SDL.h"
#include "SDL_syswm.h"

IMPLEMENT_CONOBJECT(GuiWebCtrl);

IMPLEMENT_CALLBACK(GuiWebCtrl, onLoadingStateChange, void, (bool isLoading, bool canGoBack, bool canGoForward),
   (isLoading, canGoBack, canGoForward),
   "Called when the loading state has changed. This callback will be executed twice -- "
   "once when loading is initiated either programmatically or by user action, and once "
   "when loading is terminated due to completion, cancellation or failure.\n\n"
   "@param isLoading True at the start of page load. False when loading is complete.\n"
   "@param canGoBack True if there is an earlier page in browser history and goBack() can be called.\n"
   "@param canGoForward True if there is a later page in browser history and goForward() can be called.");

IMPLEMENT_CALLBACK(GuiWebCtrl, onLoadError, void, (S32 errorCode, const char* errorText, const char* failedURL),
   (errorCode, errorText, failedURL),
   "Called when a navigation fails or is canceled.\n\n"
   "@param errorCode is the error code number. See net\base\net_error_list.h for "
   "complete descriptions of the error codes.\n"
   "@param errorText is the error text.\n"
   "@param failedUrl is the URL that failed to load.");

IMPLEMENT_CALLBACK(GuiWebCtrl, onControlModeChange, void, (bool isKeyboardMouse), (isKeyboardMouse),
   "Called when the user interaction mode changes from keyboard/mouse to controller or vice-versa.\n\n"
   "@param isKeyboardMouse True if the most recent input event came from a keyboard or mouse device. "
   "False if the most recent event was from any other device (joystick, gamepad, ovrdevice...etc.).");

//------------------------------------------------------------------------------
GuiWebCtrl::GuiWebCtrl()
:  mWebRenderer(NULL),
   mBrowser(NULL),
   mBrowserClient(NULL),
   mSourceRect(RectI::Zero),
   mOffScreenCanvas(NULL),
   mIsRectSet(false),
   mResolution(Point2I::Zero),
   mFramerate(30),
   mIsTransparent(false),
   mAlphaCutoff(0),
   mUnloadOnSleep(true),
   mIsPageLoading(false),
   mIsKeyboardMouse(true),
   mZoomLevel(0.0f),
   mForceCenter(false)
{
}

GuiWebCtrl::~GuiWebCtrl()
{
}

void GuiWebCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Browser");
   addField("Resolution", TypePoint2I, Offset(mResolution, GuiWebCtrl),
      "Forced resolution. Defaults to (0, 0) which sets the browser to the same size as the GuiWebCtrl.");
   addField("CenterBrowser", TypeBool, Offset(mForceCenter, GuiWebCtrl),
      "Only used if a forced resolution is set that does not match the gui aspect ratio. If this is "
      "false, the browser texture will be stretched to fill the GuiWebCtrl extent. If true, the browser "
      "will be centered within the gui at the aspect ratio defined by Resolution. The browser texture "
      "will fill the gui without stretching. Overhanging parts on two opposite sides will get clipped.");

   addField("StartURL", TypeRealString, Offset(mStartURL, GuiWebCtrl), "The URL which is loaded initially.");
   addField("SessionPath", TypeRealString, Offset(mSessionPath, GuiWebCtrl), "Path to a session file which will contain cookies, history, passwords etc. A blank path forces the control to use the default session.");
   addField("Framerate", TypeS32, Offset(mFramerate, GuiWebCtrl),
      "The maximum rate in frames per second (fps) that CefRenderHandler::OnPaint will be called for a "
      "windowless browser. The actual fps may be lower if the browser cannot generate frames at the "
      "requested rate. The minimum value is 1 and the maximum value is 60 (default 30). This value can "
      "also be changed dynamically via guiWebCtrl::SetFrameRate..");

   addField("IsTransparent", TypeBool, Offset(mIsTransparent, GuiWebCtrl), "Whether this control supports transparency or not. Default: False");
   addField("UnloadOnSleep", TypeBool, Offset(mUnloadOnSleep, GuiWebCtrl), "Unloads all resources if the GuiWebCtrl goes asleep. This can be used to keep the memory footprint down. Default: True");
   addField("ZoomLevel", TypeF32, Offset(mZoomLevel, GuiWebCtrl), "Sets the zoom level for the browser. Default 0.0.");
   endGroup("Browser");
}

bool GuiWebCtrl::onAdd()
{
   return Parent::onAdd();
}

void GuiWebCtrl::onRemove()
{
   if (mBrowser)
   {
      mBrowser->GetHost()->CloseBrowser(true);
      mBrowser = nullptr;
   }

   if (mBrowserClient)
      mBrowserClient = nullptr;

   if (mWebRenderer)
   {
      mWebRenderer->releaseGuiOwner();
      mWebRenderer = nullptr;
      GFXTextureManager::removeEventDelegate(this, &GuiWebCtrl::_onTextureEvent);
   }

   Parent::onRemove();
}

bool GuiWebCtrl::onWake()
{
   if (!gWebEngine)
      return false;

   if (!Parent::onWake())
      return false;

   if (!gWebEngine->isInitialized())
      gWebEngine->initCef();

   mOffScreenCanvas = dynamic_cast<GuiOffscreenCanvas*>(getRoot());

   createBrowserClient();

   if (!mBrowser && mStartURL.isNotEmpty())
   {
      String cleanedURL;
      makeCleanURL(cleanedURL, mStartURL);
      createBrowser(cleanedURL.c_str());
      if (mBrowser)
         mBrowser->GetMainFrame()->LoadURL(cleanedURL.c_str());
   }
   else if (mBrowser && !mBrowser->IsLoading())
   {
      mBrowser->GetHost()->WasResized();
      mBrowser->GetHost()->Invalidate(PET_VIEW);
   }

   return true;
}

void GuiWebCtrl::onSleep()
{
   if (mUnloadOnSleep)
   {
      if (mBrowser)
      {
         mBrowser->GetHost()->CloseBrowser(true);
         mBrowser = nullptr;
      }

      if (mBrowserClient)
         mBrowserClient = nullptr;

      if (mWebRenderer)
      {
         mWebRenderer->releaseGuiOwner();
         mWebRenderer = nullptr;
         GFXTextureManager::removeEventDelegate(this, &GuiWebCtrl::_onTextureEvent);
      }
   }

   Parent::onSleep();
}

bool GuiWebCtrl::createBrowser(const char *startUrl)
{
   if (!mBrowserClient)
      return false;

   CefWindowInfo window_info;
   CefBrowserSettings browserSettings;
   browserSettings.windowless_frame_rate = mClamp(mFramerate, 1, 60);

   PlatformWindowSDL *window = static_cast<PlatformWindowSDL*>(WindowManager->getFirstWindow());
#ifdef TORQUE_OS_WIN
   window_info.SetAsWindowless((HWND)window->getSystemWindow(PlatformWindow::WindowSystem_Windows));
#endif
#if defined(TORQUE_OS_MAC)
   window_info.SetAsWindowless(NULL);
#endif
#if defined(TORQUE_OS_LINUX)
   window_info.SetAsWindowless((unsigned long)window->getSystemWindow(PlatformWindow::WindowSystem_X11));
#endif

   mBrowser = CefBrowserHost::CreateBrowserSync(window_info, mBrowserClient.get(), startUrl, browserSettings, nullptr);
   return true;
}

void GuiWebCtrl::createBrowserClient()
{
   if (!mWebRenderer)
   {
      mWebRenderer = new GuiWebRender(this);
      GFXTextureManager::addEventDelegate(this, &GuiWebCtrl::_onTextureEvent);
   }
   mWebRenderer->setResolution(hasForcedResolution() ? mResolution : getExtent());

   if (!mBrowserClient)
      mBrowserClient = new GuiWebBrowser(this, mWebRenderer);
}

void GuiWebCtrl::_onTextureEvent(GFXTexCallbackCode code)
{
   switch (code)
   {
   case GFXZombify:
      break;

   case GFXResurrect:
      if (mWebRenderer)
         mWebRenderer->setResolution(hasForcedResolution() ? mResolution : getExtent(), true);
      if (mBrowser)
      {
         mBrowser->GetHost()->WasResized();
         mBrowser->GetHost()->Invalidate(PET_VIEW);
      }
      break;
   }
}

void GuiWebCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // Parent render.
   Parent::onRender(offset, updateRect);

   if (mWebRenderer && mWebRenderer->getTexture())
   {
      GFX->getDrawUtil()->clearBitmapModulation();

      if (mForceCenter)
      {
         if (!mIsRectSet)
         {
            GFXTextureObject* texture = mWebRenderer->getTexture();
            int inWidth = texture->getWidth();
            int inHeight = texture->getHeight();
            float inRatio = (float)inWidth / (float)inHeight;
            float outRatio = (float)updateRect.extent.x / (float)updateRect.extent.y;

            if (inRatio > outRatio)
            {
               mSourceRect.point.y = 0;
               mSourceRect.extent.y = inHeight;

               mSourceRect.extent.x = (outRatio * (float)inHeight);
               mSourceRect.point.x = (inWidth - mSourceRect.extent.x) / 2;
            }
            else
            {
               mSourceRect.point.x = 0;
               mSourceRect.extent.x = inWidth;

               mSourceRect.extent.y = ((float)inWidth / outRatio);
               mSourceRect.point.y = (inHeight - mSourceRect.extent.y) / 2;
            }
            mIsRectSet = true;
         }

         GFX->getDrawUtil()->drawBitmapStretchSR(mWebRenderer->getTexture(), updateRect, mSourceRect);
      }
      // If there is a forced resolution set, we stretch the bitmap across the control.
      else if (hasForcedResolution())
         GFX->getDrawUtil()->drawBitmapStretch(mWebRenderer->getTexture(), updateRect);
      else
         GFX->getDrawUtil()->drawBitmap(mWebRenderer->getTexture(), offset);
   
      if (mWebRenderer->isPopupVisible() && mWebRenderer->getPopupTexture())
      {
         RectI popupRect;
         mWebRenderer->getPopupRect(popupRect);
         Point2I popupOffset = localToGlobalCoord(popupRect.point);
         GFX->getDrawUtil()->drawBitmap(mWebRenderer->getPopupTexture(), popupOffset);
      }
   }
}


bool GuiWebCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
    if (Parent::resize(newPosition, newExtent))
    {
       if (!hasForcedResolution())
       {
          if (mWebRenderer)
             mWebRenderer->setResolution(getExtent());
          if (mBrowser)
             mBrowser->GetHost()->WasResized();
       }
       return true;
    }

    return false;
}

// Input event handlers
//-----------------------------------------------------------------------------
void GuiWebCtrl::setInputMode(bool isKeyboard)
{
   if (mIsKeyboardMouse != isKeyboard)
   {
      mIsKeyboardMouse = isKeyboard;
      onControlModeChange_callback(isKeyboard);
   }
}

bool GuiWebCtrl::onInputEvent(const InputEventInfo &inputEvent)
{
   if (!mBrowser || !gWebEngine)
      return false;

   // Check for the Back/stop accelerators (alt+backspace, alt+end).
   if ((inputEvent.modifier & SI_ALT) && (inputEvent.action == SI_BREAK) &&
      (inputEvent.deviceType == KeyboardDeviceType))
   {
      if (inputEvent.objInst == KEY_BACKSPACE)
         pageBack();
      if (inputEvent.objInst == KEY_END)
         pageStop();
      return false;
   }

   // General event handler is only used for  joystick/controller input.
   if ((inputEvent.deviceType == MouseDeviceType) || (inputEvent.deviceType == KeyboardDeviceType))
      return false;

   S32 keyCode;
   U32 flags;
   U16 asciiChar;
   if ((inputEvent.action == SI_MAKE) || (inputEvent.action == SI_BREAK))
   {
      if (gWebEngine->getMappedCefEvent(inputEvent.deviceType,inputEvent.deviceInst,
            inputEvent.objInst, keyCode, flags, asciiChar))
      {
         if (mIsKeyboardMouse)
            setInputMode(false);

         if (keyCode < 0)
         {
            // Only fire when pressed, not released
            if (inputEvent.action != SI_MAKE)
               return false;

            if (keyCode == WebEngine::browserBack)
               pageBack();
            else if (keyCode == WebEngine::browserForward)
               pageForward();
            else if (keyCode == WebEngine::browserReload)
               pageReload();
            else if (keyCode == WebEngine::browserStop)
               pageStop();
         }
         else
         {
            CefKeyEvent cefEvent;
#if defined(TORQUE_OS_WIN)
            cefEvent.native_key_code = cefEvent.windows_key_code = keyCode;
#elif defined(TORQUE_OS_LINUX)
            cefEvent.native_key_code = keyCode; //this is actually x keysym
            cefEvent.windows_key_code = gWebEngine->getVKCodefromX(keyCode);
#endif
            cefEvent.type = (inputEvent.action == SI_MAKE) ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
            cefEvent.modifiers = flags;

            mBrowser->GetHost()->SendKeyEvent(cefEvent);

            if (asciiChar > 0 && inputEvent.action == SI_MAKE)
            {
               CefKeyEvent cefAsciiEvent;
               cefAsciiEvent.windows_key_code = asciiChar;
#if defined(TORQUE_OS_LINUX)
               cefAsciiEvent.character = cefAsciiEvent.unmodified_character = asciiChar;
#endif
               cefAsciiEvent.type = KEYEVENT_CHAR;
               mBrowser->GetHost()->SendKeyEvent(cefAsciiEvent);
            }
         }
      }
   }
   //else if ((inputEvent.action == SI_MOVE) && (inputEvent.objType == SI_AXIS) && (mFabs(inputEvent.fValue) > 0.25f))
   //{
   //   S32 axisCode;
   //   if ((inputEvent.objInst == XI_THUMBLX) || (inputEvent.objInst == SI_XAXIS))
   //      axisCode = inputEvent.fValue > 0 ? gWebEngine->getVKCodeFromTorque(KEY_RIGHT) : gWebEngine->getVKCodeFromTorque(KEY_LEFT);
   //   else if ((inputEvent.objInst == XI_THUMBLY) || (inputEvent.objInst == SI_YAXIS))
   //      axisCode = inputEvent.fValue > 0 ? gWebEngine->getVKCodeFromTorque(KEY_DOWN) : gWebEngine->getVKCodeFromTorque(KEY_UP);
   //   else
   //      return false;

   //   if (mIsKeyboardMouse)
   //      setInputMode(false);

   //   CefKeyEvent cefEvent;
   //   cefEvent.native_key_code = cefEvent.windows_key_code = axisCode;
   //   cefEvent.type = KEYEVENT_RAWKEYDOWN;
   //   cefEvent.modifiers = 0;
   //   mBrowser->GetHost()->SendKeyEvent(cefEvent);
   //   cefEvent.type = KEYEVENT_KEYUP;
   //   mBrowser->GetHost()->SendKeyEvent(cefEvent);
   //}

   return true;
}

void GuiWebCtrl::onMouseUp(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   mouseUnlock();
   //if (mOffScreenCanvas && !mOffScreenCanvas->isActiveCanvas())
   //{
   //   clearFirstResponder();
   //   onLoseFirstResponder();
   //}

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_LEFT, true, event.mouseClickCount);
   }
}

void GuiWebCtrl::onMouseDown(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   //if (!mOffScreenCanvas || mOffScreenCanvas->isActiveCanvas())
      setFirstResponder();

   if (!mIsKeyboardMouse)
      setInputMode(true);

   //lock the mouse
   mouseLock();

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      cefEvent.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_LEFT, false, event.mouseClickCount);
   }
}

void GuiWebCtrl::onMouseMove(const GuiEvent &event)
{
    if (!mActive || !mAwake || !mVisible)
        return;

    if (mBrowser)
    {
       CefMouseEvent cefEvent;
       makeCefMouseEvent(event, cefEvent);
       mBrowser->GetHost()->SendMouseMoveEvent(cefEvent, false);
    }
}

void GuiWebCtrl::onMouseDragged(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      cefEvent.modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseMoveEvent(cefEvent, false);
   }
}

bool GuiWebCtrl::onMouseWheelUp(const GuiEvent &event)
{
   Parent::onMouseWheelUp(event);
   if (!mAwake || !mVisible)
        return false;

   if (!mIsKeyboardMouse)
      setInputMode(true);

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      mBrowser->GetHost()->SendMouseWheelEvent(cefEvent, 0, (int)event.fval);
   }

    return true;
}

bool GuiWebCtrl::onMouseWheelDown(const GuiEvent &event)
{
   Parent::onMouseWheelDown(event);
   if (!mAwake || !mVisible)
        return false;

   if (!mIsKeyboardMouse)
      setInputMode(true);

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      mBrowser->GetHost()->SendMouseWheelEvent(cefEvent, 0, (int)event.fval);
   }

   return true;
}

void GuiWebCtrl::onRightMouseDown(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (!mIsKeyboardMouse)
      setInputMode(true);

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);

      // If we're rendered on the canvas, convert right clicks to screen coords
      if (mOffScreenCanvas == NULL)
      {
         Point2I screenCoord = getRoot()->getPlatformWindow()->clientToScreen(event.mousePoint);
         cefEvent.x = screenCoord.x;
         cefEvent.y = screenCoord.y;
      }

      cefEvent.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_RIGHT, false, event.mouseClickCount);
   }
}

void GuiWebCtrl::onRightMouseUp(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);

      // If we're rendered on the canvas, convert right clicks to screen coords
      if (mOffScreenCanvas == NULL)
      {
         Point2I screenCoord = getRoot()->getPlatformWindow()->clientToScreen(event.mousePoint);
         cefEvent.x = screenCoord.x;
         cefEvent.y = screenCoord.y;
      }

      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_RIGHT, true, event.mouseClickCount);
   }
}

void GuiWebCtrl::onRightMouseDragged(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      cefEvent.modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseMoveEvent(cefEvent, false);
   }
}

void GuiWebCtrl::onMiddleMouseDown(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (!mIsKeyboardMouse)
      setInputMode(true);

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      cefEvent.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_MIDDLE, false, event.mouseClickCount);
   }
}

void GuiWebCtrl::onMiddleMouseUp(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_MIDDLE, false, event.mouseClickCount);
   }
}

void GuiWebCtrl::onMiddleMouseDragged(const GuiEvent &event)
{
   if (!mActive || !mAwake || !mVisible)
      return;

   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      makeCefMouseEvent(event, cefEvent);
      cefEvent.modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseMoveEvent(cefEvent, false);
   }
}

bool GuiWebCtrl::onKeyDown(const GuiEvent &event)
{
   sendCefKeyEvent(event, true);
   return true;
}

bool GuiWebCtrl::onKeyUp(const GuiEvent &event)
{
   sendCefKeyEvent(event, false);
   return true;
}

void GuiWebCtrl::setFirstResponder()
{
   Parent::setFirstResponder();

   GuiCanvas *root = getRoot();
   if (root != NULL)
   {
      root->enableKeyboardTranslation();

      // If the native OS accelerator keys are not disabled
      // then some key events like Delete, ctrl+V, etc may
      // not make it down to us.
      root->setNativeAcceleratorsEnabled( false );
   }

   if (mBrowser)
      mBrowser->GetHost()->SendFocusEvent(true);
}

void GuiWebCtrl::onLoseFirstResponder()
{
   Parent::onLoseFirstResponder();
   GuiCanvas *root = getRoot();
   if (root != NULL)
   {
      root->setNativeAcceleratorsEnabled( true );
      root->disableKeyboardTranslation();
   }

   if (mBrowser)
      mBrowser->GetHost()->SendFocusEvent(false);
}

bool GuiWebCtrl::getCaretPos(Point2I& pos)
{
    //wkeRect rect = mWebView->getCaret();
    //if (rect.x == 0 && rect.y == 0)
    //    return false;

    //pos.x = rect.x;
    //pos.y = rect.y + rect.h;

    //pos = localToGlobalCoord(pos);
    return true;
}

void GuiWebCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
    GuiCanvas *pRoot = getRoot();
    if( !pRoot )
        return;

    const int invalid_cur = 999;
    if (pRoot->mCursorChanged == invalid_cur)
        return;

    PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
    PlatformCursorController *pController = pWindow->getCursorController();

    // Now change the cursor shape
    pController->popCursor();
    pController->pushCursor(invalid_cur);
    pRoot->mCursorChanged = invalid_cur;
}

void GuiWebCtrl::setMouseCursor(U8 type)
{
   if (type >= CT_CUSTOM)
      type = 0;
   U32 torqueCursor = gWebEngine->getTorqueCursorFromCEF(type);

   GuiCanvas *pRoot = getRoot();
   if (!pRoot)
      return;

   PlatformWindow *pWindow = pRoot->getPlatformWindow();
   AssertFatal(pWindow != NULL, "GuiControl without owning platform window!  This should not be possible.");
   PlatformCursorController *pController = pWindow->getCursorController();
   AssertFatal(pController != NULL, "PlatformWindow without an owned CursorController!");
   pController->setCursorShape(torqueCursor);
}

void GuiWebCtrl::markDirty()
{
   if (mOffScreenCanvas)
      mOffScreenCanvas->markDirty();
}

void GuiWebCtrl::sendCefKeyEvent(const GuiEvent &torqueEvent, bool isDown)
{
   if (!mIsKeyboardMouse)
      setInputMode(true);

   CefKeyEvent cefEvent;
   if (torqueEvent.keyCode)
   {
      const U32 vkCode = gWebEngine->getVKCodeFromTorque(torqueEvent.keyCode);
#if defined(TORQUE_OS_WIN)
      cefEvent.native_key_code = cefEvent.windows_key_code = vkCode;
#elif defined(TORQUE_OS_LINUX)
      cefEvent.native_key_code = vkCode; //this is actually x keysym
      cefEvent.windows_key_code = gWebEngine->getVKCodefromX(vkCode);
#endif
      cefEvent.type = isDown ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
   }
   else if (torqueEvent.ascii)
   {
      if (!isDown)
         return;

      cefEvent.windows_key_code = torqueEvent.ascii;
   #if defined(TORQUE_OS_LINUX)
      cefEvent.character = cefEvent.unmodified_character = torqueEvent.ascii;
   #endif
      cefEvent.type = KEYEVENT_CHAR;
   }

   unsigned int flags = EVENTFLAG_NONE;
   if (torqueEvent.modifier & SI_CTRL)
      flags |= EVENTFLAG_CONTROL_DOWN;
   if (torqueEvent.modifier & SI_SHIFT)
      flags |= EVENTFLAG_SHIFT_DOWN;
   if (torqueEvent.modifier & SI_ALT)
      flags |= EVENTFLAG_ALT_DOWN;
   cefEvent.modifiers = flags;

   if (mBrowser)
      mBrowser->GetHost()->SendKeyEvent(cefEvent);

   // Cef expects '\r' sent with the enter key, but SDL doesn't send it. So ...
   if (torqueEvent.keyCode == KEY_RETURN && isDown)
   {
      CefKeyEvent cefAsciiEvent;
      cefAsciiEvent.windows_key_code = (U16) '\r';
      cefAsciiEvent.type = KEYEVENT_CHAR;
      mBrowser->GetHost()->SendKeyEvent(cefAsciiEvent);
   }
}

void GuiWebCtrl::makeCefMouseEvent(const GuiEvent &event, CefMouseEvent &cefEvent)
{
   Point2I point = globalToLocalCoord(event.mousePoint);
   if (hasForcedResolution())
   {
      point.x = F32((F32)point.x / (F32)getWidth()) * (F32)mResolution.x;
      point.y = F32((F32)point.y / (F32)getHeight()) * (F32)mResolution.y;
   }

   cefEvent.x = point.x;
   cefEvent.y = point.y;

   unsigned int flags = EVENTFLAG_NONE;
   if (event.modifier & SI_CTRL)
       flags |= EVENTFLAG_CONTROL_DOWN;
   if (event.modifier & SI_SHIFT)
       flags |= EVENTFLAG_SHIFT_DOWN;
   if (event.modifier & SI_ALT)
      flags |= EVENTFLAG_ALT_DOWN;
   cefEvent.modifiers = flags;
}

void GuiWebCtrl::loadURL(const char* url)
{
   if (url)
   {
      String cleanedURL;
      makeCleanURL(cleanedURL, url);

      if (!mBrowser)
      {
         createBrowserClient();
         createBrowser(cleanedURL.c_str());
         return;
      }

      mBrowser->GetMainFrame()->LoadURL(cleanedURL.c_str());
   }
}

void GuiWebCtrl::loadURLPostRequest(const char* url, const char* reqData)
{
   if (!url || !reqData)
      return;

   if (!mBrowser)
   {
      createBrowserClient();
      createBrowser("about:blank"); // See: Comment#1 here for why about:blank is needed: https://code.google.com/archive/p/chromiumembedded/issues/579
   }

   CefRefPtr<CefRequest> request = CefRequest::Create();

   String cleanedURL;
   makeCleanURL(cleanedURL, url);
   request->SetURL(cleanedURL.c_str());

   // Optionally specify custom headers.
   //CefRequest::HeaderMap headerMap;
   //headerMap.insert(
   //   std::make_pair("X-My-Header", "My Header Value"));
   //request->SetHeaderMap(headerMap);

   request->SetMethod("POST");
   const std::string& upload_data = reqData;
   CefRefPtr<CefPostData> postData = CefPostData::Create();
   CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
   element->SetToBytes(upload_data.size(), upload_data.c_str());
   postData->AddElement(element);
   request->SetPostData(postData);

   mBrowser->GetMainFrame()->LoadRequest(request);
}

void GuiWebCtrl::getURL(String& url)
{
   if (mBrowser)
      url = mBrowser->GetMainFrame()->GetURL().c_str();
}

void GuiWebCtrl::makeCleanURL(String& cleanURL, String url)
{
   cleanURL = url;
   if (url.find("T3D://") != String::NPos)
   {
      cleanURL = String::ToString("%s/%s", Platform::getMainDotCsDir(), url.substr(6).c_str());
   }
}

void GuiWebCtrl::execJavaScript(const String &script, const String &iFrame)
{
   if (!mBrowser)
      return;

   if (iFrame.isEmpty())
   {
      CefRefPtr<CefFrame> frame = mBrowser->GetMainFrame();
      frame->ExecuteJavaScript(CefString(script.c_str()), frame->GetURL(), 0);
   }
   else
   {
      CefRefPtr<CefFrame> frame = mBrowser->GetFrame(CefString(script.c_str()));
      if (frame)
         frame->ExecuteJavaScript(CefString(script.c_str()), frame->GetURL(), 0);
      else
         Con::errorf("Unable to find frame [%s] for ExecuteJavaScript(%s)", iFrame.c_str(), script.c_str());
   }
}

void GuiWebCtrl::touchPage(const Point2I &point)
{
   if (mBrowser)
   {
      CefMouseEvent cefEvent;
      cefEvent.x = point.x;
      cefEvent.y = point.y;

      //cefEvent.modifiers = 0;
      cefEvent.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_LEFT, false, 1);
      cefEvent.modifiers = 0;
      mBrowser->GetHost()->SendMouseClickEvent(cefEvent, MBT_LEFT, true, 1);
      CefDoMessageLoopWork();
   }
}

bool GuiWebCtrl::pageForward()
{
   if (mBrowser && mBrowser->CanGoForward())
   {
      mBrowser->GoForward();
      return true;
   }
   return false;
}

bool GuiWebCtrl::pageBack()
{
   if (mBrowser && mBrowser->CanGoBack())
   {
      mBrowser->GoBack();
      return true;
   }
   return false;
}

void GuiWebCtrl::pageReload()
{
   if (mBrowser)
      mBrowser->Reload();
}

void GuiWebCtrl::pageStop()
{
   if (mBrowser && mBrowser->IsLoading())
      mBrowser->StopLoad();
}

void GuiWebCtrl::onLoadingStateChange(bool isLoading, bool canGoBack, bool canGoForward)
{
   mIsPageLoading = isLoading;
   onLoadingStateChange_callback(isLoading, canGoBack, canGoForward);
   if (!isLoading && mBrowser->GetHost()->GetZoomLevel() != (double) mZoomLevel)
      mBrowser->GetHost()->SetZoomLevel((double) mZoomLevel);
}

void GuiWebCtrl::onLoadError(S32 errorCode, const char* errorText, const char* failedURL)
{
   onLoadError_callback(errorCode, errorText, failedURL);
}

void GuiWebCtrl::setFrameRate(U32 fps)
{
   if (mBrowser)
      mBrowser->GetHost()->SetWindowlessFrameRate(mClamp(fps, 1, 60));
}

void GuiWebCtrl::setZoomLevel(F32 zoomLevel)
{
   mZoomLevel = zoomLevel;
   if (mBrowser && !mBrowser->IsLoading() && mBrowser->GetHost()->GetZoomLevel() != (double)mZoomLevel)
      mBrowser->GetHost()->SetZoomLevel((double)mZoomLevel);
}

DefineEngineMethod(GuiWebCtrl, loadURL, void, (const char *url), ,
   "@brief Load the url in the browser.\n\n"
   "@param url The url to load. A properly formatted url string (file://..., http://... etc).\n")
{
   object->loadURL(url);
}

DefineEngineMethod(GuiWebCtrl, urlPost, void, (const char *url, const char *postData), ,
   "@brief Sends a url load request with POST data attached.\n\n"
   "@param url The url to load. A properly formatted url string (file://..., http://... etc).\n"
   "@param postData The arguments to post with the command. The default 'Content-Type' header "
   "value is 'application / x - www - form - urlencoded'. "
   "See https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-custom-requests "
   "for details about adding additional header values.\n\n"
   "@tsexample\n"
   "MyWebControl.urlPost(\"https://www.mydomain.com/playerstats/\",\"userID=32&statPage=4\");\n"
   "@endtsexample\n\n")
{
   object->loadURLPostRequest(url, postData);
}

DefineEngineMethod(GuiWebCtrl, getURL, String, (), ,
   "@brief Returns the url of the main frame loaded in the browser.\n\n")
{
   String url;
   object->getURL(url);
   return url;
}

DefineEngineMethod(GuiWebCtrl, execJavaScript, void, (const char *script, const char *iFrame), (""),
   "@brief Execute a string of JavaScript code in this frame.\n\n"
   "@param script The JavaScript instruction to execute.\n"
   "@param iFrame The name of the frame to execute within. If this is blank, the main browser frame will be used.\n"
   "This executes a single JavaScript instruction formatted as a string asynchronously on the browser. "
   "Any return value from the instruction will not be returned to TorqueScript. To return data to "
   "torque script, call window.execTS() on the JavaScript side to explicitly pass data.\n\n"

   "@tsexample\n"
   "MyWebControl.execJavaScript(\"AddForTorque(.2, 5);\");\n"
   "function GuiWebCtrl::JS_MyResultFunc(%this, %result)\n"
   "{\n"
   "   echo(%result); // Print the value returned from JS\n"
   "}\n\n"

   "//And in JavaScript on the loaded url:\n"
   "function AddForTorque(firstArg, secondArg)\n" 
   "{\n"
   "   var myResult = firstArg + secondArg;\n"
   "   if (window.execTS)\n"
   "      window.execTS(\"MyResultFunc\", myResult);\n"
   "}\n"
   "@endtsexample\n\n")
{
   object->execJavaScript(script, iFrame);
}

DefineEngineMethod(GuiWebCtrl, touchPage, void, (Point2I point), (Point2I::One),
   "@brief Preforms a mouse click/release at a specific point on the page.\n\n"
   "For security reasons, Chromium disallows some external javascript commands until "
   "after a user has interacted with the page. If you need to run javascript on your page "
   "to initialize it after it loads, call touchPage() after the page has completed loading "
   "but before your first javascript from torque call.\n"
   "@param point The pixel to click on. Default \"1 1\".\n")
{
   object->touchPage(point);
}

DefineEngineMethod(GuiWebCtrl, pageForward, bool, (), ,
   "@brief Go forward one page in the browsers history.\n\n"
   "@return Returns true if there is a page in history to go to. False if there is no forward page to go to.\n")
{
   return object->pageForward();
}

DefineEngineMethod(GuiWebCtrl, pageBack, bool, (), ,
   "@brief Go back one page in the browsers history.\n\n"
   "@return Returns true if there is a page in history to go back to. False if there is no page to go back to.\n")
{
   return object->pageBack();
}

DefineEngineMethod(GuiWebCtrl, pageReload, void, (), ,
   "@brief Reload the current page.\n")
{
   object->pageReload();
}

DefineEngineMethod(GuiWebCtrl, pageStop, void, (), ,
   "@brief Stops the current page load.\n")
{
   object->pageStop();
}

DefineEngineMethod(GuiWebCtrl, setFrameRate, void, (U32 fps), (30),
   "Set the maximum rate in frames per second (fps) that CefRenderHandler::OnPaint "
   "will be called for a windowless browser. The actual fps may be lower if the "
   "browser cannot generate frames at the requested rate. The minimum value is 1 "
   "and the maximum value is 60 (default 30). Can also be set at browser creation "
   "via GuiWebCtrl.Framerate.\n")
{
   object->setFrameRate(fps);
}

DefineEngineMethod(GuiWebCtrl, setZoomLevel, void, (F32 zoomLevel), (0.0f),
   "Set the zoom level for the browser. Default 0.0.\n")
{
   object->setZoomLevel(zoomLevel);
}
