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

#pragma once

#if defined(TORQUE_OS_WIN)
const U8 WebEngine::VKToTorque[] =
{
   0,                   // 0x00
   0,                   // 0x01  VK_LBUTTON
   0,                   // 0x02  VK_RBUTTON
   0,                   // 0x03  VK_CANCEL
   0,                   // 0x04  VK_MBUTTON
   0,                   // 0x05
   0,                   // 0x06
   0,                   // 0x07
   KEY_BACKSPACE,       // 0x08  VK_BACK
   KEY_TAB,             // 0x09  VK_TAB
   0,                   // 0x0A
   0,                   // 0x0B
   0,                   // 0x0C  VK_CLEAR
   KEY_RETURN,          // 0x0D  VK_RETURN
   0,                   // 0x0E
   0,                   // 0x0F
   KEY_SHIFT,           // 0x10  VK_SHIFT
   KEY_CONTROL,         // 0x11  VK_CONTROL
   KEY_ALT,             // 0x12  VK_MENU
   KEY_PAUSE,           // 0x13  VK_PAUSE
   KEY_CAPSLOCK,        // 0x14  VK_CAPITAL
   0,                   // 0x15  VK_KANA, VK_HANGEUL, VK_HANGUL
   0,                   // 0x16
   0,                   // 0x17  VK_JUNJA
   0,                   // 0x18  VK_FINAL
   0,                   // 0x19  VK_HANJA, VK_KANJI
   0,                   // 0x1A
   KEY_ESCAPE,          // 0x1B  VK_ESCAPE

   0,                   // 0x1C  VK_CONVERT
   0,                   // 0x1D  VK_NONCONVERT
   0,                   // 0x1E  VK_ACCEPT
   0,                   // 0x1F  VK_MODECHANGE

   KEY_SPACE,           // 0x20  VK_SPACE
   KEY_PAGE_UP,         // 0x21  VK_PRIOR
   KEY_PAGE_DOWN,       // 0x22  VK_NEXT
   KEY_END,             // 0x23  VK_END
   KEY_HOME,            // 0x24  VK_HOME
   KEY_LEFT,            // 0x25  VK_LEFT
   KEY_UP,              // 0x26  VK_UP
   KEY_RIGHT,           // 0x27  VK_RIGHT
   KEY_DOWN,            // 0x28  VK_DOWN
   0,                   // 0x29  VK_SELECT
   KEY_PRINT,           // 0x2A  VK_PRINT
   0,                   // 0x2B  VK_EXECUTE
   0,                   // 0x2C  VK_SNAPSHOT
   KEY_INSERT,          // 0x2D  VK_INSERT
   KEY_DELETE,          // 0x2E  VK_DELETE
   KEY_HELP,            // 0x2F  VK_HELP

   KEY_0,               // 0x30  VK_0   VK_0 thru VK_9 are the same as ASCII '0' thru '9' (// 0x30 - // 0x39)
   KEY_1,               // 0x31  VK_1
   KEY_2,               // 0x32  VK_2
   KEY_3,               // 0x33  VK_3
   KEY_4,               // 0x34  VK_4
   KEY_5,               // 0x35  VK_5
   KEY_6,               // 0x36  VK_6
   KEY_7,               // 0x37  VK_7
   KEY_8,               // 0x38  VK_8
   KEY_9,               // 0x39  VK_9
   0,                   // 0x3A
   0,                   // 0x3B
   0,                   // 0x3C
   0,                   // 0x3D
   0,                   // 0x3E
   0,                   // 0x3F
   0,                   // 0x40

   KEY_A,               // 0x41  VK_A      VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (// 0x41 - // 0x5A)
   KEY_B,               // 0x42  VK_B
   KEY_C,               // 0x43  VK_C
   KEY_D,               // 0x44  VK_D
   KEY_E,               // 0x45  VK_E
   KEY_F,               // 0x46  VK_F
   KEY_G,               // 0x47  VK_G
   KEY_H,               // 0x48  VK_H
   KEY_I,               // 0x49  VK_I
   KEY_J,               // 0x4A  VK_J
   KEY_K,               // 0x4B  VK_K
   KEY_L,               // 0x4C  VK_L
   KEY_M,               // 0x4D  VK_M
   KEY_N,               // 0x4E  VK_N
   KEY_O,               // 0x4F  VK_O
   KEY_P,               // 0x50  VK_P
   KEY_Q,               // 0x51  VK_Q
   KEY_R,               // 0x52  VK_R
   KEY_S,               // 0x53  VK_S
   KEY_T,               // 0x54  VK_T
   KEY_U,               // 0x55  VK_U
   KEY_V,               // 0x56  VK_V
   KEY_W,               // 0x57  VK_W
   KEY_X,               // 0x58  VK_X
   KEY_Y,               // 0x59  VK_Y
   KEY_Z,               // 0x5A  VK_Z


   KEY_WIN_LWINDOW,     // 0x5B  VK_LWIN
   KEY_WIN_RWINDOW,     // 0x5C  VK_RWIN
   KEY_WIN_APPS,        // 0x5D  VK_APPS
   0,                   // 0x5E
   0,                   // 0x5F

   KEY_NUMPAD0,         // 0x60  VK_NUMPAD0
   KEY_NUMPAD1,         // 0x61  VK_NUMPAD1
   KEY_NUMPAD2,         // 0x62  VK_NUMPAD2
   KEY_NUMPAD3,         // 0x63  VK_NUMPAD3
   KEY_NUMPAD4,         // 0x64  VK_NUMPAD4
   KEY_NUMPAD5,         // 0x65  VK_NUMPAD5
   KEY_NUMPAD6,         // 0x66  VK_NUMPAD6
   KEY_NUMPAD7,         // 0x67  VK_NUMPAD7
   KEY_NUMPAD8,         // 0x68  VK_NUMPAD8
   KEY_NUMPAD9,         // 0x69  VK_NUMPAD9
   KEY_MULTIPLY,        // 0x6A  VK_MULTIPLY
   KEY_ADD,             // 0x6B  VK_ADD
   KEY_SEPARATOR,       // 0x6C  VK_SEPARATOR
   KEY_SUBTRACT,        // 0x6D  VK_SUBTRACT
   KEY_DECIMAL,         // 0x6E  VK_DECIMAL
   KEY_DIVIDE,          // 0x6F  VK_DIVIDE
   KEY_F1,              // 0x70  VK_F1
   KEY_F2,              // 0x71  VK_F2
   KEY_F3,              // 0x72  VK_F3
   KEY_F4,              // 0x73  VK_F4
   KEY_F5,              // 0x74  VK_F5
   KEY_F6,              // 0x75  VK_F6
   KEY_F7,              // 0x76  VK_F7
   KEY_F8,              // 0x77  VK_F8
   KEY_F9,              // 0x78  VK_F9
   KEY_F10,             // 0x79  VK_F10
   KEY_F11,             // 0x7A  VK_F11
   KEY_F12,             // 0x7B  VK_F12
   KEY_F13,             // 0x7C  VK_F13
   KEY_F14,             // 0x7D  VK_F14
   KEY_F15,             // 0x7E  VK_F15
   KEY_F16,             // 0x7F  VK_F16
   KEY_F17,             // 0x80  VK_F17
   KEY_F18,             // 0x81  VK_F18
   KEY_F19,             // 0x82  VK_F19
   KEY_F20,             // 0x83  VK_F20
   KEY_F21,             // 0x84  VK_F21
   KEY_F22,             // 0x85  VK_F22
   KEY_F23,             // 0x86  VK_F23
   KEY_F24,             // 0x87  VK_F24
   0,                   // 0x88
   0,                   // 0x89
   0,                   // 0x8A
   0,                   // 0x8B
   0,                   // 0x8C
   0,                   // 0x8D
   0,                   // 0x8E
   0,                   // 0x8F

   KEY_NUMLOCK,         // 0x90  VK_NUMLOCK
   KEY_SCROLLLOCK,      // 0x91  VK_OEM_SCROLL
   0,                   // 0x92
   0,                   // 0x93
   0,                   // 0x94
   0,                   // 0x95
   0,                   // 0x96
   0,                   // 0x97
   0,                   // 0x98
   0,                   // 0x99
   0,                   // 0x9A
   0,                   // 0x9B
   0,                   // 0x9C
   0,                   // 0x9D
   0,                   // 0x9E
   0,                   // 0x9F

   KEY_LSHIFT,          // 0xA0  VK_LSHIFT
   KEY_RSHIFT,          // 0xA1  VK_RSHIFT
   KEY_LCONTROL,        // 0xA2  VK_LCONTROL
   KEY_RCONTROL,        // 0xA3  VK_RCONTROL
   KEY_LALT,            // 0xA4  VK_LMENU
   KEY_RALT,            // 0xA5  VK_RMENU
   0,                   // 0xA6
   0,                   // 0xA7
   0,                   // 0xA8
   0,                   // 0xA9
   0,                   // 0xAA
   0,                   // 0xAB
   0,                   // 0xAC
   0,                   // 0xAD
   0,                   // 0xAE
   0,                   // 0xAF
   0,                   // 0xB0
   0,                   // 0xB1
   0,                   // 0xB2
   0,                   // 0xB3
   0,                   // 0xB4
   0,                   // 0xB5
   0,                   // 0xB6
   0,                   // 0xB7
   0,                   // 0xB8
   0,                   // 0xB9
   KEY_SEMICOLON,       // 0xBA  VK_OEM_1
   KEY_EQUALS,          // 0xBB  VK_OEM_PLUS
   KEY_COMMA,           // 0xBC  VK_OEM_COMMA
   KEY_MINUS,           // 0xBD  VK_OEM_MINUS
   KEY_PERIOD,          // 0xBE  VK_OEM_PERIOD
   KEY_SLASH,           // 0xBF  VK_OEM_2
   KEY_TILDE,           // 0xC0  VK_OEM_3
   0,                   // 0xC1
   0,                   // 0xC2
   0,                   // 0xC3
   0,                   // 0xC4
   0,                   // 0xC5
   0,                   // 0xC6
   0,                   // 0xC7
   0,                   // 0xC8
   0,                   // 0xC9
   0,                   // 0xCA
   0,                   // 0xCB
   0,                   // 0xCC
   0,                   // 0xCD
   0,                   // 0xCE
   0,                   // 0xCF
   0,                   // 0xD0
   0,                   // 0xD1
   0,                   // 0xD2
   0,                   // 0xD3
   0,                   // 0xD4
   0,                   // 0xD5
   0,                   // 0xD6
   0,                   // 0xD7
   0,                   // 0xD8
   0,                   // 0xD9
   0,                   // 0xDA
   KEY_LBRACKET,        // 0xDB  VK_OEM_4
   KEY_BACKSLASH,       // 0xDC  VK_OEM_5
   KEY_RBRACKET,        // 0xDD  VK_OEM_6
   KEY_APOSTROPHE,      // 0xDE  VK_OEM_7
   0,                   // 0xDF  VK_OEM_8
   0,                   // 0xE0
   0,                   // 0xE1  VK_OEM_AX  AX key on Japanese AX keyboard
   KEY_OEM_102,         // 0xE2  VK_OEM_102
   0,                   // 0xE3
   0,                   // 0xE4

   0,                   // 0xE5  VK_PROCESSKEY

   0,                   // 0xE6
   0,                   // 0xE7
   0,                   // 0xE8
   0,                   // 0xE9
   0,                   // 0xEA
   0,                   // 0xEB
   0,                   // 0xEC
   0,                   // 0xED
   0,                   // 0xEE
   0,                   // 0xEF

   0,                   // 0xF0
   0,                   // 0xF1
   0,                   // 0xF2
   0,                   // 0xF3
   0,                   // 0xF4
   0,                   // 0xF5

   0,                   // 0xF6  VK_ATTN
   0,                   // 0xF7  VK_CRSEL
   0,                   // 0xF8  VK_EXSEL
   0,                   // 0xF9  VK_EREOF
   0,                   // 0xFA  VK_PLAY
   0,                   // 0xFB  VK_ZOOM
   0,                   // 0xFC  VK_NONAME
   0,                   // 0xFD  VK_PA1
   0,                   // 0xFE  VK_OEM_CLEAR
   0                    // 0xFF
};

