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
#ifndef _WEBENGINE_H_
#define _WEBENGINE_H_

#ifndef _TDICTIONARY_H_
#include "core/util/tDictionary.h"
#endif
#ifndef _ENGINEAPI_H_
   #include "console/engineAPI.h"
#endif
#include "platform/types.h"
#include "include/cef_app.h"

class ProcessList;
class WebEngine
{
public:
   enum LogModeType
   {
      ModeDefault = LOGSEVERITY_DEFAULT,
      ModeVerbose = LOGSEVERITY_VERBOSE,
      ModeInfo = LOGSEVERITY_INFO,
      ModeWarning = LOGSEVERITY_WARNING,
      ModeError = LOGSEVERITY_ERROR,
      ModeNone = LOGSEVERITY_DISABLE,
   };

   enum BrowserEventType
   {
      browserBack = -1,
      browserForward = -2,
      browserReload = -3,
      browserStop = -4
   };

   struct CefKeyEvent
   {
      S32 code;
      U32 flags;
      U16 ascii;
      CefKeyEvent() : code(0), flags(0), ascii(0) { }
      CefKeyEvent(const S32& sVal, const U32& uVal, const U16& aVal) : code(sVal), flags(uVal), ascii(aVal) { }
   };
   typedef Map<U32, CefKeyEvent> KeyEventMap;
   typedef Map<U32, KeyEventMap> KeyDeviceMap;

private:
   ProcessList* mProcessList;
   bool mCefInitialized;
   KeyDeviceMap mMappedDevices;
   void addMappedEvent(U32 deviceType, U32 deviceNum, U32 deviceCode, S32 keyCode, U32 flags, U16 asciiChar);
   U16 asciiFromTorqueCode(S32 keyCode, bool shiftDown);

   void removeCefLogFile();

   //Mappings - see webEngineMappings.h
   static void _buildKeyMaps();
   const static U8 VKToTorque[256];
   static U32 TorqueToVK[256];
   static const U8 CEFCursorToTorque[];

protected:
   static StringTableEntry mSubProcessPath;
   static StringTableEntry mRootCachePath;
   static StringTableEntry mCachePath;
   static StringTableEntry mUserDataPath;
   static StringTableEntry mLocaleStr;
   static StringTableEntry mLogFile;
   static StringTableEntry mUserAgent;
   static S32 mLogSeverity;
   static StringTableEntry mResourcePath;
   static StringTableEntry mLocalesPath;
   static S32 mRemoteDebuggingPort;

public:
   DECLARE_STATIC_CLASS(WebEngine);

   WebEngine();
   ~WebEngine();

   bool init();
   void shutdown();
   void process();

   inline bool isInitialized() { return mCefInitialized; }
   bool initCef();
   //this will return native X keysym under linux, on windows it will return the virtual keycode
   inline static U32 getVKCodeFromTorque(U32 torque) { return TorqueToVK[torque]; }
#if defined(TORQUE_OS_LINUX)
   //return posix virtual keycode from X keysym
   static U32 getVKCodefromX(U32 keysym);
#endif

   inline static U32 getTorqueCursorFromCEF(U8 cefCursor) { return CEFCursorToTorque[cefCursor]; }

   bool getMappedCefEvent(const U32 deviceType, const U32 deviceNum, const U32 deviceCode, S32& keyCode, U32& flags, U16& asciiChar);
   bool mapDeviceEvent(const char* deviceInst, const char* deviceAction, const char* keyboardAction);
};

typedef WebEngine::LogModeType CefLogModeType;
DefineEnumType(CefLogModeType);

/// The global instance of WebEngine, if there is one.
extern WebEngine *gWebEngine;

#endif // _WEBENGINE_H_