U32 WebEngine::TorqueToVK[] = { 0 };

void WebEngine::_buildKeyMaps()
{
  dMemset(TorqueToVK, 0, 256);

  // Torque to virtual keycode
  for (U32 i = 0; i < 256; ++i)
  {
     if (VKToTorque[i] > 0)
        TorqueToVK[VKToTorque[i]] = i;
  }
}

#elif defined(TORQUE_OS_LINUX)

#include <X11/keysym.h>

//unused
const U8 WebEngine::VKToTorque[] = {0};
//contains X keysym to posix vkey
U32 WebEngine::TorqueToVK[] = {0};

enum PosixKeyboardCode {
  VKEY_BACK = 0x08,
  VKEY_TAB = 0x09,
  VKEY_BACKTAB = 0x0A,
  VKEY_CLEAR = 0x0C,
  VKEY_RETURN = 0x0D,
  VKEY_SHIFT = 0x10,
  VKEY_CONTROL = 0x11,
  VKEY_MENU = 0x12,
  VKEY_PAUSE = 0x13,
  VKEY_CAPITAL = 0x14,
  VKEY_KANA = 0x15,
  VKEY_HANGUL = 0x15,
  VKEY_JUNJA = 0x17,
  VKEY_FINAL = 0x18,
  VKEY_HANJA = 0x19,
  VKEY_KANJI = 0x19,
  VKEY_ESCAPE = 0x1B,
  VKEY_CONVERT = 0x1C,
  VKEY_NONCONVERT = 0x1D,
  VKEY_ACCEPT = 0x1E,
  VKEY_MODECHANGE = 0x1F,
  VKEY_SPACE = 0x20,
  VKEY_PRIOR = 0x21,
  VKEY_NEXT = 0x22,
  VKEY_END = 0x23,
  VKEY_HOME = 0x24,
  VKEY_LEFT = 0x25,
  VKEY_UP = 0x26,
  VKEY_RIGHT = 0x27,
  VKEY_DOWN = 0x28,
  VKEY_SELECT = 0x29,
  VKEY_PRINT = 0x2A,
  VKEY_EXECUTE = 0x2B,
  VKEY_SNAPSHOT = 0x2C,
  VKEY_INSERT = 0x2D,
  VKEY_DELETE = 0x2E,
  VKEY_HELP = 0x2F,
  VKEY_0 = 0x30,
  VKEY_1 = 0x31,
  VKEY_2 = 0x32,
  VKEY_3 = 0x33,
  VKEY_4 = 0x34,
  VKEY_5 = 0x35,
  VKEY_6 = 0x36,
  VKEY_7 = 0x37,
  VKEY_8 = 0x38,
  VKEY_9 = 0x39,
  VKEY_A = 0x41,
  VKEY_B = 0x42,
  VKEY_C = 0x43,
  VKEY_D = 0x44,
  VKEY_E = 0x45,
  VKEY_F = 0x46,
  VKEY_G = 0x47,
  VKEY_H = 0x48,
  VKEY_I = 0x49,
  VKEY_J = 0x4A,
  VKEY_K = 0x4B,
  VKEY_L = 0x4C,
  VKEY_M = 0x4D,
  VKEY_N = 0x4E,
  VKEY_O = 0x4F,
  VKEY_P = 0x50,
  VKEY_Q = 0x51,
  VKEY_R = 0x52,
  VKEY_S = 0x53,
  VKEY_T = 0x54,
  VKEY_U = 0x55,
  VKEY_V = 0x56,
  VKEY_W = 0x57,
  VKEY_X = 0x58,
  VKEY_Y = 0x59,
  VKEY_Z = 0x5A,
  VKEY_LWIN = 0x5B,
  VKEY_COMMAND = VKEY_LWIN,  // Provide the Mac name for convenience.
  VKEY_RWIN = 0x5C,
  VKEY_APPS = 0x5D,
  VKEY_SLEEP = 0x5F,
  VKEY_NUMPAD0 = 0x60,
  VKEY_NUMPAD1 = 0x61,
  VKEY_NUMPAD2 = 0x62,
  VKEY_NUMPAD3 = 0x63,
  VKEY_NUMPAD4 = 0x64,
  VKEY_NUMPAD5 = 0x65,
  VKEY_NUMPAD6 = 0x66,
  VKEY_NUMPAD7 = 0x67,
  VKEY_NUMPAD8 = 0x68,
  VKEY_NUMPAD9 = 0x69,
  VKEY_MULTIPLY = 0x6A,
  VKEY_ADD = 0x6B,
  VKEY_SEPARATOR = 0x6C,
  VKEY_SUBTRACT = 0x6D,
  VKEY_DECIMAL = 0x6E,
  VKEY_DIVIDE = 0x6F,
  VKEY_F1 = 0x70,
  VKEY_F2 = 0x71,
  VKEY_F3 = 0x72,
  VKEY_F4 = 0x73,
  VKEY_F5 = 0x74,
  VKEY_F6 = 0x75,
  VKEY_F7 = 0x76,
  VKEY_F8 = 0x77,
  VKEY_F9 = 0x78,
  VKEY_F10 = 0x79,
  VKEY_F11 = 0x7A,
  VKEY_F12 = 0x7B,
  VKEY_F13 = 0x7C,
  VKEY_F14 = 0x7D,
  VKEY_F15 = 0x7E,
  VKEY_F16 = 0x7F,
  VKEY_F17 = 0x80,
  VKEY_F18 = 0x81,
  VKEY_F19 = 0x82,
  VKEY_F20 = 0x83,
  VKEY_F21 = 0x84,
  VKEY_F22 = 0x85,
  VKEY_F23 = 0x86,
  VKEY_F24 = 0x87,
  VKEY_NUMLOCK = 0x90,
  VKEY_SCROLL = 0x91,
  VKEY_LSHIFT = 0xA0,
  VKEY_RSHIFT = 0xA1,
  VKEY_LCONTROL = 0xA2,
  VKEY_RCONTROL = 0xA3,
  VKEY_LMENU = 0xA4,
  VKEY_RMENU = 0xA5,
  VKEY_BROWSER_BACK = 0xA6,
  VKEY_BROWSER_FORWARD = 0xA7,
  VKEY_BROWSER_REFRESH = 0xA8,
  VKEY_BROWSER_STOP = 0xA9,
  VKEY_BROWSER_SEARCH = 0xAA,
  VKEY_BROWSER_FAVORITES = 0xAB,
  VKEY_BROWSER_HOME = 0xAC,
  VKEY_VOLUME_MUTE = 0xAD,
  VKEY_VOLUME_DOWN = 0xAE,
  VKEY_VOLUME_UP = 0xAF,
  VKEY_MEDIA_NEXT_TRACK = 0xB0,
  VKEY_MEDIA_PREV_TRACK = 0xB1,
  VKEY_MEDIA_STOP = 0xB2,
  VKEY_MEDIA_PLAY_PAUSE = 0xB3,
  VKEY_MEDIA_LAUNCH_MAIL = 0xB4,
  VKEY_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5,
  VKEY_MEDIA_LAUNCH_APP1 = 0xB6,
  VKEY_MEDIA_LAUNCH_APP2 = 0xB7,
  VKEY_OEM_1 = 0xBA,
  VKEY_OEM_PLUS = 0xBB,
  VKEY_OEM_COMMA = 0xBC,
  VKEY_OEM_MINUS = 0xBD,
  VKEY_OEM_PERIOD = 0xBE,
  VKEY_OEM_2 = 0xBF,
  VKEY_OEM_3 = 0xC0,
  VKEY_OEM_4 = 0xDB,
  VKEY_OEM_5 = 0xDC,
  VKEY_OEM_6 = 0xDD,
  VKEY_OEM_7 = 0xDE,
  VKEY_OEM_8 = 0xDF,
  VKEY_OEM_102 = 0xE2,
  VKEY_PROCESSKEY = 0xE5,
  VKEY_PACKET = 0xE7,
  VKEY_DBE_SBCSCHAR = 0xF3,
  VKEY_DBE_DBCSCHAR = 0xF4,
  VKEY_ATTN = 0xF6,
  VKEY_CRSEL = 0xF7,
  VKEY_EXSEL = 0xF8,
  VKEY_EREOF = 0xF9,
  VKEY_PLAY = 0xFA,
  VKEY_ZOOM = 0xFB,
  VKEY_NONAME = 0xFC,
  VKEY_PA1 = 0xFD,
  VKEY_OEM_CLEAR = 0xFE,
  VKEY_UNKNOWN = 0
};

//from ui/base/keycodes/keyboard_code_conversion_x.cc in chromium
U32 WebEngine::getVKCodefromX(U32 keysym)
{
  switch (keysym)
  {
    case XK_BackSpace:
      return VKEY_BACK;
    case XK_Delete:
    case XK_KP_Delete:
      return VKEY_DELETE;
    case XK_Tab:
    case XK_KP_Tab:
    case XK_ISO_Left_Tab:
      return VKEY_TAB;
    case XK_Linefeed:
    case XK_Return:
    case XK_KP_Enter:
    case XK_ISO_Enter:
      return VKEY_RETURN;
    case XK_Clear:
    case XK_KP_Begin:  // NumPad 5 without Num Lock, for crosbug.com/29169.
      return VKEY_CLEAR;
    case XK_KP_Space:
    case XK_space:
      return VKEY_SPACE;
    case XK_Home:
    case XK_KP_Home:
      return VKEY_HOME;
    case XK_End:
    case XK_KP_End:
      return VKEY_END;
    case XK_Page_Up:
    case XK_KP_Page_Up:
      return VKEY_PRIOR;
    case XK_Page_Down:
    case XK_KP_Page_Down:
      return VKEY_NEXT;
    case XK_Left:
    case XK_KP_Left:
      return VKEY_LEFT;
    case XK_Right:
    case XK_KP_Right:
      return VKEY_RIGHT;
    case XK_Down:
    case XK_KP_Down:
      return VKEY_DOWN;
    case XK_Up:
    case XK_KP_Up:
      return VKEY_UP;
    case XK_Escape:
      return VKEY_ESCAPE;
    case XK_Kana_Lock:
    case XK_Kana_Shift:
      return VKEY_KANA;
    case XK_Hangul:
      return VKEY_HANGUL;
    case XK_Hangul_Hanja:
      return VKEY_HANJA;
    case XK_Kanji:
      return VKEY_KANJI;
    case XK_Henkan:
      return VKEY_CONVERT;
    case XK_Muhenkan:
      return VKEY_NONCONVERT;
    case XK_Zenkaku_Hankaku:
      return VKEY_DBE_DBCSCHAR;
    case XK_A:
    case XK_a:
      return VKEY_A;
    case XK_B:
    case XK_b:
      return VKEY_B;
    case XK_C:
    case XK_c:
      return VKEY_C;
    case XK_D:
    case XK_d:
      return VKEY_D;
    case XK_E:
    case XK_e:
      return VKEY_E;
    case XK_F:
    case XK_f:
      return VKEY_F;
    case XK_G:
    case XK_g:
      return VKEY_G;
    case XK_H:
    case XK_h:
      return VKEY_H;
    case XK_I:
    case XK_i:
      return VKEY_I;
    case XK_J:
    case XK_j:
      return VKEY_J;
    case XK_K:
    case XK_k:
      return VKEY_K;
    case XK_L:
    case XK_l:
      return VKEY_L;
    case XK_M:
    case XK_m:
      return VKEY_M;
    case XK_N:
    case XK_n:
      return VKEY_N;
    case XK_O:
    case XK_o:
      return VKEY_O;
    case XK_P:
    case XK_p:
      return VKEY_P;
    case XK_Q:
    case XK_q:
      return VKEY_Q;
    case XK_R:
    case XK_r:
      return VKEY_R;
    case XK_S:
    case XK_s:
      return VKEY_S;
    case XK_T:
    case XK_t:
      return VKEY_T;
    case XK_U:
    case XK_u:
      return VKEY_U;
    case XK_V:
    case XK_v:
      return VKEY_V;
    case XK_W:
    case XK_w:
      return VKEY_W;
    case XK_X:
    case XK_x:
      return VKEY_X;
    case XK_Y:
    case XK_y:
      return VKEY_Y;
    case XK_Z:
    case XK_z:
      return VKEY_Z;

    case XK_0:
    case XK_1:
    case XK_2:
    case XK_3:
    case XK_4:
    case XK_5:
    case XK_6:
    case XK_7:
    case XK_8:
    case XK_9:
      return static_cast<PosixKeyboardCode>(VKEY_0 + (keysym - XK_0));

    case XK_parenright:
      return VKEY_0;
    case XK_exclam:
      return VKEY_1;
    case XK_at:
      return VKEY_2;
    case XK_numbersign:
      return VKEY_3;
    case XK_dollar:
      return VKEY_4;
    case XK_percent:
      return VKEY_5;
    case XK_asciicircum:
      return VKEY_6;
    case XK_ampersand:
      return VKEY_7;
    case XK_asterisk:
      return VKEY_8;
    case XK_parenleft:
      return VKEY_9;

    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9:
      return static_cast<PosixKeyboardCode>(VKEY_NUMPAD0 + (keysym - XK_KP_0));

    case XK_multiply:
    case XK_KP_Multiply:
      return VKEY_MULTIPLY;
    case XK_KP_Add:
      return VKEY_ADD;
    case XK_KP_Separator:
      return VKEY_SEPARATOR;
    case XK_KP_Subtract:
      return VKEY_SUBTRACT;
    case XK_KP_Decimal:
      return VKEY_DECIMAL;
    case XK_KP_Divide:
      return VKEY_DIVIDE;
    case XK_equal:
    case XK_plus:
      return VKEY_OEM_PLUS;
    case XK_comma:
    case XK_less:
      return VKEY_OEM_COMMA;
    case XK_minus:
    case XK_underscore:
      return VKEY_OEM_MINUS;
    case XK_greater:
    case XK_period:
      return VKEY_OEM_PERIOD;
    case XK_colon:
    case XK_semicolon:
      return VKEY_OEM_1;
    case XK_question:
    case XK_slash:
      return VKEY_OEM_2;
    case XK_asciitilde:
    case XK_quoteleft:
      return VKEY_OEM_3;
    case XK_bracketleft:
    case XK_braceleft:
      return VKEY_OEM_4;
    case XK_backslash:
    case XK_bar:
      return VKEY_OEM_5;
    case XK_bracketright:
    case XK_braceright:
      return VKEY_OEM_6;
    case XK_quoteright:
    case XK_quotedbl:
      return VKEY_OEM_7;
    case XK_Shift_L:
    case XK_Shift_R:
      return VKEY_SHIFT;
    case XK_Control_L:
    case XK_Control_R:
      return VKEY_CONTROL;
    case XK_Meta_L:
    case XK_Meta_R:
    case XK_Alt_L:
    case XK_Alt_R:
      return VKEY_MENU;
    case XK_Pause:
      return VKEY_PAUSE;
    case XK_Caps_Lock:
      return VKEY_CAPITAL;
    case XK_Num_Lock:
      return VKEY_NUMLOCK;
    case XK_Scroll_Lock:
      return VKEY_SCROLL;
    case XK_Select:
      return VKEY_SELECT;
    case XK_Print:
      return VKEY_PRINT;
    case XK_Execute:
      return VKEY_EXECUTE;
    case XK_Insert:
    case XK_KP_Insert:
      return VKEY_INSERT;
    case XK_Help:
      return VKEY_HELP;
    case XK_Super_L:
      return VKEY_LWIN;
    case XK_Super_R:
      return VKEY_RWIN;
    case XK_Menu:
      return VKEY_APPS;
    case XK_F1:
    case XK_F2:
    case XK_F3:
    case XK_F4:
    case XK_F5:
    case XK_F6:
    case XK_F7:
    case XK_F8:
    case XK_F9:
    case XK_F10:
    case XK_F11:
    case XK_F12:
    case XK_F13:
    case XK_F14:
    case XK_F15:
    case XK_F16:
    case XK_F17:
    case XK_F18:
    case XK_F19:
    case XK_F20:
    case XK_F21:
    case XK_F22:
    case XK_F23:
    case XK_F24:
      return static_cast<PosixKeyboardCode>(VKEY_F1 + (keysym - XK_F1));
  }

  return VKEY_UNKNOWN;
}


void WebEngine::_buildKeyMaps()
{
   dMemset(TorqueToVK, 0, 256);

   TorqueToVK[KEY_A] = XK_a;
   TorqueToVK[KEY_B] = XK_b;
   TorqueToVK[KEY_C] = XK_c;
   TorqueToVK[KEY_D] = XK_d;
   TorqueToVK[KEY_E] = XK_e;
   TorqueToVK[KEY_F] = XK_f;
   TorqueToVK[KEY_G] = XK_g;
   TorqueToVK[KEY_H] = XK_h;
   TorqueToVK[KEY_I] = XK_i;
   TorqueToVK[KEY_J] = XK_j;
   TorqueToVK[KEY_K] = XK_k;
   TorqueToVK[KEY_L] = XK_l;
   TorqueToVK[KEY_M] = XK_m;
   TorqueToVK[KEY_N] = XK_n;
   TorqueToVK[KEY_O] = XK_o;
   TorqueToVK[KEY_P] = XK_p;
   TorqueToVK[KEY_Q] = XK_q;
   TorqueToVK[KEY_R] = XK_r;
   TorqueToVK[KEY_S] = XK_s;
   TorqueToVK[KEY_T] = XK_t;
   TorqueToVK[KEY_U] = XK_u;
   TorqueToVK[KEY_V] = XK_v;
   TorqueToVK[KEY_W] = XK_w;
   TorqueToVK[KEY_X] = XK_x;
   TorqueToVK[KEY_Y] = XK_y;
   TorqueToVK[KEY_Z] = XK_z;

   TorqueToVK[KEY_1] = XK_1;
   TorqueToVK[KEY_2] = XK_2;
   TorqueToVK[KEY_3] = XK_3;
   TorqueToVK[KEY_4] = XK_4;
   TorqueToVK[KEY_5] = XK_5;
   TorqueToVK[KEY_6] = XK_6;
   TorqueToVK[KEY_7] = XK_7;
   TorqueToVK[KEY_8] = XK_8;
   TorqueToVK[KEY_9] = XK_9;
   TorqueToVK[KEY_0] = XK_0;

   TorqueToVK[KEY_CONTROL] = 0; //no mapping, see right/left version
   TorqueToVK[KEY_ALT] = 0; //no mapping, see right/left version
   TorqueToVK[KEY_SHIFT] = 0; //no mapping, see right/left version
   TorqueToVK[KEY_LCONTROL] = XK_Control_L;
   TorqueToVK[KEY_RCONTROL] = XK_Control_R;
   TorqueToVK[KEY_LALT] = XK_Alt_L;
   TorqueToVK[KEY_RALT] = XK_Alt_R;
   TorqueToVK[KEY_LSHIFT] = XK_Shift_L;
   TorqueToVK[KEY_RSHIFT] = XK_Shift_R;

   TorqueToVK[KEY_BACKSPACE] = XK_BackSpace;
   TorqueToVK[KEY_TAB] = XK_Tab;
   TorqueToVK[KEY_RETURN] = XK_Return;
   TorqueToVK[KEY_PAUSE] = XK_Pause;
   TorqueToVK[KEY_CAPSLOCK] = XK_Caps_Lock;
   TorqueToVK[KEY_ESCAPE] = XK_Escape;
   TorqueToVK[KEY_SPACE] = XK_space;
   TorqueToVK[KEY_PAGE_DOWN] = XK_Page_Down;
   TorqueToVK[KEY_PAGE_UP] = XK_Page_Up;
   TorqueToVK[KEY_END] = XK_End;
   TorqueToVK[KEY_HOME] = XK_Home;
   TorqueToVK[KEY_LEFT] = XK_Left;
   TorqueToVK[KEY_UP] = XK_Up;
   TorqueToVK[KEY_RIGHT] = XK_Right;
   TorqueToVK[KEY_DOWN] = XK_Down;
   TorqueToVK[KEY_PRINT] = XK_Print;
   TorqueToVK[KEY_INSERT] = XK_Insert;
   TorqueToVK[KEY_DELETE] = XK_Delete;
   TorqueToVK[KEY_HELP] = XK_Help;

   TorqueToVK[KEY_TILDE] = XK_grave;
   TorqueToVK[KEY_MINUS] = XK_minus;
   TorqueToVK[KEY_EQUALS] = XK_equal;
   TorqueToVK[KEY_LBRACKET] = XK_braceleft;
   TorqueToVK[KEY_RBRACKET] = XK_braceright;
   TorqueToVK[KEY_BACKSLASH] = XK_backslash;
   TorqueToVK[KEY_SEMICOLON] = XK_semicolon;
   TorqueToVK[KEY_APOSTROPHE] = XK_apostrophe;
   TorqueToVK[KEY_COMMA] = XK_comma;
   TorqueToVK[KEY_PERIOD] = XK_period;
   TorqueToVK[KEY_SLASH] = XK_slash;
   TorqueToVK[KEY_NUMPAD0] = XK_KP_0;
   TorqueToVK[KEY_NUMPAD1] = XK_KP_1;
   TorqueToVK[KEY_NUMPAD2] = XK_KP_2;
   TorqueToVK[KEY_NUMPAD3] = XK_KP_3;
   TorqueToVK[KEY_NUMPAD4] = XK_KP_4;
   TorqueToVK[KEY_NUMPAD5] = XK_KP_5;
   TorqueToVK[KEY_NUMPAD6] = XK_KP_6;
   TorqueToVK[KEY_NUMPAD7] = XK_KP_7;
   TorqueToVK[KEY_NUMPAD8] = XK_KP_8;
   TorqueToVK[KEY_NUMPAD9] = XK_KP_9;
   TorqueToVK[KEY_MULTIPLY] = XK_KP_Multiply;
   TorqueToVK[KEY_ADD] = XK_KP_Add;
   TorqueToVK[KEY_SEPARATOR] = XK_KP_Separator;
   TorqueToVK[KEY_SUBTRACT] = XK_KP_Subtract;
   TorqueToVK[KEY_DECIMAL] = XK_KP_Decimal;
   TorqueToVK[KEY_DIVIDE] = XK_KP_Divide;
   TorqueToVK[KEY_NUMPADENTER] = XK_KP_Enter;

   TorqueToVK[KEY_F1] = XK_F1;
   TorqueToVK[KEY_F2] = XK_F2;
   TorqueToVK[KEY_F3] = XK_F3;
   TorqueToVK[KEY_F4] = XK_F4;
   TorqueToVK[KEY_F5] = XK_F5;
   TorqueToVK[KEY_F6] = XK_F6;
   TorqueToVK[KEY_F7] = XK_F7;
   TorqueToVK[KEY_F8] = XK_F8;
   TorqueToVK[KEY_F9] = XK_F9;
   TorqueToVK[KEY_F10] = XK_F10;
   TorqueToVK[KEY_F11] = XK_F11;
   TorqueToVK[KEY_F12] = XK_F12;
   TorqueToVK[KEY_F13] = XK_F13;
   TorqueToVK[KEY_F14] = XK_F14;
   TorqueToVK[KEY_F15] = XK_F15;
   TorqueToVK[KEY_F16] = XK_F16;
   TorqueToVK[KEY_F17] = XK_F17;
   TorqueToVK[KEY_F18] = XK_F18;
   TorqueToVK[KEY_F19] = XK_F19;
   TorqueToVK[KEY_F20] = XK_F20;
   TorqueToVK[KEY_F21] = XK_F21;
   TorqueToVK[KEY_F22] = XK_F22;
   TorqueToVK[KEY_F23] = XK_F23;
   TorqueToVK[KEY_F24] = XK_F24;
}

#endif



const U8 WebEngine::CEFCursorToTorque[] =
{
   PlatformCursorController::curArrow, //CT_POINTER = 0,
   PlatformCursorController::curPlus, //CT_CROSS,
   PlatformCursorController::curHand, //CT_HAND,
   PlatformCursorController::curIBeam, //CT_IBEAM,
   PlatformCursorController::curWait, //CT_WAIT,
   PlatformCursorController::curArrow, //CT_HELP,
   PlatformCursorController::curResizeHorz, //CT_EASTRESIZE,
   PlatformCursorController::curResizeVert, //CT_NORTHRESIZE,
   PlatformCursorController::curResizeNESW, //CT_NORTHEASTRESIZE,
   PlatformCursorController::curResizeNWSE, //CT_NORTHWESTRESIZE,
   PlatformCursorController::curResizeVert, //CT_SOUTHRESIZE,
   PlatformCursorController::curResizeNWSE, //CT_SOUTHEASTRESIZE,
   PlatformCursorController::curResizeNESW, //CT_SOUTHWESTRESIZE,
   PlatformCursorController::curResizeHorz, //CT_WESTRESIZE,
   PlatformCursorController::curResizeVert, //CT_NORTHSOUTHRESIZE,
   PlatformCursorController::curResizeHorz, //CT_EASTWESTRESIZE,
   PlatformCursorController::curResizeNESW, //CT_NORTHEASTSOUTHWESTRESIZE,
   PlatformCursorController::curResizeNWSE, //CT_NORTHWESTSOUTHEASTRESIZE,
   PlatformCursorController::curResizeHorz, //CT_COLUMNRESIZE,
   PlatformCursorController::curResizeVert, //CT_ROWRESIZE,
   PlatformCursorController::curResizeAll, //CT_MIDDLEPANNING,
   PlatformCursorController::curResizeHorz, //CT_EASTPANNING,
   PlatformCursorController::curResizeVert, //CT_NORTHPANNING,
   PlatformCursorController::curResizeNESW, //CT_NORTHEASTPANNING,
   PlatformCursorController::curResizeNWSE, //CT_NORTHWESTPANNING,
   PlatformCursorController::curResizeVert, //CT_SOUTHPANNING,
   PlatformCursorController::curResizeNWSE, //CT_SOUTHEASTPANNING,
   PlatformCursorController::curResizeNESW, //CT_SOUTHWESTPANNING,
   PlatformCursorController::curResizeHorz, //CT_WESTPANNING,
   PlatformCursorController::curResizeAll, //CT_MOVE,
   PlatformCursorController::curIBeam, //CT_VERTICALTEXT,
   PlatformCursorController::curArrow, //CT_CELL,
   PlatformCursorController::curArrow, //CT_CONTEXTMENU,
   PlatformCursorController::curArrow, //CT_ALIAS,
   PlatformCursorController::curArrow, //CT_PROGRESS,
   PlatformCursorController::curNoNo, //CT_NODROP, // If you get an error on this line, you need to pull:
   // https://github.com/OTHGMars/Torque3D "MouseCursors" branch. Pending PR #2249
   PlatformCursorController::curArrow, //CT_COPY,
   PlatformCursorController::curNoNo, //CT_NONE,
   PlatformCursorController::curNoNo, //CT_NOTALLOWED,
   PlatformCursorController::curArrow, //CT_ZOOMIN,
   PlatformCursorController::curArrow, //CT_ZOOMOUT,
   PlatformCursorController::curHand, //CT_GRAB,
   PlatformCursorController::curHand, //CT_GRABBING,
   PlatformCursorController::curArrow, //CT_CUSTOM,
};
